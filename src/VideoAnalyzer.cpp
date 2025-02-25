//
// Created by nicholas on 23.02.25.
//
#include <MicrofluidicNucleation/VideoAnalyzer.h>
#include <spdlog/spdlog.h>

mfn::VideoAnalyzer::VideoAnalyzer(const mfn::Experiment &experiment)
{
    VideoAnalyzer::experiment = experiment;
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

