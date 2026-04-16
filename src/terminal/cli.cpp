#include "cli.hpp"
#include <cstring>
#include <iomanip>

void cli_clear(){  
  if(strcmp(gFmt.CLEAR_TERMINAL, "") == 0){
#ifdef _WIN32
    system("cls");
#else
  system("clear");
#endif
  }

  else 
    std::cout << gFmt.CLEAR_TERMINAL;
}

void cli_input(const InputDesc& desc){
  while(true){
    std::cout << gFmt.BOLD << desc.prompt << gFmt.RESET;
  
    if (desc.obsecure) std::cout << gFmt.WHITE_BACKGROUND;
    std::string input;
    std::getline(std::cin, input);
    if (desc.obsecure) std::cout << gFmt.RESET;

    auto error = desc.validate(input);
    if(error.empty()){
      desc.out = input;
      break;
    }

    cli_error(error);
  }
}

uint32_t cli_menu(const std::vector<const char*> &items) {
  for (size_t i = 0; i < items.size(); ++i)
    std::cout << "(" << i << ") " << items[i] << "\n";

  cli_separator(10);

  std::string input;
  while (true) {
    std::cout << "(0 - " << items.size() - 1 << ") > ";
    std::getline(std::cin, input);

    try {
      int choice = std::stoi(input);
      if(choice < 0 || choice >= items.size()) 
        throw std::exception();

      return choice;
    }catch(const std::exception& e) { cli_error("Invalid choice!"); }
  }
}

bool cli_bool(const std::string& prompt, bool def){
  std::string str;
  cli_input({prompt + " (y/n) ", str});
  if(str == "y") 
    return true;
  else if(str == "n") 
    return false;

  return def;
}

void cli_table(const std::vector<const char*>& names, const std::vector<std::vector<std::string>>& values){
  std::cout << gFmt.BOLD;
  for(const auto& str: names)
    std::cout << std::setw(8) << str << "\t";
  std::cout << gFmt.RESET << "\n";

  cli_separator(30, '-');
  for(const auto& v: values){
    for(const auto& str: v)
      std::cout << std::setw(8) << str << "\t";
    std::cout << "\n";
  }
}