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
#include <MicrofluidicNucleation/ResultsWriter.h>
#include <MicrofluidicNucleation/TemperatureReader.h>
#include <MicrofluidicNucleationGui/MicrofluidicNucleationGui.h>
#include <MicrofluidicNucleation/CsvWriter.h>


int hmain()
{
    auto logger_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/log.txt", true);
    auto logger_console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto logger = std::make_shared<spdlog::logger>("mfn_logger", spdlog::sinks_init_list({logger_file_sink, logger_console_sink}));
    spdlog::register_logger(logger);
    spdlog::get("mfn_logger")->info("Microfluidic Nucleation Counter started");
    const mfn::Experiment experiment("/home/nicholas/testvideo/700Ul Oel; 10Ul Wasser; 30 Warm; -39 Kalt; 47,8196 Framerate-11222024174140-0000.mp4", 47.82, 1e-6, -10);
    mfn::AnalysisConfig config;
    config.parallel = 100;
    config.net_file = "/mnt/md0/Progammiersoftwareprojekte/CLionProjects/MicrofluidicNucleationSoftware/resources/fixed_100_model.onnx";
    config.frame_stop = 100;

    mfn::TemperatureReader temperature_reader("/mnt/md0/Progammiersoftwareprojekte/CLionProjects/MicrofluidicNucleationSoftware/examples/temp_curve_example.tc");


    mfn::VideoAnalyzer analyzer(experiment, config, temperature_reader);
    analyzer.analyze();
    spdlog::get("mfn_logger")->info("Microfluidic Nucleation Counter finished");
    mfn::ResultsWriter writer(analyzer, experiment);
    std::vector<std::vector<double>> speeds_vector;
    for (double speed : writer.getSpeeds())
        speeds_vector.emplace_back(speed);
    mfn::CsvWriter::writeToCsvFile(speeds_vector, "out.csv");
    std::tuple<int, int> droplet_count = writer.countDroplets();
    std::vector<mfn::DropletResult> results = writer.getDropletResults();
    spdlog::get("mfn_logger")->info("Droplets counted: {} are frozen, {} are liquid.",
        std::get<0>(droplet_count),
        std::get<1>(droplet_count));
    return 0;
}

int main(int argc, char **argv)
{
    return mfngui::MicrofluidicNucleationGui::main(argc, argv);
}

int dchmain()
{
    auto logger_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/log.txt", true);
    auto logger_console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto logger = std::make_shared<spdlog::logger>("mfn_logger", spdlog::sinks_init_list({logger_file_sink, logger_console_sink}));
    spdlog::register_logger(logger);
    spdlog::get("mfn_logger")->info("Microfluidic Nucleation Counter started");
    mfn::AnalysisConfig config("/mnt/md0/Progammiersoftwareprojekte/CLionProjects/MicrofluidicNucleationSoftware/examples/analysis_config.cf");
    return 0;
}