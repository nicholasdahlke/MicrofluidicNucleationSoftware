//
// Created by nicholas on 23.02.25.
//

#ifndef MICROFLUIDICNUCLEATIONSOFTWARE_DETECTION_H
#define MICROFLUIDICNUCLEATIONSOFTWARE_DETECTION_H
#include <opencv2/core/types.hpp>
#include <string>

namespace mfn
{
    class Detection
    {
    private:
        cv::Rect rect;
        std::string detection_type;
        float confidence;

    public:
        Detection(cv::Rect _rect, std::string _detection_type, float _confidence);
        cv::Rect getRect();
        float getConfidence();
        std::string getDetectionType();
    };
}
#endif //MICROFLUIDICNUCLEATIONSOFTWARE_DETECTION_H

