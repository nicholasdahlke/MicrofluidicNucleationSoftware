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
        typedef std::tuple<cv::Mat, int> frame_transfer_t;
        mfn::Experiment experiment;
        mfn::AnalysisConfig config;
        cv::VideoCapture video_capture;

        std::vector<mfn::Frame> frames;
        YoloV5 yolo;

        void openCapture();
        void processLoop();
        mfn::Frame detectDroplets(const frame_transfer_t & input);

        static void detectCollision(mfn::Frame & frame);
        static void detectContour(mfn::Frame & frame);
        static cv::Rect enlargeRect(cv::Rect _rect, double _factor);
    };

}
#endif //MICROFLUIDICNUCLEATIONSOFTWARE_VIDEOANALYZER_H
