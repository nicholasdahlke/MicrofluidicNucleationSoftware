//
// Created by nicholas on 23.02.25.
//
#include <MicrofluidicNucleation/VideoAnalyzer.h>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <tuple>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>
#include <execution>
#include <chrono>
#include <numbers>
#include <ranges>

mfn::VideoAnalyzer::VideoAnalyzer(const mfn::Experiment &experiment, const AnalysisConfig &config, const TemperatureReader &temperature)
{
    VideoAnalyzer::experiment = experiment;
    VideoAnalyzer::config = config;
    VideoAnalyzer::temperatureReader = temperature;
    distribution = std::uniform_int_distribution<>(50, 255);
    yolo = std::make_shared<YoloV5>(config);
    yolo->open(config);
}


void mfn::VideoAnalyzer::openCapture()
{
    video_capture.open(experiment.getVideo().string());
    volume_images_path = experiment.getVideo().parent_path().string() + "/" + experiment.getVideo().stem().string() + "-volume-images/";
    std::filesystem::create_directories(volume_images_path);
    if(!video_capture.isOpened())
        spdlog::get("mfn_logger")->error("An error occurred opening the video capture.");

    if(video_capture.get(cv::CAP_PROP_FRAME_COUNT) <= 0)
        spdlog::get("mfn_logger")->error("No frames found in video.");

    frame_size = cv::Size(static_cast<int>(video_capture.get(cv::CAP_PROP_FRAME_WIDTH)), static_cast<int>(video_capture.get(cv::CAP_PROP_FRAME_HEIGHT)));

    if(frame_size.width <= 0 || frame_size.height <= 0)
        spdlog::get("mfn_logger")->error("Video dimensions must be non zero");

    spdlog::get("mfn_logger")->info("Successfully opened video with dimensions(h x w) {}px x {}px with {} frames",
                                    video_capture.get(cv::CAP_PROP_FRAME_HEIGHT),
                                    video_capture.get(cv::CAP_PROP_FRAME_WIDTH),
                                    video_capture.get(cv::CAP_PROP_FRAME_COUNT));
}

void mfn::VideoAnalyzer::processLoop()
{
    spdlog::get("mfn_logger")->info("Starting process loop");
    const std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();

    if (config.frame_start > 0)
        video_capture.set(cv::CAP_PROP_POS_FRAMES, config.frame_start - 1);

    if (config.frame_stop <= 0)
        config.frame_stop = video_capture.get(cv::CAP_PROP_FRAME_COUNT) -1;

    int frame_id = config.frame_start;
    while (frame_id < config.frame_stop || config.frame_stop <= 0)
    {
        spdlog::get("mfn_logger")->info("Starting frame queue of size {}", config.parallel);
        std::vector<std::tuple<cv::Mat, int>> raw_frame_queue;
        while(raw_frame_queue.size() < config.parallel || (frame_id > config.frame_stop && config.frame_stop > 0))
        {
            cv::Mat temp;
            video_capture >> temp;
            if(temp.empty() || (frame_id > config.frame_stop && config.frame_stop > 0))
            {
                spdlog::get("mfn_logger")->info("End of video reached");
                break;
            }
            raw_frame_queue.emplace_back(temp, frame_id);
            frame_id++;
        }

        std::vector<mfn::Frame> frame_queue = detectDroplets(raw_frame_queue);

        // Detect collisions
        spdlog::get("mfn_logger")->info("Detecting collisions");
        std::for_each(
            std::execution::par,
            frame_queue.begin(),
            frame_queue.end(),
            detectCollision
        );

        // Detect contours
        spdlog::get("mfn_logger")->info("Detecting contours");
        std::for_each(
            std::execution::par,
            frame_queue.begin(),
            frame_queue.end(),
            [&](mfn::Frame & frame){detectContour(frame, volume_images_path);}
        );

        //Delete the droplet images
        /*spdlog::get("mfn_logger")->info("Deleting droplet images");
        std::for_each(
            std::execution::par,
            frame_queue.begin(),
            frame_queue.end(),
            [&](mfn::Frame & frame) {
                std::for_each(
                    std::execution::par,
                    frame.droplets.begin(),
                    frame.droplets.end(),
                    [&](mfn::Droplet & droplet) {droplet.clearDropletImage();});
            });*/

        frames.insert(frames.end(), frame_queue.begin(), frame_queue.end());
        spdlog::get("mfn_logger")->info("Preprocessing queue finished");

    }
    spdlog::get("mfn_logger")->info("Preprocessing loop finished");

    spdlog::get("mfn_logger")->info("Calculating displacement");
    calculateDisplacement(frames);

    spdlog::get("mfn_logger")->info("Saving the droplets");
    for (const Frame & frame : frames)
    {
        for (const Droplet & droplet : frame.droplets)
        {
            dropletHeap.emplace_back(frame, droplet);
        }
    }

    spdlog::get("mfn_logger")->info("Counting droplets");
    std::for_each(
        std::execution::par,
        frames.begin(),
        frames.end(),
        [this](mfn::Frame & frame){this->replaceDroplets(frame);}
        );

    spdlog::get("mfn_logger")->info("Writing temperature");
    std::for_each(
        std::execution::par,
        frames.begin(),
        frames.end(),
        [&](mfn::Frame & frame) {writeTemperature(frame, temperatureReader);}
        );

    // Delete the yolo object, as it consumes too much memory
    yolo.reset();
    const std::chrono::duration<double> diff = std::chrono::high_resolution_clock::now() - begin;
    spdlog::get("mfn_logger")->info("Processing loop done in {:.2f}min.", diff.count()/60.0f);
}

