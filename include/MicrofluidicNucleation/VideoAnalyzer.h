//
// Created by nicholas on 23.02.25.
//

#ifndef MICROFLUIDICNUCLEATIONSOFTWARE_VIDEOANALYZER_H
#define MICROFLUIDICNUCLEATIONSOFTWARE_VIDEOANALYZER_H
#include <MicrofluidicNucleation/Experiment.h>
#include <MicrofluidicNucleation/Frame.h>
#include <MicrofluidicNucleation/AnalysisConfig.h>
#include <MicrofluidicNucleation/YoloV5.h>
#include <MicrofluidicNucleation/TemperatureReader.h>
#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include <random>

namespace mfn
{
    class VideoAnalyzer
    {
    public:
        explicit VideoAnalyzer(const Experiment &experiment, const AnalysisConfig &config, const TemperatureReader &temperature);
        VideoAnalyzer() = default;
        void analyze();
        std::vector<mfn::Frame> getFrames() const;

    private:
        typedef std::tuple<cv::Mat, int> frame_transfer_t;
        mfn::Experiment experiment;
        mfn::AnalysisConfig config;
        mfn::TemperatureReader temperatureReader;
        cv::VideoCapture video_capture;

        std::vector<mfn::Frame> frames;
        YoloV5 yolo;

        cv::Size frame_size;

        std::default_random_engine generator;
        std::uniform_int_distribution<> distribution;

        void openCapture();
        void processLoop();
        std::vector<mfn::Frame> detectDroplets(const std::vector<frame_transfer_t> & input);

        static void detectCollision(mfn::Frame & frame);
        static void detectContour(mfn::Frame & frame);
        int calculateDisplacement(mfn::Droplet & droplet, const mfn::Frame & next_frame) const;
        void calculateDisplacement(std::vector<mfn::Frame> &frames) const;
        static void writeTemperature(mfn::Frame & frame, const mfn::TemperatureReader & temperature);
        static cv::Rect enlargeRect(cv::Rect _rect, double _factor=1.2);
        static bool isInsideFrame(cv::Rect _rect, cv::Size _size);
        void showDisplacement(const std::vector<mfn::Frame> & frames);
        void replaceDroplets(mfn::Frame & frame) const;
        cv::Scalar getRandomColor();

    };

}
#endif //MICROFLUIDICNUCLEATIONSOFTWARE_VIDEOANALYZER_H
