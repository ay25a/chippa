#include "../helpers.hpp"
#include <cassert>
#include <iostream>

void InitializeDatabase(){
  std::string path = GetExecutablePath();
  size_t pos = path.find_last_of("\\/");
  assert(pos != std::string::npos);

  std::cout << path.substr(0, pos);
}
