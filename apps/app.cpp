//
// Created by nicholas on 23.02.25.
//
#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <MicrofluidicNucleation/VideoAnalyzer.h>
#include <MicrofluidicNucleation/AnalysisConfig.h>
#include <MicrofluidicNucleation/Experiment.h>




int main()
{
    auto logger_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/log.txt", true);
    auto logger_console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto logger = std::make_shared<spdlog::logger>("mfn_logger", spdlog::sinks_init_list({logger_file_sink, logger_console_sink}));
    spdlog::register_logger(logger);
    spdlog::get("mfn_logger")->info("Microfluidic Nucleation Counter started");
    const mfn::Experiment experiment("/home/nicholas/testvideo/700Ul Oel; 10Ul Wasser; 30 Warm; -39 Kalt; 47,8196 Framerate-11222024174140-0000.mp4", 47.82, 1);
    mfn::AnalysisConfig config;
    config.parallel = 100;
    config.net_file = "/mnt/md0/Progammiersoftwareprojekte/CLionProjects/MicrofluidicNucleationSoftware/resources/fixed_100_model.onnx";
    config.frame_stop = 200;

    mfn::VideoAnalyzer analyzer(experiment, config);
    analyzer.analyze();
    spdlog::get("mfn_logger")->info("Microfluidic Nucleation Counter finished");

}