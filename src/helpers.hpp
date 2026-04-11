#pragma once

#include <string>

// Platform-Dependent
extern std::string GetExecutablePath();
extern bool CreateDirectory(std::string_view path);
char GetChar();