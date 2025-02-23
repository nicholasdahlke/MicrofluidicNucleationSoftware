#include <opencv2/core/types.hpp>
#include "MicrofluidicNucleation/RawDroplet.h"

//
// Created by nicholas on 23.02.25.
//
void mfn::RawDroplet::setEllipse(const cv::RotatedRect &ellipse)
{
    RawDroplet::ellipse = ellipse;
}

void mfn::RawDroplet::setIsFrozen(bool isFrozen)
{
    is_frozen = isFrozen;
}

void mfn::RawDroplet::setDetection(const mfn::Detection &detection)
{
    RawDroplet::detection = detection;
}

void mfn::RawDroplet::setMovement(const mfn::Vector2D &movement)
{
    RawDroplet::movement = movement;
}

void mfn::RawDroplet::setDistanceToNext(double distanceToNext)
{
    distance_to_next = distanceToNext;
}

void mfn::RawDroplet::setContour(const std::vector <cv::Point> &contour)
{
    RawDroplet::contour = contour;
}

bool mfn::RawDroplet::isFrozen() const
{
    return is_frozen;
}

const std::vector<cv::Point> &mfn::RawDroplet::getContour() const
{
    return contour;
}

double mfn::RawDroplet::getDistanceToNext() const
{
    return distance_to_next;
}

const mfn::Detection &mfn::RawDroplet::getDetection() const
{
    return detection;
}

const mfn::Vector2D &mfn::RawDroplet::getMovement() const
{
    return movement;
}

cv::Point mfn::RawDroplet::getMidpoint() const
{
    return ellipse.center;
}
