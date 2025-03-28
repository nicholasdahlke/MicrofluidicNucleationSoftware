//
// Created by nicholas on 20.03.25.
//

#include <MicrofluidicNucleation/NucleationCalculator.h>
#include <MicrofluidicNucleation/ThermalSimulation.h>
#include <MicrofluidicNucleation/Experiment.h>
#include <MicrofluidicNucleation/CSV.h>
#include <gsl/gsl_statistics_double.h>
#include <spdlog/spdlog.h>
#include <string>
#include <MicrofluidicNucleation/CSV.h>
#include <toml++/toml.hpp>

mfn::NucleationCalculator::NucleationCalculator(
    std::vector<mfn::DropletResult> _droplet_results,
    mfn::AnalysisConfig _analysis_config,
    std::vector<double> _droplet_speeds,
    std::vector<double> _droplet_volumes,
    Experiment _experiment
    )
{
    dropletResults = _droplet_results;
    analysisConfig = _analysis_config;
    dropletSpeeds = _droplet_speeds;
    dropletVolumes = _droplet_volumes;
    experiment = _experiment;
}

mfn::NucleationCalculator::NucleationCalculator(std::filesystem::path _case_path, mfn::AnalysisConfig _analysis_config)
{
    toml::table table;
    try
    {
        table = toml::parse_file(_case_path.string());
    }
    catch (...)
    {
        spdlog::get("mfn_logger")->error("Failed to parse config file {}", _case_path.string());
    }

    std::optional<std::string> droplet_path = table["droplets"].value<std::string>();
    std::optional<std::string> exp_path = table["experiment"].value<std::string>();
    std::optional<std::string> speeds_path = table["speeds"].value<std::string>();
    std::optional<std::string> volume_path = table["volumes"].value<std::string>();



    analysisConfig = _analysis_config;
    if (droplet_path.has_value() && exp_path.has_value() && speeds_path.has_value() && volume_path.has_value())
    {
        initFromFiles(
            droplet_path.value(),
            speeds_path.value(),
            volume_path.value(),
            exp_path.value());
    }

}

mfn::NucleationCalculator::NucleationCalculator(
    std::filesystem::path _droplet_path,
    mfn::AnalysisConfig _analysis_config,
    std::filesystem::path _speed_path,
    std::filesystem::path _volume_path,
    std::filesystem::path _experiment_path
    )
{
    analysisConfig = _analysis_config;
    initFromFiles(_droplet_path, _speed_path, _volume_path, _experiment_path);
}

void mfn::NucleationCalculator::initFromFiles(
    std::filesystem::path _droplet_path,
    std::filesystem::path _speed_path,
    std::filesystem::path _volume_path,
    std::filesystem::path _experiment_path)
{
    experiment = mfn::Experiment(_experiment_path);

    std::vector<std::vector<std::string>> speeds = mfn::CSV::read(_speed_path);
    for (const std::vector<std::string>& speed : speeds)
        dropletSpeeds.push_back(stod(speed[0]));

    std::vector<std::vector<std::string>> volumes = mfn::CSV::read(_volume_path);
    for (const std::vector<std::string>& volume : volumes)
        dropletVolumes.push_back(stod(volume.at(0)));

    std::vector<std::vector<std::string>> droplets = mfn::CSV::read(_droplet_path, true);
    for (const std::vector<std::string>& droplet : droplets)
    {
        dropletResults.emplace_back(
            stod(droplet[0]),
            droplet[1] == "1",
            stod(droplet[2]),
            stod(droplet[3]),
            droplet[4] == "1"
            );
    }
}



double mfn::NucleationCalculator::getDropletVolume()
{
    std::ranges::sort(dropletVolumes);
    return gsl_stats_gastwirth_from_sorted_data(dropletVolumes.data(),  1, dropletVolumes.size());
}

double mfn::NucleationCalculator::getDropletSpeed()
{
    std::ranges::sort(dropletSpeeds);
    return gsl_stats_gastwirth_from_sorted_data(dropletSpeeds.data(), 1, dropletSpeeds.size());
}

std::tuple<int, int> mfn::NucleationCalculator::getDropletCount(
            const std::vector<mfn::DropletResult>::const_iterator first,
            const std::vector<mfn::DropletResult>::const_iterator last)
{
    int liquid = 0;
    int frozen = 0;
    for (auto it = first; it != last; ++it)
    {
        if (it->is_frozen)
            frozen++;
        else
            liquid++;
    }
    return std::make_tuple(frozen, liquid);
}

