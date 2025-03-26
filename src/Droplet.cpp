#include <opencv2/core/types.hpp>
#include "MicrofluidicNucleation/Droplet.h"
#include <numbers>
#include "spdlog/spdlog.h"
#include <cmath>

//
// Created by nicholas on 23.02.25.
//
void mfn::Droplet::setEllipse(const cv::RotatedRect &ellipse)
{
    Droplet::ellipse = ellipse;
}

void mfn::Droplet::setMovement(const mfn::Vector2D &movement)
{
    Droplet::movement = movement;
}

void mfn::Droplet::setDistanceToNext(double distanceToNext)
{
    distance_to_next = distanceToNext;
}


bool mfn::Droplet::isFrozen() const
{
    return is_frozen;
}

double mfn::Droplet::getDistanceToNext() const
{
    return distance_to_next;
}

const mfn::Detection &mfn::Droplet::getDetection() const
{
    return detection;
}

const mfn::Vector2D &mfn::Droplet::getMovement() const
{
    return movement;
}

cv::Point mfn::Droplet::getMidpoint() const
{
    if (!ignore)
        return ellipse.center;
    return (detection.getRect().br() + detection.getRect().tl())*0.5;
}

mfn::Droplet::Droplet(const mfn::Detection& detection)
{
    Droplet::detection = detection;
    is_frozen = detection.getDetectionType() == "droplets_frozen";
}

const cv::Mat &mfn::Droplet::getDropletImage() const
{
    return droplet_image;
}

void mfn::Droplet::setDropletImage(const cv::Mat &dropletImage)
{
    droplet_image = dropletImage;
}

bool mfn::Droplet::getIgnore() const
{
    return ignore;
}

void mfn::Droplet::setIgnore(const bool ignore)
{
    this->ignore = ignore;
}

double mfn::Droplet::getVolume() const
{
    if (ignore)
    {
        spdlog::get("mfn_logger")->warn("Droplet has no calculated volume");
        return 0.0;
    }

    double c = std::max(ellipse.size.width, ellipse.size.height) * 0.5;
    double a = std::min(ellipse.size.height, ellipse.size.width) * 0.5;

    return (4.0/3.0) * std::numbers::pi * pow(a, 2) * c;

}

void mfn::Droplet::clearDropletImage()
{
    droplet_image.release();
}
