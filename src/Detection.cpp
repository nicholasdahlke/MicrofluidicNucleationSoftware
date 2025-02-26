//
// Created by nicholas on 23.02.25.
//
#include <MicrofluidicNucleation/Detection.h>

mfn::Detection::Detection(cv::Rect _rect, std::string _detection_type, float _confidence)
{
    rect = _rect;
    detection_type = _detection_type;
    confidence = _confidence;
}

cv::Rect mfn::Detection::getRect() const
{
    return rect;
}

float mfn::Detection::getConfidence() const
{
    return confidence;
}

std::string mfn::Detection::getDetectionType() const
{
    return detection_type;
}
