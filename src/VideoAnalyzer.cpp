//
// Created by nicholas on 23.02.25.
//
#include <MicrofluidicNucleation/VideoAnalyzer.h>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <tuple>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

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
        std::vector<std::tuple<cv::Mat, int>> frame_queue;
        while(frame_queue.size() < config.parallel || (frame_id > config.frame_stop && config.frame_stop > 0))
        {
            cv::Mat temp;
            video_capture >> temp;
            if(temp.empty() || (frame_id > config.frame_stop && config.frame_stop > 0))
            {
                spdlog::get("mfn_logger")->info("End of video reached");
                break;
            }
            frame_queue.emplace_back(temp, frame_id);
            frame_id++;
        }

        for (const std::tuple<cv::Mat, int> & frame : frame_queue)
            frames.push_back(preprocessFrame(std::get<0>(frame), std::get<1>(frame)));
        spdlog::get("mfn_logger")->info("Frame queue finished");
    }

}

mfn::Frame mfn::VideoAnalyzer::preprocessFrame(const cv::Mat & input, int frame_id)
{
    Frame frame(frame_id * experiment.getFrameRate());
    std::vector<mfn::Detection> detections = yolo.process(input);
    for (const mfn::Detection & detection : detections)
    {
        mfn::RawDroplet droplet(detection);
        // Store the downsampled droplet
        float down_factor = 0.5;
        cv::Mat droplet_image = input(detection.getRect()).clone();
        cv::resize(droplet_image, droplet_image, cv::Size(0,0), down_factor, down_factor);
        droplet.setDropletImage(droplet_image);
        frame.droplets.push_back(droplet);
    }
    spdlog::get("mfn_logger")->info("Preprocessed frame {} with {} droplets detected", frame_id, frame.droplets.size());
    return frame;
}


void mfn::VideoAnalyzer::analyze()
{
    openCapture();
    processLoop();
}