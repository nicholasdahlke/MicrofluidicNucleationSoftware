//
// Created by nicholas on 21.03.25.
//

#ifndef THERMALSIMULATION_H
#define THERMALSIMULATION_H
#include <MicrofluidicNucleation/Experiment.h>
#include <vector>

namespace mfn
{
    class ThermalSimulation
    {
    public:
        struct dgl_parameters
        {
            double iterations_max;
            double time_step;
        };

        explicit ThermalSimulation(mfn::Experiment _experiment);
        void simulate(dgl_parameters parameters, double end_temp);
        std::vector<std::tuple<double, double>> getResults();

    private:

        mfn::Experiment experiment;
        static double dgl(
            double T,
            double k,
            double r_A,
            double r_I,
            double rho,
            double c_p,
            double T_k);
        std::vector<std::tuple<double, double>> results;
    };
}
#endif //THERMALSIMULATION_H
