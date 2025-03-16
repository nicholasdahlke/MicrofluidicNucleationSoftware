//
// Created by nicholas on 23.02.25.
//

#include <MicrofluidicNucleation/Experiment.h>
#include <spdlog/spdlog.h>

mfn::Experiment::Experiment(const std::filesystem::path &video, double frameRate, double calibration, double heater_temperature)
{
    if (!std::filesystem::exists(video))
        spdlog::get("mfn_logger")->error("The provided video file {} does not exist.", video.string());
    else
        Experiment::video = video;

    if (frameRate <= 0 || calibration <= 0)
        spdlog::get("mfn_logger")->error("Calibration and framerate must be greater than zero.");
    else
    {
        Experiment::frame_rate = frameRate;
        Experiment::calibration = calibration;
    }

    Experiment::heater_temperature = heater_temperature;
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

double mfn::Experiment::getHeaterTemperature() const
{
    return heater_temperature;
}