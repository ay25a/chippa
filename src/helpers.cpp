#include "helpers.hpp"
#include <fstream>

std::string ReadFile(std::string_view path){
  std::ifstream file(path.data());
  if(!file.is_open()){
    WriteFile(path, "");
    return "";
  }

  file.seekg(0, std::ios_base::end);

  std::streampos pos = file.tellg();
  size_t size = static_cast<size_t>(pos);
  file.seekg(0, std::ios_base::beg);

  std::string res;
  res.resize(size);
  file.read(res.data(), size);
  file.close();

  return res;
}

void WriteFile(std::string_view path, std::string_view content){
  std::ofstream file(path.data(), std::ios_base::app);
  file.write(content.data(), content.size());
  file.close();
}