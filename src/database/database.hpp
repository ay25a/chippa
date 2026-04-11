#pragma once

#include <string>
#include <vector>
#include "auth/user.hpp"

// Global
#define DATABASE_ATTR_SEPARATOR '|'
#define DATABASE_ENTRY_SEPARATOR '\n'

extern std::string gDatabasePath;
extern void InitializeDatabase();

// Helpers
extern std::vector<std::string> 
ReadAttributes(std::string_view content, size_t &i);

extern std::vector<sUser> gUsers;

extern void LoadUsers();
extern void AddUser(const sUser& user);
extern void MatchUsers(const sUser& filter);

extern void CommitChanges();
// Parsing functions
extern std::string Parse(const sUser& user);