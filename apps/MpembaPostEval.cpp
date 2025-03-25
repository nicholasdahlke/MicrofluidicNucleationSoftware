//
// Created by nicholas on 21.03.25.
//
#include <MicrofluidicNucleation/NucleationCalculator.h>
#include <MicrofluidicNucleation/AnalysisConfig.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

int main()
{
    auto logger_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/log.txt", true);
    auto logger_console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto logger = std::make_shared<spdlog::logger>("mfn_logger", spdlog::sinks_init_list({logger_file_sink, logger_console_sink}));
    spdlog::register_logger(logger);
    spdlog::get("mfn_logger")->info("Microfluidic Nucleation Evaluation started");

    std::string droplet_path = "/home/nicholas/testvideo/700Ul Oel; 10Ul Wasser; 30 Warm; -39 Kalt; 47,8196 Framerate-11222024174140-0000-droplets.csv";
    std::string speed_path = "/home/nicholas/testvideo/700Ul Oel; 10Ul Wasser; 30 Warm; -39 Kalt; 47,8196 Framerate-11222024174140-0000-speeds.csv";
    std::string volume_path = "/home/nicholas/testvideo/700Ul Oel; 10Ul Wasser; 30 Warm; -39 Kalt; 47,8196 Framerate-11222024174140-0000-volumes.csv";

    std::string config_path = "/mnt/md0/Progammiersoftwareprojekte/CLionProjects/MicrofluidicNucleationSoftware/examples/analysis_config.cf";
    mfn::AnalysisConfig config(config_path);
    mfn::Experiment experiment("", 47.8196, 1, 30);
    mfn::NucleationCalculator calculator(droplet_path, config, speed_path, volume_path, experiment);
    spdlog::get("mfn_logger")->info("Microfluidic Nucleation Evaluation completed, nucleation rate is {}", calculator.getNucleationRate());

    return 0;
}