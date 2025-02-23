//
// Created by nicholas on 23.02.25.
//

#include <MicrofluidicNucleation/Experiment.h>

mfn::Experiment::Experiment(const std::filesystem::path &video, double frameRate, double calibration)
{
    Experiment::video = video;
    Experiment::frame_rate = frameRate;
    Experiment::calibration = calibration;
}

const std::filesystem::path &mfn::Experiment::getVideo() const
{
    return video;
}

double mfn::Experiment::getFrameRate() const
{
    return frame_rate;
}

double mfn::Experiment::getCalibration() const
{
    return calibration;
}
