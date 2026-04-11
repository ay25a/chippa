#include "platform/common.hpp"
#include "database.hpp"
#include "helpers.hpp"
#include <cstdint>
#include <sstream>

std::vector<sUser> gUsers;
std::string gDatabasePath;

void InitializeDatabase(){
  std::string path = GetExecutablePath();
  path = path.substr(0, path.find_last_of("\\/"));

  gDatabasePath = path + "/storage";
  CreateDirectory(path);
}

std::vector<std::string> ReadAttributes(std::string_view content, size_t &i) {
  std::vector<std::string> res;
  while(content[i] != '\n' && i < content.size()){
    std::string word;

    if(content[i] == DATABASE_ATTR_SEPARATOR) ++i;
    while(content[i] != DATABASE_ATTR_SEPARATOR && content[i] != '\n' && i < content.size())
      word += content[i++];

    res.emplace_back(word);
  }

  return std::move(res);
}

void LoadUsers() {
  std::string content = ReadFile(gDatabasePath + "/Users.txt");

  for(size_t i = 0; i < content.size(); ++i){
    auto attrs = ReadAttributes(content, i);
    if(attrs.size() == 0) break;

    try{
      gUsers.push_back(sUser{
        .UserID = attrs.at(0),
        .Email = attrs.at(1),
        .Password = attrs.at(2),
        .ContactNumber = attrs.at(3),
        .Faculty = static_cast<eFaculty>(std::stoi(attrs.at(4))),
        .Status = static_cast<eUserStatus>(std::stoi(attrs.at(5))),
        .Role = static_cast<eUserRole>(std::stoi(attrs.at(6))),
      });
    }catch(...){
      // log corrupted
    }
  }
}

void AddUser(const sUser& user){
  gUsers.emplace_back(user);
  WriteFile(gDatabasePath + "/Users.txt", Parse(user));
}

// Parsing functions
std::string Parse(const sUser& user) {
  std::stringstream txt;
  txt << user.UserID << DATABASE_ATTR_SEPARATOR
      << user.Email << DATABASE_ATTR_SEPARATOR
      << user.Password << DATABASE_ATTR_SEPARATOR
      << user.ContactNumber << DATABASE_ATTR_SEPARATOR
      << static_cast<uint32_t>(user.Faculty) << DATABASE_ATTR_SEPARATOR
      << static_cast<uint32_t>(user.Status) << DATABASE_ATTR_SEPARATOR
      << static_cast<uint32_t>(user.Role) << DATABASE_ENTRY_SEPARATOR;

  return txt.str();
}