#include "user.hpp"
#include <vector>
#include <sstream>
#include "database.hpp"

static std::vector<sUser> s_Users;
sUser* gCurrentUser;

void LoadUsers() {
  std::string content = ReadFile(GetUsersFilePath());
  for(size_t i = 0; i < content.size(); ++i){
    std::vector<std::string> attrs = ReadAttributes(content, i);

    try{
      s_Users.emplace_back(
        attrs.at(0), attrs.at(1), attrs.at(2), attrs.at(3), 
        std::stoi(attrs.at(4)), std::stoi(attrs.at(5)), std::stoi(attrs.at(6)));
    }catch(const std::exception&){
      // log corrupted
    }
  }
}

void CreateUser(const sUser& user){
  WriteFile(GetUsersFilePath(), Parse(user));
  s_Users.push_back(user);
}

std::vector<sUser*> MatchUsers(const sUser &f, bool limitToOne){
  std::vector<sUser*> res;
  for(auto& user: s_Users){
    // matching
    if(!f.UserID.empty() && f.UserID != user.UserID) continue;
    if(!f.Email.empty() && f.Email != user.Email) continue;
    if(!f.Password.empty() && f.Password != user.Password) continue;
    if(!f.ContactNumber.empty() && f.ContactNumber != user.ContactNumber) continue;
    if(f.Faculty != eFaculty::Unknown && f.Faculty != user.Faculty) continue;
    if(f.Status != eUserStatus::Unknown && f.Status != user.Status) continue;
    if(f.Role != eUserRole::Unknown && f.Role != user.Role) continue;
    
    res.emplace_back(&user);
    if(limitToOne) break;
  }

  return res;
}

void SetActiveUser(std::string_view uid){
  std::vector<sUser*> matched = MatchUsers(sUser{uid}, true);
  if(!matched.empty()) 
    gCurrentUser = matched[0]; 
}

std::string Parse(const sUser& user) {
  std::stringstream txt;
  txt << user.UserID << DB_ATTR_DILM
      << user.Email << DB_ATTR_DILM
      << user.Password << DB_ATTR_DILM
      << user.ContactNumber << DB_ATTR_DILM
      << static_cast<uint32_t>(user.Faculty) << DB_ATTR_DILM
      << static_cast<uint32_t>(user.Status) << DB_ATTR_DILM
      << static_cast<uint32_t>(user.Role) << DB_ENTRY_DILM;

  return txt.str();
}