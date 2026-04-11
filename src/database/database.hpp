#pragma once

#include <string>
#include <vector>

// Global
#define DATABASE_ATTR_SEPARATOR '|'
#define DATABASE_ENTRY_SEPARATOR '\n'

extern std::string gDatabasePath;
extern void InitializeDatabase();

// Helpers
extern std::vector<std::string> 
ReadAttributes(std::string_view content, size_t &i);

extern void LoadUsers();

//void MatchUsers(const sUser *filter, std::vector<sUser>);