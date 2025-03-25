//
// Created by nicholas on 20.03.25.
//

#ifndef NUCLEATIONCALCULATOR_H
#define NUCLEATIONCALCULATOR_H
#include <MicrofluidicNucleation/DropletResult.h>
#include <MicrofluidicNucleation/AnalysisConfig.h>
#include <vector>
#include <tuple>

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
            Experiment _experiment);

        double getNucleationRate();


    private:
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

        double getCoolTime(std::vector<std::tuple<double, double>> results, double temp);



    };
}

#endif //NUCLEATIONCALCULATOR_H
