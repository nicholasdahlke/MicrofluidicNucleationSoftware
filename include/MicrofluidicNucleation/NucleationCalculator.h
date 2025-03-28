//
// Created by nicholas on 20.03.25.
//

#ifndef NUCLEATIONCALCULATOR_H
#define NUCLEATIONCALCULATOR_H
#include <MicrofluidicNucleation/DropletResult.h>
#include <MicrofluidicNucleation/AnalysisConfig.h>
#include <vector>
#include <tuple>
#include <utility>

#include "Experiment.h"

namespace mfn
{
    class NucleationCalculator
    {
    public:
        NucleationCalculator(
            std::vector<mfn::DropletResult> _droplet_results,
            mfn::AnalysisConfig _analysis_config,
            std::vector<double> _droplet_speeds,
            std::vector<double> _droplet_volumes,
            Experiment _experiment
            );

        NucleationCalculator(
            std::filesystem::path _droplet_path,
            mfn::AnalysisConfig _analysis_config,
            std::filesystem::path _speed_path,
            std::filesystem::path _volume_path,
            std::filesystem::path _experiment_path);

        NucleationCalculator(
            std::filesystem::path _case_path,
            mfn::AnalysisConfig _analysis_config
            );

        double getNucleationRate();
        std::vector<std::tuple<double, double>> getNucleationRateBinned(int bins);


    private:
        void initFromFiles(
            std::filesystem::path _droplet_path,
            std::filesystem::path _speed_path,
            std::filesystem::path _volume_path,
            std::filesystem::path _experiment_path);

        enum
        {
            FROZEN = 0,
            LIQUID = 1
        };

        std::vector<mfn::DropletResult> dropletResults;
        mfn::AnalysisConfig analysisConfig;
        std::vector<double> dropletSpeeds;
        std::vector<double> dropletVolumes;

        Experiment experiment;

        double getDropletVolume();
        double getDropletSpeed();

        static std::tuple<int, int> getDropletCount(
            std::vector<mfn::DropletResult>::const_iterator first,
            std::vector<mfn::DropletResult>::const_iterator last
            );

        double getNucleationRate(
            std::vector<mfn::DropletResult>::const_iterator first,
            std::vector<mfn::DropletResult>::const_iterator last,
            double temp
            );

        std::vector<std::tuple<double, double>> getNucleationRateBinned(
            std::vector<mfn::DropletResult>::const_iterator first,
            std::vector<mfn::DropletResult>::const_iterator last,
            double start_temp,
            double end_temp,
            int bins
            );

        double getCoolTime(std::vector<std::tuple<double, double>> results, double temp);

        std::pair<std::vector<mfn::DropletResult>::const_iterator, std::vector<mfn::DropletResult>::const_iterator> getBinIteratorPair(
            std::vector<mfn::DropletResult>::const_iterator first,
            std::vector<mfn::DropletResult>::const_iterator last,
            int bin,
            double start_temp,
            double end_temp,
            int bins
            );

        static double getBinCenterTemp(int bin, double start_temp, double end_temp, int bins);

    };
}

#endif //NUCLEATIONCALCULATOR_H