void mfn::VideoAnalyzer::writeTemperature(mfn::Frame &frame, const mfn::TemperatureReader &temperature)
{
    frame.setTemperature(temperature.getTemperature(frame.getTime()));
}



std::vector<mfn::Frame> mfn::VideoAnalyzer::detectDroplets(const std::vector<frame_transfer_t> &input)
{
    std::vector<cv::Mat> frame_mats;
    for (const frame_transfer_t & frame : input)
        frame_mats.push_back(std::get<0>(frame));

    std::vector<mfn::Frame> frames_detected;
    std::vector<std::vector<mfn::Detection>> detections = yolo->process(frame_mats);
    for (size_t i = 0; i < detections.size(); i++)
    {
        Frame frame(static_cast<double>((std::get<1>(input[i])) / experiment.getParameter("frame_rate"))*1000);
        for (const mfn::Detection & detection : detections[i])
        {
            mfn::Droplet droplet(detection);
            if (droplet.isFrozen())
                droplet.setIgnore(true);
            cv::Rect detection_rect = enlargeRect(detection.getRect());
            if (isInsideFrame(detection_rect, frame_mats[i].size()))
            {
                cv::Mat droplet_image = frame_mats[i](detection_rect).clone();
                droplet.setDropletImage(droplet_image);
            }
            else
            {
                droplet.setIgnore(true);
            }

            frame.droplets.push_back(droplet);
        }
        frames_detected.push_back(frame);
        spdlog::get("mfn_logger")->info("Preprocessed frame {} with {} droplets detected", std::get<1>(input[i]), frame.droplets.size());
    }
    return frames_detected;
}


void mfn::VideoAnalyzer::detectCollision(mfn::Frame & frame)
{
    for (mfn::Droplet & droplet : frame.droplets)
    {
        for (mfn::Droplet & droplet2 : frame.droplets)
        {
            cv::Rect r1 = enlargeRect(droplet.getDetection().getRect());
            cv::Rect r2 = enlargeRect(droplet2.getDetection().getRect());
            if
            (
                r1 != r2 &&
                (r1 & r2).area() > 0
            )
            {
                droplet.setIgnore(true);
                droplet2.setIgnore(true);
                spdlog::get("mfn_logger")->info("Droplet collision detected at t={}s.", frame.getTime());
            }
        }
    }
}

