#include "cli.hpp"
#include "platform/common.hpp"

void cli_clear(){  
  if(!gFmt->isEnabled) SYSTEM_CLEAR_TERMINAL();
  else cout << gFmt->CLEAR_TERMINAL;
}

void cli_input(const char *prompt, std::string &val, bool obsecure){
  cout << gFmt->BOLD << prompt << gFmt->RESET;
  if (obsecure)
    cout << gFmt->WHITE_BACKGROUND;

  std::getline(std::cin, val);

  if (obsecure)
    cout << gFmt->RESET;
}

uint32_t cli_menu(const std::vector<std::string> &items) {
  for (size_t i = 0; i < items.size(); ++i)
    cout << "(" << i << ") " << items[i] << "\n";

  cli_separator(10);

  std::string input;
  while (true) {
    cout << "(0 - " << items.size() - 1 << ") > ";
    std::getline(std::cin, input);

    try {
      int choice = std::stoi(input);
      if(choice < 0 || choice >= items.size()) 
        throw std::exception();

      return choice;
    }catch(const std::exception& e) { cli_error("Invalid choice!"); }
  }
}
