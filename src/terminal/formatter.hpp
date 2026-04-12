/*
 *  ====================================
 *  formatter.hpp
 *  =====================================
 *
 *  Provides a single, globally shared ANSI escape code formatter object
 *  `gFmt` for styled terminal output.
 *
 *  - `gFmt` is to be defined in `main.cpp`
 *  - Usage: `std::cout << gFmt.BOLD << "Hello" << gFmt.RESET << "\n";`
 *
 *  Disabling mechanism:
 *    The global variable `gFmt` is used instead of #define or constexpr
 *    to support runtime disabling. In case the program user has problems
 *    with output display (common with older windows terminals), the formatter 
 *    can be disabled by setting all of `gFmt` member variables to empty string.
 * =============================================================================
 */

#pragma once

// In case of adding a new member `DisableFormatter` 
// should be updated to nullify the new member variable
struct sANSIFormatter {
  // Essentials
  const char *CLEAR_TERMINAL = "\033[2J\033[H";
  const char *RESET = "\033[0m";

  // Font style
  const char *BOLD = "\033[1m";
  const char *UNDERLINE = "\033[4m";
  
  // Colors
  const char *RED_BACKGROUND = "\033[0;41m";
  const char *WHITE_BACKGROUND = "\033[0;47m";
  const char *WHITE_FOREGROUND = "\033[37;0m";
};

extern sANSIFormatter gFmt;
inline void DisableFormatter(){ 
  gFmt = {"", "", "", "", "", "", "" };
}