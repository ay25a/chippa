#include "cli.hpp"
#include <iomanip>

bool StringToInt(std::string_view str, int& out){
  for(const auto& ch: str){
    if(!std::isdigit(ch)) 
      return false;
  }

  out = std::stoi(str.data());
  return true;
}

void cli_confirm(){
  std::cout << gFmt.BOLD << "Press any key " << gFmt.RESET;
  std::string dummy;
  std::getline(std::cin, dummy);
}


bool cli_boolean(std::string_view prompt){
  std::string str;
  while(true){
    cli_input(prompt, str);
    if(str == "n" || str == "no") 
      return false;
    else if(str == "y" || str == "yes")
      return false;

    cli_error("Invalid option!");
  }
}

void cli_input(std::string_view prompt, std::string& value){
  std::cout << prompt;
  std::getline(std::cin, value);
}

unsigned int cli_menu(const std::vector<std::string> &items) {
  for (size_t i = 0; i < items.size(); ++i)
    std::cout << "(" << i << ") " << items[i] << "\n";

  cli_separator(10);

  std::string input;
  while (true) {
    std::cout << "(0 - " << items.size() << ") > ";
    std::getline(std::cin, input);

    int choice = 0;
    if(!StringToInt(input, choice) || (choice < 0 || choice > items.size() - 1))
      cli_error("Invalid choice!");
    else
      return choice;
  }
}

void cli_table(const std::vector<std::string>& names, const std::vector<std::vector<std::string>>& values){
  size_t cols = names.size();
  std::vector<size_t> width(cols, 0);

  auto print_row = [&](const auto& row) {
    for (size_t i = 0; i < row.size(); ++i)
      std::cout << std::left << std::setw(width[i] + 2) << row[i];
    
    std::cout << '\n';
  };

  for (size_t i = 0; i < cols; ++i)
    width[i] = names[i].size();

  for (const auto& row : values)
    for (size_t i = 0; i < row.size(); ++i)
      width[i] = std::max(width[i], row[i].size());
    
  std::cout << gFmt.BOLD;
  print_row(names);
  std::cout << gFmt.RESET;

  size_t total = 0;
  for (auto w : width) 
  total += w + 2;
  cli_separator(total, '-');

  for (const auto& row : values)
    print_row(row);
}