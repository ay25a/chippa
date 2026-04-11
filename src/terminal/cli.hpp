#pragma once

#include "formatter.hpp"
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

using std::cout;

// Enabled formatter: clears terminal with `sANSIFormatter::CLEAR_TERMINAL`
//
// Disabled formatter: clears terminal with platform-dependent command
extern void cli_clear();

// Text with spacing (bold and underlined if formatter enabled)
inline void cli_header(const char *text) {
  cout << "\n" << gFmt->BOLD << gFmt->UNDERLINE << text << gFmt->RESET << "\n\n";
}

// Text with spaces (bold if formatted)
inline void cli_subheader(const char *text){
  cout << "\n" << gFmt->BOLD << text << gFmt->RESET << "\n";
}

inline void cli_separator(size_t length, char ch = '=') {
  cout << std::string(length, ch) << '\n';
}

// Text with '[Error]' prefix (bold and red background if formatter enabled)
inline void cli_error(const char *text) {
  cout << gFmt->RED_BACKGROUND << gFmt->BOLD << "[Error] "<< text << gFmt->RESET << '\n';
}

// Gets input using `std::getline`, 
// `obscure` enables rendering a white background if formatter enabled (not very secure but portable) 
extern void cli_input(const char *prompt, std::string &val, bool obsecure = false);

// Shows a menu with input for selectable items
// returns a valid choice input from user
extern uint32_t cli_menu(const std::vector<std::string> &items);