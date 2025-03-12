#include <opencv2/core/types.hpp>
#include "MicrofluidicNucleation/RawDroplet.h"
#include <numbers>
#include "spdlog/spdlog.h"

//
// Created by nicholas on 23.02.25.
//
void mfn::RawDroplet::setEllipse(const cv::RotatedRect &ellipse)
{
    RawDroplet::ellipse = ellipse;
}

void mfn::RawDroplet::setMovement(const mfn::Vector2D &movement)
{
    RawDroplet::movement = movement;
}

void mfn::RawDroplet::setDistanceToNext(double distanceToNext)
{
    distance_to_next = distanceToNext;
}


bool mfn::RawDroplet::isFrozen() const
{
    return is_frozen;
}


float mfn::RawDroplet::getTemperature() const
{
    return temperature_k;
}

void mfn::RawDroplet::setTemperature(float temperature_k)
{
    this->temperature_k = temperature_k;
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
    if (!ignore)
        return ellipse.center;
    return (detection.getRect().br() + detection.getRect().tl())*0.5;
}

mfn::RawDroplet::RawDroplet(const mfn::Detection& detection)
{
    RawDroplet::detection = detection;
    is_frozen = detection.getDetectionType() == "droplets_frozen";
}

const cv::Mat &mfn::RawDroplet::getDropletImage() const
{
    return droplet_image;
}

void mfn::RawDroplet::setDropletImage(const cv::Mat &dropletImage)
{
    droplet_image = dropletImage;
}

bool mfn::RawDroplet::getIgnore() const
{
    return ignore;
}

void mfn::RawDroplet::setIgnore(bool ignore)
{
    this->ignore = ignore;
}

double mfn::RawDroplet::getVolume() const
{
    if (ignore)
    {
        spdlog::get("mfn_logger")->error("Droplet has no calculated volume");
        return 0.0;
    }

    return std::numbers::pi * ellipse.size.height * ellipse.size.width;

}