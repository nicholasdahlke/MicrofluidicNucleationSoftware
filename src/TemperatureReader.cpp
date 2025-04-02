//
// Created by nicholas on 15.03.25.
//

#include "MicrofluidicNucleation/TemperatureReader.h"
#include <fstream>
#include <spdlog/spdlog.h>

mfn::TemperatureReader::TemperatureReader(std::filesystem::path filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
        spdlog::get("mfn_logger")->error("Error opening temperature curve file");

    std::regex valid_line_regex = getValidLineRegex(valid_commands);

    std::string line;
    while (getline(file, line))
    {
        std::smatch match;
        if (std::regex_search(line, match, valid_line_regex))
        {
            command current_command;
            current_command.commandName = match[COMMAND_NAME_IND];
            if (match[VALUE1_IND].matched)
                current_command.value1 = std::stod(match[VALUE1_IND]);

            if (match[VALUE2_IND].matched)
                current_command.value2 = std::stod(match[VALUE2_IND]);
            commands.push_back(current_command);
        }
    }

    file.close();
    if (file.is_open())
        spdlog::get("mfn_logger")->error("Error closing temperature curve file");
}

mfn::TemperatureReader::TemperatureReader(double temperature)
{
    commands.emplace_back("STABLE_TEMP", temperature, 0);
}


std::regex mfn::TemperatureReader::getValidLineRegex(std::vector<std::string> commands)
{
    std::string regex = "^(";
    for (auto it = commands.begin(); it != commands.end(); ++it)
    {
        regex += "(?:" + *it + ")";
        if (it != commands.end()-1)
            regex += "|";
    }

    regex += ");(-?\\d+.?\\d*)?;(-?\\d+.?\\d*)?\\s*(#.*)?$";

    return std::regex(regex);
}

double mfn::TemperatureReader::getTemperature(const double time_ms) const
{
    int command_time = 0;
    double current_temp = 0;
    for (const command& current_command : commands)
    {
        if ((time_ms >= command_time && time_ms <= command_time + current_command.value2) || current_command.value2 == 0.0)
        {
            if (current_command.commandName == "STABLE_TEMP")
                return current_command.value1;

            if (current_command.commandName == "LIN_RAMP_TIME")
            {
                const double m = (current_command.value1 - current_temp) / current_command.value2;
                return m*(time_ms - command_time) + current_temp;
            }
        }
        command_time += current_command.value2;
        current_temp = current_command.value1;
    }
    return -999;
}