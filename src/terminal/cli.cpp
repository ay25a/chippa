#include "cli.hpp"
#include <cstring>

void cli_clear(){  
  if(strcmp(gFmt.CLEAR_TERMINAL, "")){
#ifdef _WIN32
    system("cls");
#else
  system("clear");
#endif
  }

  else std::cout << gFmt.CLEAR_TERMINAL;
}

void cli_input(std::string_view prompt, std::string &val, bool obsecure){
  std::cout << gFmt.BOLD << prompt << gFmt.RESET;
  
  if (obsecure) std::cout << gFmt.WHITE_BACKGROUND;
  std::getline(std::cin, val);
  if (obsecure) std::cout << gFmt.RESET;
}

uint32_t cli_menu(const std::vector<std::string> &items) {
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
  cli_input(prompt + " (y/n) ", str);
  if(str == "y") 
    return true;
  else if(str == "n") 
    return false;

  return def;
}