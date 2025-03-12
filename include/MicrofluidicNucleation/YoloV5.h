//
// Created by nicholas on 25.02.25.
//

#ifndef MICROFLUIDICNUCLEATIONSOFTWARE_YOLOV5_H
#define MICROFLUIDICNUCLEATIONSOFTWARE_YOLOV5_H
#include <opencv2/dnn.hpp>
#include <MicrofluidicNucleation/AnalysisConfig.h>
#include <MicrofluidicNucleation/Detection.h>
#include <filesystem>
#include <vector>
#include <opencv2/core/types.hpp>

namespace mfn
{
    class YoloV5
    {
    public:
        explicit YoloV5(const mfn::AnalysisConfig &config);
        YoloV5() = default;

        void open(const mfn::AnalysisConfig &config);
        //std::vector<mfn::Detection> process(const cv::Mat &input);
        std::vector<std::vector<mfn::Detection>> process(const std::vector<cv::Mat> & inputs);

    private:
        cv::dnn::Net yolo;
        cv::Size net_input_size;
        mfn::AnalysisConfig config;

        std::vector<mfn::Detection> getDetectionFromOutput(const cv::Mat &input, float * data) const;
        std::vector<std::vector<mfn::Detection>> getDetectionsFromOutput(const std::vector<cv::Mat> &inputs, const std::vector<cv::Mat> & outputs) const;

    };
}



#endif //MICROFLUIDICNUCLEATIONSOFTWARE_YOLOV5_H
