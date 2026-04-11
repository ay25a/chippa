#pragma once

#include "formatter.hpp"
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

using std::cout;

// clang-format off

// Output only


inline void cli_clear() {
  cout << gFmt->CLEAR_TERMINAL;
}

inline void cli_header(const char *text) {
  cout << gFmt->BOLD << gFmt->UNDERLINE << text << gFmt->RESET << "\n\n";
}

inline void cli_separator(char ch, size_t length){
  cout << std::string(length, ch) << '\n';
}

inline void cli_error(const char *text) {
  cout << gFmt->RED_BACKGROUND << gFmt->BOLD << text << gFmt->RESET << '\n';
}

// Input related

inline void cli_input(const char *prompt, std::string &val, bool obsecure = false) {
  std::cout << gFmt->BOLD << prompt << gFmt->RESET;
  if (obsecure)
    std::cout << gFmt->INVISIBLE;

  std::cin >> val;

  if (obsecure)
    std::cout << gFmt->RESET;
}

static uint32_t cli_menu(const std::vector<std::string> &items) {
  for (size_t i = 0; i < items.size(); ++i)
    std::cout << "(" << i << ") " << items[i] << "\n";

  cli_separator('=', 10);
  int choice = -1;
  while (true) {
    std::cout << "(0 - " << items.size() - 1 << ") > ";
    std::cin >> choice;

    if (choice < 0 || choice >= items.size()) {
      cli_error("Invalid choice!");
      continue;
    }

    break;
  }

  return choice;
}