void mfn::VideoAnalyzer::detectContour(mfn::Frame &frame, std::filesystem::path &image_path)
{
    if (!frame.droplets.empty())
    {
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        for (mfn::Droplet & droplet: frame.droplets)
        {
            if (!droplet.getIgnore())
            {
                cv::Mat droplet_image = droplet.getDropletImage().clone();
                constexpr float down_factor = 0.5;
                cv::resize(droplet_image, droplet_image, cv::Size(0,0), down_factor, down_factor);

                cv::fastNlMeansDenoising(droplet_image, droplet_image, 10, 7, 21);

                cv::Mat temp;
                cv::GaussianBlur(droplet_image, temp, cv::Size(0, 0), 10);
                cv::addWeighted(droplet_image, 4, temp, -3, 0, droplet_image);

                cv::Mat bgdModel, fgdModel, outMask;
                constexpr float scaling = 0.9;

                cv::Rect grabcut_rect;

                // Define the rectangle containing the droplet
                grabcut_rect.x = (droplet_image.size[0] * (1 - scaling))/2;
                grabcut_rect.y = (droplet_image.size[1] * (1 - scaling))/2;
                grabcut_rect.height = droplet_image.size[0] * scaling;
                grabcut_rect.width = droplet_image.size[1] * scaling;

                cv::grabCut(droplet_image,
                    outMask,
                    grabcut_rect,
                    bgdModel,
                    fgdModel,
                    5,
                    cv::GC_INIT_WITH_RECT);

                cv::Mat lookUpTable(1, 256, CV_8U);
                uint8_t * p = lookUpTable.data;
                for(size_t j = 0; j < 256; ++j)
                    p[j] = 255;
                p[0] = 0;
                p[2] = 0;
                cv::LUT(outMask, lookUpTable, outMask);

                cv::resize(outMask, outMask, cv::Size(0, 0), 1.0/down_factor, 1.0/down_factor);
                cv::resize(droplet_image, droplet_image, cv::Size(0, 0), 1.0/down_factor, 1.0/down_factor);
                std::vector<std::vector<cv::Point> > contours;
                cv::findContours(outMask, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
                if (!contours.empty())
                {
                    // Find contour with the largest area
                    std::vector<cv::Point> contour = *std::ranges::max_element(contours,
                                                                               [](const std::vector<cv::Point> &a,
                                                                                  const std::vector<cv::Point> &b) -> bool {
                                                                                   return cv::contourArea(a) <
                                                                                          cv::contourArea(b);
                                                                               });
                    if (contour.size() > 5) // Check if enough points are present
                    {
                        cv::RotatedRect ellipse_fit = cv::fitEllipse(contour);
                        double ellipse_area = std::numbers::pi * ellipse_fit.size.width * ellipse_fit.size.height;
                        double error = abs(ellipse_area - cv::contourArea(contour));
                        spdlog::get("mfn_logger")->info("Ellipse fitted with an accuracy of {:.2}%", 100*(error/ellipse_area));
                        cv::Scalar color = droplet.isFrozen() ? cv::Scalar(255, 0, 0) : cv::Scalar(0, 0, 255);
                        cv::ellipse(droplet_image, ellipse_fit, color, 2);
                        cv::imwrite(image_path.string() + std::to_string(frame.getTime()) + ".png", droplet_image);
                        ellipse_fit.center.x += droplet.getDetection().getRect().x;
                        ellipse_fit.center.y += droplet.getDetection().getRect().y;
                        droplet.setEllipse(ellipse_fit);
                    }
                    else
                    {
                        droplet.setIgnore(true);
                    }
                }
                else
                {
                    droplet.setIgnore(true);
                }

            }
        }
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        spdlog::get("mfn_logger")->info("Contour detection for t={}s finished in {}ms",
            frame.getTime(),
            std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());

    }
}

int mfn::VideoAnalyzer::calculateDisplacement(mfn::Droplet &droplet, const mfn::Frame &next_frame) const
{
    const cv::Point droplet_center = droplet.getMidpoint();
    if (droplet_center.x > config.right_border_displacement)
    {
        spdlog::get("mfn_logger")->warn("Droplet behind border, not calculating displacement.");
        return 0;
    }
    std::vector<mfn::Vector2D> displacements;
    for (const mfn::Droplet & next_droplet : next_frame.droplets)
    {
        mfn::Vector2D displacement({
            static_cast<double>(next_droplet.getMidpoint().x - droplet_center.x),
            static_cast<double>(next_droplet.getMidpoint().y - droplet_center.y)});

        if (displacement.content[0] > 0 && displacement.get_length() < config.max_movement_threshold_displacement) // Check if displacement is positive and smaller than threshold
            displacements.push_back(displacement);
    }



    if (displacements.empty() && next_frame.droplets.empty())
    {
        spdlog::get("mfn_logger")->warn("No droplets present in next frame. This is probably normal.");
        return 0;
    }

    if (displacements.empty())
        return 0;

    const mfn::Vector2D displacement = *std::ranges::min_element(displacements,
                                                           [](const mfn::Vector2D &a,
                                                              const mfn::Vector2D &b) -> bool {
                                                               return a.get_length() <
                                                                     b.get_length();
                                                           });
    droplet.setMovement(displacement);
    return static_cast<int>(displacements.size());
}

void mfn::VideoAnalyzer::calculateDisplacement(std::vector<mfn::Frame> &frames) const
{
    std::ranges::sort(frames,[](const mfn::Frame &a, const mfn::Frame &b) -> bool{return a.getTime() < b.getTime();});
    for (size_t i = 1; i < frames.size()-1; i++)
    {
        for (mfn::Droplet & droplet : frames[i].droplets)
        {
            if (calculateDisplacement(droplet, frames[i+1]) == 0)
                spdlog::get("mfn_logger")->warn("Displacement for frame at t={}s not calculated.", frames[i].getTime());
        }
    }
}

void mfn::VideoAnalyzer::showDisplacement(const std::vector<mfn::Frame> & frames)
{
    cv::Mat displacement_image = cv::Mat::zeros(frame_size, CV_8UC3);
    for (const mfn::Frame & frame : frames)
    {
        for (const mfn::Droplet & droplet : frame.droplets)
        {

            if (droplet.getMovement().content[0] != 0 && droplet.getMovement().content[1] != 0)
            {
                cv::circle(displacement_image, droplet.getMidpoint(), 5, cv::Scalar(0, 0, 255), 2);
                const cv::Point displaced_point = droplet.getMidpoint() + droplet.getMovement().get_point();
                const int frame_middle = frame_size.width / 2;
                int thickness = 1;
                if (droplet.getMidpoint().x < frame_middle && displaced_point.x > frame_middle) // If the displacement crossed the frame middle
                {
                    thickness = 4;
                }
                cv::line(displacement_image, droplet.getMidpoint(), displaced_point, getRandomColor(), thickness);
                cv::imshow("displacement", displacement_image);
                cv::waitKey(500);
            }
        }
    }
}

void mfn::VideoAnalyzer::replaceDroplets(mfn::Frame &frame) const
{
    bool droplet_found = false;
    for (mfn::Droplet & droplet : frame.droplets)
    {
        mfn::Vector2D displacement = droplet.getMovement();
        const int frame_middle = frame_size.width / 2;
        if (droplet.getMidpoint().x < frame_middle && (droplet.getMidpoint() + displacement.get_point()).x > frame_middle)
        {
            frame.droplets.clear();
            frame.droplets.push_back(droplet);
            droplet_found = true;
            break;
        }
    }
    if (!droplet_found)
        frame.droplets.clear();
}


cv::Rect mfn::VideoAnalyzer::enlargeRect(cv::Rect _rect, const double _factor)
{
    const int pre_width = _rect.width;
    const int pre_height = _rect.height;
    _rect.width *= _factor;
    _rect.height *= _factor;
    _rect.x -= (_rect.width-pre_width)/2;
    _rect.y -= (_rect.height-pre_height)/2;
    return _rect;
}

bool mfn::VideoAnalyzer::isInsideFrame(const cv::Rect _rect, const cv::Size _size)
{
    if (_rect.x < 0 || _rect.y < 0)
        return false;

    if (_rect.x + _rect.width > _size.width || _rect.y + _rect.height > _size.height)
        return false;

    return true;
}

void mfn::VideoAnalyzer::analyze()
{
    openCapture();
    processLoop();
}

std::vector<mfn::Frame> mfn::VideoAnalyzer::getFrames() const
{
    return frames;
}

std::vector<mfn::DropletResult> mfn::VideoAnalyzer::getDropletHeap() const
{
    return dropletHeap;
}

cv::Scalar mfn::VideoAnalyzer::getRandomColor()
{
    const double c1 = distribution(generator);
    const double c2 = distribution(generator);
    const double  c3 = distribution(generator);
    return {c1, c2, c3};
}

mfn::Experiment mfn::VideoAnalyzer::getExperiment() const
{
    return experiment;
}