double mfn::NucleationCalculator::getNucleationRate(
    const std::vector<mfn::DropletResult>::const_iterator first,
    const std::vector<mfn::DropletResult>::const_iterator last,
    double temp)
{
    std::tuple<int, int> droplet_count = getDropletCount(first, last);
    double v = getDropletVolume();

    ThermalSimulation simulation(experiment);
    ThermalSimulation::dgl_parameters sim_parameters = {1e5, 1e-4};
    simulation.simulate(sim_parameters, temp);
    std::vector<std::tuple<double, double>> results = simulation.getResults();
    mfn::CSV::write(results, "/home/nicholas/testvideo/thermal.csv");
    double t = getCoolTime(results, temp);
    double n_u = std::get<LIQUID>(droplet_count);
    double n_0 = std::get<LIQUID>(droplet_count) + std::get<FROZEN>(droplet_count);
    return -1.0 * (1.0 / (t * v)) * log(n_u / n_0);
}

double mfn::NucleationCalculator::getCoolTime(std::vector<std::tuple<double, double> > results, double temp)
{
    double cutoff_temp = temp + 7.0; // TODO: Read from config file
    double cutoff_time = std::get<0>(*std::ranges::min_element(results, [cutoff_temp](const std::tuple<double, double> &a, const std::tuple<double, double> &b) {return std::abs(cutoff_temp - std::get<1>(a)) < std::abs(cutoff_temp - std::get<1>(b));})); // Closest element to the temperature cutoff

    double cooler_exit_time = experiment.getParameters()["cooler_length"] / getDropletSpeed(); // Time when the droplet has left the cooler

    double threshold = 0.5;
    double temp_reached_time = std::get<0>(*std::ranges::find_if(results, [=](const auto& tup) {
        return std::abs(std::get<1>(tup) - temp) < threshold;
    }));

    return cooler_exit_time - cutoff_time;
}

double mfn::NucleationCalculator::getNucleationRate()
{
    return getNucleationRate(dropletResults.begin(), dropletResults.end(), dropletResults.front().temperature_c);
}

std::vector<std::tuple<double, double>> mfn::NucleationCalculator::getNucleationRateBinned(
    const std::vector<mfn::DropletResult>::const_iterator first,
    const std::vector<mfn::DropletResult>::const_iterator last,
    const double start_temp,
    const double end_temp,
    const int bins)
{
    std::vector<std::tuple<double, double>> results;  // Bin center temp is first, Bin nucleation rate second
    results.reserve(bins);

    for (int i = 0; i < bins - 1; i++)
    {
        std::pair<std::vector<mfn::DropletResult>::const_iterator, std::vector<mfn::DropletResult>::const_iterator> it_pair = getBinIteratorPair(first, last, i, start_temp, end_temp, bins);
        double temp = getBinCenterTemp(i, start_temp, end_temp, bins);
        double nucleation_rate = getNucleationRate(it_pair.first, it_pair.second, temp);
        results.emplace_back(temp, nucleation_rate);
    }
    return results;
}

std::vector<std::tuple<double, double>> mfn::NucleationCalculator::getNucleationRateBinned(int bins)
{
    double start_temp = dropletResults.front().temperature_c;
    double end_temp = dropletResults.back().temperature_c;
    return getNucleationRateBinned(dropletResults.begin(), dropletResults.end(), start_temp, end_temp, bins);
}

std::pair<std::vector<mfn::DropletResult>::const_iterator, std::vector<mfn::DropletResult>::const_iterator> mfn::NucleationCalculator::getBinIteratorPair(
    const std::vector<mfn::DropletResult>::const_iterator first,
    const std::vector<mfn::DropletResult>::const_iterator last,
    const int bin,
    const double start_temp,
    const double end_temp,
    const int bins)
{
    // For this function to work, the results need to be sorted by temperature. This should THEORETICALLY always be the case.
    // The first bin, is bin 0
    double bin_width = (end_temp - start_temp)/static_cast<double>(bins);
    double bin_start_temp = start_temp + bin_width * static_cast<double>(bin);
    double bin_end_temp = bin_start_temp + bin_width;

    auto start_it = std::min_element(first, last, [&](const auto& a, const auto& b) {return std::abs(a.temperature_c - bin_start_temp) < std::abs(b.temperature_c - bin_start_temp);});
    auto end_it = std::min_element(first, last, [&](const auto& a, const auto&b ) {return std::abs(a.temperature_c - bin_end_temp) < std::abs(b.temperature_c - bin_end_temp);});

    return std::make_pair(start_it, end_it);
}

double mfn::NucleationCalculator::getBinCenterTemp(int bin, double start_temp, double end_temp, int bins)
{
    return start_temp + ((end_temp - start_temp)/static_cast<double>(bins)) * (static_cast<double>(bin) + 0.5);
}
