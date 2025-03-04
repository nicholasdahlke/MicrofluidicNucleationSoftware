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
    mfn::Experiment experiment("C:/Users/Nicholas Dahlke/Documents/Frozen T Cool -33-9 T Warm12-8 Oil 700 Water 10.mp4", 50, 1);
    mfn::AnalysisConfig config;
    config.net_file = "C:/Users/Nicholas Dahlke/Documents/multiphase.onnx";
    config.frame_stop = 20;

    mfn::VideoAnalyzer analyzer(experiment, config);
    analyzer.analyze();
    spdlog::get("mfn_logger")->info("Microfluidic Nucleation Counter finished");

}