//
// Created by nicholas on 23.02.25.
//

#ifndef MICROFLUIDICNUCLEATIONSOFTWARE_RAWDROPLET_H
#define MICROFLUIDICNUCLEATIONSOFTWARE_RAWDROPLET_H
#include <opencv2/core/types.hpp>
#include <vector>
#include <MicrofluidicNucleation/Vector2D.h>
#include <MicrofluidicNucleation/Detection.h>
#include <opencv2/core/mat.hpp>

namespace mfn
{
    class Droplet
    {
    private:
        cv::RotatedRect ellipse;
        cv::Mat droplet_image;
        bool is_frozen = false;
        mfn::Detection detection;
        mfn::Vector2D movement{};
        double distance_to_next{};
        bool ignore = false;

    public:
        explicit Droplet(const mfn::Detection& detection);
        Droplet() = default;

        void setEllipse(const cv::RotatedRect &ellipse);

        bool getIgnore() const;

        void setIgnore(bool ignore);

        void setMovement(const Vector2D &movement);

        void setDistanceToNext(double distanceToNext);

        bool isFrozen() const;



        double getDistanceToNext() const;

        double getVolume() const;

        const Detection &getDetection() const;

        const Vector2D &getMovement() const;

        cv::Point getMidpoint() const;

        const cv::Mat &getDropletImage() const;

        void setDropletImage(const cv::Mat &dropletImage);

        void clearDropletImage();

    };
}
#endif //MICROFLUIDICNUCLEATIONSOFTWARE_RAWDROPLET_H
