//
// Created by nicholas on 15.03.25.
//

#ifndef TEMPERATUREREADER_H
#define TEMPERATUREREADER_H
#include <filesystem>
#include <regex>


namespace mfn
{
    class TemperatureReader
    {
    public:
        explicit TemperatureReader(std::filesystem::path filePath);
        explicit TemperatureReader(double temperature);
        TemperatureReader() = default;
        double getTemperature(double time_ms) const;

    private:
        struct command
        {
            std::string commandName;
            double value1 = -999;
            double value2 = 0;
        };
        std::vector<std::string> valid_commands = {
            "STABLE_TEMP",
            "LIN_RAMP_TIME",
            "START_TEMP"
        };
        enum command_index
        {
            COMMAND_NAME_IND = 1,
            VALUE1_IND = 2,
            VALUE2_IND = 3
        };
        static std::regex getValidLineRegex(std::vector<std::string> commands);
        std::vector<command> commands;
    };
}

#endif //TEMPERATUREREADER_H
