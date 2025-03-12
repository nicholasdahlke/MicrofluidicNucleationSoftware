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

        cv::Size frame_size;

        void openCapture();
        void processLoop();
        std::vector<mfn::Frame> detectDroplets(const std::vector<frame_transfer_t> & input);

        static void detectCollision(mfn::Frame & frame);
        static void detectContour(mfn::Frame & frame);
        void calculateDisplacement(mfn::RawDroplet & droplet, const mfn::Frame & next_frame);
        void calculateDisplacement(std::vector<mfn::Frame> &frames);
        static cv::Rect enlargeRect(cv::Rect _rect, double _factor);
        static bool isInsideFrame(cv::Rect _rect, cv::Size _size);
        void showDisplacement(const std::vector<mfn::Frame> & frames) const;
    };

}
#endif //MICROFLUIDICNUCLEATIONSOFTWARE_VIDEOANALYZER_H
