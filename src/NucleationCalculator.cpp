//
// Created by nicholas on 20.03.25.
//

#include <MicrofluidicNucleation/NucleationCalculator.h>
#include <MicrofluidicNucleation/ThermalSimulation.h>
#include <MicrofluidicNucleation/Experiment.h>
#include <gsl/gsl_statistics_double.h>
#include <spdlog/spdlog.h>

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
    double t = getCoolTime(results, temp);
    return -1.0 * (1.0 / (t * v)) * log(std::get<LIQUID>(droplet_count)/(std::get<LIQUID>(droplet_count) + std::get<FROZEN>(droplet_count)));
}

double mfn::NucleationCalculator::getCoolTime(std::vector<std::tuple<double, double> > results, double temp)
{
    double cutoff_temp = temp + 7.0; // TODO: Read from config file

    double cutoff_time = std::get<0>(*std::ranges::min_element(results, [temp](const std::tuple<double, double> &a, const std::tuple<double, double> &b) {return std::abs(temp - std::get<1>(a)) < std::abs(temp - std::get<1>(b));})); // Closest element to the temperature cutoff

    double cooler_exit_time = experiment.getParameters()["cooler_length"] * getDropletSpeed(); // Time when the droplet has left the cooler

    double threshold = 0.5;
    double temp_reached_time = std::get<0>(*std::ranges::find_if(results, [=](const auto& tup) {
        return std::abs(std::get<1>(tup) - temp) < threshold;
    }));

    return cooler_exit_time - cutoff_time;
}


