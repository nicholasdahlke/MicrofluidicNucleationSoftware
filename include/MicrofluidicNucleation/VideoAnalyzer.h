//
// Created by nicholas on 23.02.25.
//

#ifndef MICROFLUIDICNUCLEATIONSOFTWARE_VIDEOANALYZER_H
#define MICROFLUIDICNUCLEATIONSOFTWARE_VIDEOANALYZER_H
#include <MicrofluidicNucleation/Experiment.h>
#include <MicrofluidicNucleation/Frame.h>
#include <MicrofluidicNucleation/AnalysisConfig.h>
#include <MicrofluidicNucleation/YoloV5.h>
#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include <memory>

namespace mfn
{
    class VideoAnalyzer
    {
    public:
        explicit VideoAnalyzer(const Experiment &experiment, const AnalysisConfig &config);
        void analyze();

    private:
        mfn::Experiment experiment;
        mfn::AnalysisConfig config;
        cv::VideoCapture video_capture;

        std::vector<mfn::Frame> frames;
        YoloV5 yolo;

        void openCapture();
        void processLoop();
        mfn::Frame preprocessFrame(const cv::Mat & input, int frame_id);
    };

}
#endif //MICROFLUIDICNUCLEATIONSOFTWARE_VIDEOANALYZER_H
