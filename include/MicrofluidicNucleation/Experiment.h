//
// Created by nicholas on 23.02.25.
//

#ifndef MICROFLUIDICNUCLEATIONSOFTWARE_EXPERIMENT_H
#define MICROFLUIDICNUCLEATIONSOFTWARE_EXPERIMENT_H
#include <filesystem>
#include <map>

namespace mfn
{
    class Experiment
    {
    public:
        Experiment(const std::filesystem::path &video, double frameRate, double calibration, double heater_temperature);
        Experiment(const std::filesystem::path & exp_file);
        Experiment() = default;

        const std::filesystem::path &getVideo() const;
        std::map<std::string, double> getParameters() const;
        double getParameter(const std::string &name) const;
        void writeToFile(const std::filesystem::path &config_file) const;

    private:
        std::filesystem::path video;
        //double frame_rate = 0;
        //double calibration = 0;
        //double heater_temperature = 0;
        std::map<std::string, double> setup_parameters; // TODO: Read this from file
    };


}
#endif //MICROFLUIDICNUCLEATIONSOFTWARE_EXPERIMENT_H
