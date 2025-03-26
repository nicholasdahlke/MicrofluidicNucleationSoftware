//
// Created by nicholas on 16.03.25.
//

#ifndef CSVWRITER_H
#define CSVWRITER_H
#include <vector>
#include <fstream>
#include <filesystem>
#include <string>
#include <ranges>
#include <MicrofluidicNucleation/DropletResult.h>
#include <MicrofluidicNucleation/Experiment.h>

namespace mfn
{
   class CSV
   {
   public:
      template <typename T>
      static void write(std::vector<std::vector<T>> _vec, const std::filesystem::path& _filename)
      {
         std::ofstream outfile(_filename);
         for (const auto& row : _vec)
            writeValueToCsvFile(row, outfile);
         outfile.close();
      }

      template <typename T>
      static void write(std::vector<std::vector<T>> _vec,
         const std::vector<std::string>& _column_names,
         const std::filesystem::path& _filename)
      {
         if (_column_names.size() != _vec.front().size())
            throw std::runtime_error("Wrong number of column names in file.");
         std::ofstream outfile(_filename);
         writeValueToCsvFile(_column_names, outfile);
         for (const auto& row : _vec)
            writeValueToCsvFile(row, outfile);
         outfile.close();
      }

      static void write(const std::vector<mfn::DropletResult>& _vec, const std::filesystem::path& _filename)
      {
         std::ofstream outfile(_filename);
         std::vector<std::string> column_names = {"volume", "frozen", "time", "temperature", "has_volume"};
         std::vector<std::vector<double>> output;

         for (const mfn::DropletResult& row : _vec)
         {
            output.push_back({
               row.droplet_volume,
               row.is_frozen ? 1.0 : 0.0,
               row.time_ms,
               row.temperature_c,
               row.has_volume ? 1.0: 0.0}
               );
         }
         write(output, column_names, _filename);
      }

      static void write(const std::vector<std::tuple<double, double>>& _vec, const std::filesystem::path& _filename)
      {
         std::ofstream outfile(_filename);
         for (const std::tuple<double, double>& row : _vec)
            writeValueToCsvFile(std::vector<double>({std::get<0>(row), std::get<1>(row)}), outfile);
         outfile.close();
      }

      static std::vector<std::vector<std::string>> read(const std::filesystem::path& _filename, bool skip_header=false, std::string delimiter=";")
      {
         std::ifstream infile(_filename);

         if (!infile.is_open())
            throw std::runtime_error("Could not open file.");

         std::string line;
         std::vector<std::vector<std::string>> output;
         if (skip_header)
            std::getline(infile, line);

         while (std::getline(infile, line))
         {
            std::vector<std::string> row;
            for(const auto value : std::views::split(line, delimiter))
            {
               row.emplace_back(std::string_view(value));
            }
            output.push_back(row);
         }


         return output;
      }

   private:
      template <typename T>
      static void writeValueToCsvFile(std::vector<T> _vec, std::ofstream & _file)
      {
         for (size_t i = 0; i < _vec.size(); ++i) {
            _file << _vec[i] << (i==_vec.size()-1 ? "" : ";");
         }
         _file << "\n";
      }

   };
}

#endif //CSVWRITER_H
