//
// Created by nicholas on 23.02.25.
//
#include <MicrofluidicNucleation/VideoAnalyzer.h>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <tuple>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/photo.hpp>
#include <execution>
#include <chrono>

mfn::VideoAnalyzer::VideoAnalyzer(const mfn::Experiment &experiment, const AnalysisConfig &config)
{
    VideoAnalyzer::experiment = experiment;
    VideoAnalyzer::config = config;
    yolo.open(config);
}


void mfn::VideoAnalyzer::openCapture()
{
    video_capture.open(experiment.getVideo().string());
    if(!video_capture.isOpened())
        spdlog::get("mfn_logger")->error("An error occurred opening the video capture.");

    if(video_capture.get(cv::CAP_PROP_FRAME_COUNT) <= 0)
        spdlog::get("mfn_logger")->error("No frames found in video.");

    if(video_capture.get(cv::CAP_PROP_FRAME_WIDTH) <= 0 || video_capture.get(cv::CAP_PROP_FRAME_HEIGHT) <= 0)
        spdlog::get("mfn_logger")->error("Video dimensions must be non zero");

    spdlog::get("mfn_logger")->info("Successfully opened video with dimensions(h x w) {}px x {}px with {} frames",
                                    video_capture.get(cv::CAP_PROP_FRAME_HEIGHT),
                                    video_capture.get(cv::CAP_PROP_FRAME_WIDTH),
                                    video_capture.get(cv::CAP_PROP_FRAME_COUNT));
}

void mfn::VideoAnalyzer::processLoop()
{
    spdlog::get("mfn_logger")->info("Starting process loop");

    if (config.frame_start > 0)
        video_capture.set(cv::CAP_PROP_POS_FRAMES, config.frame_start - 1);

    int frame_id = config.frame_start;
    while (frame_id < config.frame_stop || config.frame_stop <= 0)
    {
        spdlog::get("mfn_logger")->info("Starting frame queue of size {}", config.parallel);
        std::vector<mfn::Frame> frame_queue;
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

        for (const frame_transfer_t & frame : raw_frame_queue)
            frame_queue.push_back(detectDroplets(frame));

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
            detectContour
        );


        spdlog::get("mfn_logger")->info("Frame queue finished");
    }

}

mfn::Frame mfn::VideoAnalyzer::detectDroplets(const frame_transfer_t & input)
{
    Frame frame(std::get<1>(input) * experiment.getFrameRate());
    std::vector<mfn::Detection> detections = yolo.process(std::get<0>(input));
    for (const mfn::Detection & detection : detections)
    {
        mfn::RawDroplet droplet(detection);
        // Store the downsampled droplet
        const float down_factor = 0.5;
        //Enlarge rect to avoid problems with border collisions
        cv::Mat droplet_image = std::get<0>(input)(enlargeRect(detection.getRect(), 1)).clone();
        resize(droplet_image, droplet_image, cv::Size(0,0), down_factor, down_factor);
        droplet.setDropletImage(droplet_image);
        frame.droplets.push_back(droplet);
    }
    spdlog::get("mfn_logger")->info("Preprocessed frame {} with {} droplets detected", std::get<1>(input), frame.droplets.size());
    return frame;
}

void mfn::VideoAnalyzer::detectCollision(mfn::Frame & frame)
{
    for (mfn::RawDroplet & droplet : frame.droplets)
    {
        for (mfn::RawDroplet & droplet2 : frame.droplets)
        {
            if
            (
                droplet.getDetection().getRect() != droplet2.getDetection().getRect() &&
                (droplet.getDetection().getRect() & droplet2.getDetection().getRect()).area() > 0
            )
            {
                droplet.setIgnore(true);
                droplet2.setIgnore(true);
                spdlog::get("mfn_logger")->info("Droplet collision detected at t={}s.", frame.getTime());
            }
        }
    }
}

void mfn::VideoAnalyzer::detectContour(mfn::Frame &frame)
{
    if (frame.droplets.size() > 0)
    {
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        for (mfn::RawDroplet & droplet: frame.droplets)
        {
            cv::Mat droplet_image = droplet.getDropletImage().clone();
            cv::fastNlMeansDenoising(droplet_image, droplet_image, 10, 7, 21);
            //cv::cvtColor(droplet_image, droplet_image, cv::COLOR_GRAY2BGR);

            cv::Mat bgdModel, fgdModel, outMask;
            const float scaling = 0.9;

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
        }
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        spdlog::get("mfn_logger")->info("Contour detection for t={}s finished in {}ms",
            frame.getTime(),
            std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());
    }
}

cv::Rect mfn::VideoAnalyzer::enlargeRect(cv::Rect _rect, const double _factor)
{
    const int pre_width = _rect.width;
    const int pre_height = _rect.height;
    _rect.width *= _factor;
    _rect.height *= _factor;
    _rect.x -= (_rect.width-pre_width)/2;
    _rect.y -= (_rect.height-pre_height)/2;
    _rect.x = std::max(_rect.x, 0);
    _rect.y = std::max(_rect.y, 0);
    return _rect;
}


void mfn::VideoAnalyzer::analyze()
{
    openCapture();
    processLoop();
}
