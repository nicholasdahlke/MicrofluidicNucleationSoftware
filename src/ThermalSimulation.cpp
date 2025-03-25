//
// Created by nicholas on 21.03.25.
//
#include <MicrofluidicNucleation/ThermalSimulation.h>
#include <numbers>
#include <cmath>

mfn::ThermalSimulation::ThermalSimulation(mfn::Experiment _experiment)
{
    experiment = _experiment;
}

void mfn::ThermalSimulation::simulate(dgl_parameters parameters, double end_temp)
{
    int iterations = 0;

    results.clear();
    results.push_back(std::make_tuple(0, experiment.getHeaterTemperature()));

    double k = experiment.getParameters()["thermal_conductivity_tubing"];
    double r_A = experiment.getParameters()["outer_radius_tubing"];
    double r_I = experiment.getParameters()["inner_radius_tubing"];
    double rho = experiment.getParameters()["water_density"];
    double T_K = end_temp;


    while (iterations < parameters.iterations_max)
    {
        double T = std::get<1>(results.back());
        double c_p_oil = 1.4982 * T + 1091;
        double c_p_water = (37.688 + (0.085778 * (T + 273.15)) + (5.3764e5 / pow((T + 273.15), 2)) + 3.663 * pow((T + 273.15) / 185 - 1, -0.99)) / 0.01801258;
        double c_p_tot = ((2.0 / 3.0) * c_p_water) + ((1.0 / 3.0) * c_p_oil);

        double dgl_timestep_result = T + parameters.time_step * dgl(
            T,
            k,
            r_A,
            r_I,
            rho,
            c_p_tot,
            T_K
            );
        results.emplace_back(std::get<0>(results.back()) += parameters.time_step, dgl_timestep_result);
        iterations++;
    }
}

double mfn::ThermalSimulation::dgl(
    double T,
    double k,
    double r_A,
    double r_I,
    double rho,
    double c_p,
    double T_k)
{
    return ((2 * std::numbers::pi * k * (T - T_k)) / (rho * std::numbers::pi * pow(r_I, 2) * c_p ) / log(r_I / r_A));
}

std::vector<std::tuple<double, double>> mfn::ThermalSimulation::getResults()
{
    return results;
}