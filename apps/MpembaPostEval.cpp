//
// Created by nicholas on 21.03.25.
//
#include <MicrofluidicNucleation/NucleationCalculator.h>
#include <MicrofluidicNucleation/AnalysisConfig.h>
#include <MicrofluidicNucleation/CSV.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <queue>
#include <iostream>

int main()
{
    auto logger_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/log.txt", true);
    auto logger_console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto logger = std::make_shared<spdlog::logger>("mfn_logger", spdlog::sinks_init_list({logger_file_sink, logger_console_sink}));
    spdlog::register_logger(logger);
    spdlog::get("mfn_logger")->info("Microfluidic Nucleation Evaluation started");

    std::string config_path = "/mnt/md0/Progammiersoftwareprojekte/CLionProjects/MicrofluidicNucleationSoftware/examples/analysis_config.cf";
    mfn::AnalysisConfig config(config_path);

    std::filesystem::path mpemba_directory = "/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/05.03.25/";
    std::queue<std::filesystem::path> case_queue;
    for (auto const& dir_entry: std::filesystem::directory_iterator(mpemba_directory))
    {
        if(dir_entry.path().string().ends_with("-case.cf"))
            case_queue.push(dir_entry.path());
    }

    while(!case_queue.empty())
    {
        std::filesystem::path path = case_queue.front();
        case_queue.pop();
        mfn::NucleationCalculator calculator(path, config);
        std::filesystem::path bin_result_file = path.parent_path().string() + "/" + path.stem().string() + "-bin-results.csv";
        spdlog::get("mfn_logger")->info("Analyzing {}", path.string());
        int bins;
        std::cin >> bins;
        mfn::CSV::write(calculator.getNucleationRateBinned(bins), bin_result_file);
    }


    return 0;
}