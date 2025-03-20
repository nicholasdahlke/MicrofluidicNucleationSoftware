//
// Created by nicholas on 16.03.25.
//

#include <MicrofluidicNucleation/AnalysisConfig.h>
#include <toml++/toml.hpp>
#include <spdlog/spdlog.h>

mfn::AnalysisConfig::AnalysisConfig(const std::filesystem::path &configFile)
{
    toml::table table;
    try
    {
        table = toml::parse_file(configFile.string());
    }
    catch (...)
    {
        spdlog::get("mfn_logger")->error("Failed to parse config file {}", configFile.string());
    }

    std::optional<int> right_border_displacement_optional = table["config"]["right_border_displacement"].value<int>();
    if (right_border_displacement_optional.has_value())
        right_border_displacement = right_border_displacement_optional.value();

    std::optional<float> max_movement_threshold_displacement_optional = table["config"]["max_movement_threshold_displacement"].value<float>();
    if (max_movement_threshold_displacement_optional.has_value())
        max_movement_threshold_displacement = max_movement_threshold_displacement_optional.value();

    std::optional<int> left_border_volume_optional = table["config"]["left_border_volume"].value<int>();
    if (left_border_volume_optional.has_value())
        left_border_volume = left_border_volume_optional.value();

    std::optional<int> right_border_volume_optional = table["config"]["right_border_volume"].value<int>();
    if (right_border_volume_optional.has_value())
        right_border_volume = right_border_volume_optional.value();

    std::optional<int> x_threshold_count_optional = table["config"]["x_threshold_count"].value<int>();
    if (x_threshold_count_optional.has_value())
        x_threshold_count = x_threshold_count_optional.value();

    std::optional<float> score_threshold_optional = table["config"]["score_threshold"].value<float>();
    if (score_threshold_optional.has_value())
        score_threshold = score_threshold_optional.value();

    std::optional<float> nms_threshold_optional = table["config"]["nms_threshold"].value<float>();
    if (nms_threshold_optional.has_value())
        nms_threshold = nms_threshold_optional.value();

    std::optional<float> confidence_threshold_optional = table["config"]["confidence_threshold"].value<float>();
    if (confidence_threshold_optional.has_value())
        confidence_threshold = confidence_threshold_optional.value();

    std::optional<float> speed_border_left_optional = table["config"]["speed_border_left"].value<float>();
    if (speed_border_left_optional.has_value())
        speed_border_left = speed_border_left_optional.value();

    std::optional<float> speed_border_right_optional = table["config"]["speed_border_right"].value<float>();
    if (speed_border_right_optional.has_value())
        speed_border_right = speed_border_right_optional.value();

    std::optional<int> parallel_optional = table["config"]["parallel"].value<int>();
    if (parallel_optional.has_value())
        parallel = parallel_optional.value();

    std::optional<int> frame_start_optional = table["config"]["frame_start"].value<int>();
    if (frame_start_optional.has_value())
        frame_start = frame_start_optional.value();

    std::optional<int> frame_stop_optional = table["config"]["frame_stop"].value<int>();
    if (frame_stop_optional.has_value())
        frame_stop = frame_stop_optional.value();

    std::optional<std::string> net_file_optional = table["config"]["net_file"].value<std::string>();
    if (net_file_optional.has_value())
        net_file = net_file_optional.value();
    else
        spdlog::get("mfn_logger")->error("A DNN file needs to be provided.");
    spdlog::get("mfn_logger")->info("Config file read.");
    int a = 0;
}
