//
// Created by nicholas on 16.03.25.
//

#ifndef CSVWRITER_H
#define CSVWRITER_H
#include <vector>
#include <fstream>
#include <filesystem>
#include <memory>

namespace mfn
{
   class CsvWriter
   {
   public:
      template <typename T>
      static void writeToCsvFile(std::vector<std::vector<T>> _vec, const std::filesystem::path& _filename)
      {
         std::ofstream outfile(_filename);
         for (const auto& row : _vec)
            writeValueToCsvFile(row, outfile);
         outfile.close();
      }

   private:
      template <typename T>
      static void writeValueToCsvFile(std::vector<T> _vec, std::ofstream & _file)
      {
         for (size_t i = 0; i < _vec.size(); ++i) {
            _file << i << (i==_vec.size()-1 ? "" : ";") << _vec[i] << "\n";
         }
      }

   };
}

#endif //CSVWRITER_H
