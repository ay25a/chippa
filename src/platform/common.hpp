#pragma once

#include <cstdlib>
#include <string>

inline void SYSTEM_CLEAR_TERMINAL(){
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}

inline void ENABLE_MANUAL_TERMINAL_INPUT(){
#ifdef __unix__
// Termios specific logic
#endif
}

extern std::string GetExecutablePath();
extern void CreateDirectory(std::string_view path);