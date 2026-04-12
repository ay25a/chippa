#pragma once

#include "formatter.hpp"
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

inline void cli_header(std::string_view text) {
  std::cout << "\n" << gFmt.BOLD << gFmt.UNDERLINE << text << gFmt.RESET << "\n\n";
}

inline void cli_subheader(std::string_view text){
  std::cout << "\n" << gFmt.BOLD << text << gFmt.RESET << "\n";
}

inline void cli_separator(size_t length, char ch = '=') {
  std::cout << std::string(length, ch) << '\n';
}

inline void cli_error(std::string_view text) {
  std::cout << gFmt.RED_BACKGROUND << gFmt.BOLD << "[Error] "<< text << gFmt.RESET << '\n';
}

void cli_clear();
void cli_input(std::string_view prompt, std::string &val, bool obsecure = false);
bool cli_bool(const std::string& prompt, bool def = true);
uint32_t cli_menu(const std::vector<std::string> &items);
