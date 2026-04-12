#pragma once

#include <string>
#include <vector>

#define DB_ATTR_DILM  '|'
#define DB_ENTRY_DILM '\n'

void InitializeDatabase();
std::vector<std::string> ReadAttributes(std::string_view content, size_t &i);

std::string ReadFile(std::string_view path);
void WriteFile(std::string_view path, std::string_view content);

std::string GetUsersFilePath();