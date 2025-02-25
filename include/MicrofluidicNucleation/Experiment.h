//
// Created by nicholas on 23.02.25.
//

#ifndef MICROFLUIDICNUCLEATIONSOFTWARE_EXPERIMENT_H
#define MICROFLUIDICNUCLEATIONSOFTWARE_EXPERIMENT_H
#include <filesystem>

namespace mfn
{
    class Experiment
    {
    public:
        Experiment(const std::filesystem::path &video, double frameRate, double calibration);
        Experiment() = default;

        const std::filesystem::path &getVideo() const;

        double getFrameRate() const;

        double getCalibration() const;

    private:
        std::filesystem::path video;
        double frame_rate;
        double calibration;
    };


}
#endif //MICROFLUIDICNUCLEATIONSOFTWARE_EXPERIMENT_H
