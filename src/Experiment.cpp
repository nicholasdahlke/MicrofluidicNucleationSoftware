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

    setup_parameters["thermal_conductivity_tubing"] = 0.25;
    setup_parameters["inner_radius_tubing"] = 0.0004;
    setup_parameters["outer_radius_tubing"] = 0.000794;
    setup_parameters["water_density"] = 997.0;
    setup_parameters["cooler_length"] = 0.129115;
}

std::map<std::string, double> mfn::Experiment::getParameters() const
{
    return setup_parameters;
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