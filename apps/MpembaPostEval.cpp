//
// Created by nicholas on 21.03.25.
//
#include <MicrofluidicNucleation/NucleationCalculator.h>
#include <MicrofluidicNucleation/AnalysisConfig.h>
#include <MicrofluidicNucleation/CSV.h>
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

    std::string config_path = "/mnt/md0/Progammiersoftwareprojekte/CLionProjects/MicrofluidicNucleationSoftware/examples/analysis_config.cf";
    mfn::AnalysisConfig config(config_path);

    std::string case_path = "/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/05.03.25/3- 700Ul Oel- 20Ul Wasser - 30 Warm- Ramp- -20 - -21 In 4 Min Danach Stable Auf -21- 22-8 Raumtemp-15-08 Messung Fertig- 47-81955 Framerate-03052025145632-0000_mirrored-case.cf";
    mfn::NucleationCalculator calculator(case_path, config);
    spdlog::get("mfn_logger")->info("Microfluidic Nucleation Evaluation completed, nucleation rate is {}", calculator.getNucleationRate());

    mfn::CSV::write(calculator.getNucleationRateBinned(4), "/home/nicholas/results2.csv");
    return 0;
}