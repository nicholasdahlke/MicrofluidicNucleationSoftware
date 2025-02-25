//
// Created by nicholas on 23.02.25.
//

#ifndef MICROFLUIDICNUCLEATIONSOFTWARE_RAWDROPLET_H
#define MICROFLUIDICNUCLEATIONSOFTWARE_RAWDROPLET_H
#include <opencv2/core/types.hpp>
#include <vector>
#include <MicrofluidicNucleation/Vector2D.h>
#include <MicrofluidicNucleation/Detection.h>

namespace mfn
{
    class RawDroplet
    {
    private:
        cv::RotatedRect ellipse;
        bool is_frozen;
        mfn::Detection detection;
        mfn::Vector2D movement;
        double distance_to_next;
        std::vector<cv::Point> contour;

    public:
        void setEllipse(const cv::RotatedRect &ellipse);

        void setIsFrozen(bool isFrozen);

        void setDetection(const Detection &detection);

        void setMovement(const Vector2D &movement);

        void setDistanceToNext(double distanceToNext);

        void setContour(const std::vector<cv::Point> &contour);

        bool isFrozen() const;

        const std::vector<cv::Point> &getContour() const;

        double getDistanceToNext() const;

        const Detection &getDetection() const;

        const Vector2D &getMovement() const;

        cv::Point getMidpoint() const;


    };
}
#endif //MICROFLUIDICNUCLEATIONSOFTWARE_RAWDROPLET_H
