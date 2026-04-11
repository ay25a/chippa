#pragma once

#include <string_view>

extern std::string ReadFile(std::string_view path);
extern void WriteFile(std::string_view path, std::string_view content);