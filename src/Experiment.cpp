//
// Created by nicholas on 23.02.25.
//

#include <MicrofluidicNucleation/Experiment.h>
#include <spdlog/spdlog.h>
#include <toml++/toml.hpp>
#include <fstream>

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
        setup_parameters["frame_rate"] = frameRate;
        setup_parameters["calibration"] = calibration;
    }

    setup_parameters["heater_temperature"] = heater_temperature;

    setup_parameters["thermal_conductivity_tubing"] = 0.25;
    setup_parameters["inner_radius_tubing"] = 0.0004;
    setup_parameters["outer_radius_tubing"] = 0.000794;
    setup_parameters["water_density"] = 997.0;
    setup_parameters["cooler_length"] = 0.129115;
}

mfn::Experiment::Experiment(const std::filesystem::path &exp_file)
{
    if (!std::filesystem::exists(exp_file))
        spdlog::get("mfn_logger")->error("The provided config file {} does not exist.", exp_file.string());

    toml::table table;
    try
    {
        table = toml::parse_file(exp_file.string());
    }
    catch (...)
    {
        spdlog::get("mfn_logger")->error("Failed to parse config file {}", exp_file.string());
    }

    // TODO: Could be done nicer

    std::string k1 = "calibration";
    std::optional<double> v1 = table[k1].value<double>();
    setup_parameters[k1] = v1.has_value() ? v1.value() : 0.0;

    std::string k2 = "cooler_length";
    std::optional<double> v2 = table[k2].value<double>();
    setup_parameters[k2] = v2.has_value() ? v2.value() : 0.0;

    std::string k3 = "frame_rate";
    std::optional<double> v3 = table[k3].value<double>();
    setup_parameters[k3] = v3.has_value() ? v3.value() : 0.0;

    std::string k4 = "heater_temperature";
    std::optional<double> v4 = table[k4].value<double>();
    setup_parameters[k4] = v4.has_value() ? v4.value() : 0.0;

    std::string k5 = "inner_radius_tubing";
    std::optional<double> v5 = table[k5].value<double>();
    setup_parameters[k5] = v5.has_value() ? v5.value() : 0.0;

    std::string k6 = "outer_radius_tubing";
    std::optional<double> v6 = table[k6].value<double>();
    setup_parameters[k6] = v6.has_value() ? v6.value() : 0.0;

    std::string k7 = "thermal_conductivity_tubing";
    std::optional<double> v7 = table[k7].value<double>();
    setup_parameters[k7] = v7.has_value() ? v7.value() : 0.0;

    std::string k8 = "water_density";
    std::optional<double> v8 = table[k8].value<double>();
    setup_parameters[k8] = v8.has_value() ? v8.value() : 0.0;

    std::string k9 = "video";
    std::optional<std::string> v9 = table[k9].value<std::string>();
    video = v9.has_value() ? v9.value() : "";
}


std::map<std::string, double> mfn::Experiment::getParameters() const
{
    return setup_parameters;
}

double mfn::Experiment::getParameter(const std::string &name) const
{
    if (!setup_parameters.contains(name))
    {
        spdlog::get("mfn_logger")->error("Parameter {} does not exist.", name);
        return 0.0;
    }

    return setup_parameters.at(name);
}


const std::filesystem::path &mfn::Experiment::getVideo() const
{
    return video;
}

void mfn::Experiment::writeToFile(const std::filesystem::path &config_file) const
{
    auto toml_table = toml::table();
    for (const auto &[fst, snd] : setup_parameters )
    {
        toml_table.insert(fst, snd);
    }
    toml_table.insert("video", video.string());
    std::ofstream output_file(config_file, std::ios::out);
    output_file << toml_table;
    output_file.close();
}
