#include "user.hpp"
#include "database.hpp"
#include <vector>

static std::vector<User> s_Users;
User gCurrentUser;

static inline std::string UserToString(const User& user) {
  return  user.UserID + DB_ATTR_DILM + 
          user.Email + DB_ATTR_DILM +
          user.Password + DB_ATTR_DILM + 
          user.ContactNumber + DB_ATTR_DILM +
          std::to_string(static_cast<int>(user.Faculty)) + DB_ATTR_DILM + 
          std::to_string(static_cast<int>(user.Status)) + DB_ATTR_DILM +
          std::to_string(static_cast<int>(user.Role)) + DB_ENTRY_DILM;
}

void LoadUsers() {
  std::string content = ReadFile(GetUsersFilePath());
  for(size_t i = 0; i < content.size(); ++i){
    std::vector<std::string> attrs = ReadAttributes(content, i);

    try{
      User u{attrs[0], attrs[1], attrs[2], attrs[3]};
      u.Faculty = static_cast<Faculty>(std::stoi(attrs[4]));
      u.Status = static_cast<UserStatus>(std::stoi(attrs[5]));
      u.Role = static_cast<UserRole>(std::stoi(attrs[6]));

      s_Users.push_back(u);
    } catch(const std::exception&){
      // error
    }
  }
}

void CreateUser(const User& user){
  WriteFile(GetUsersFilePath(), UserToString(user));
  s_Users.push_back(user);
}

std::vector<User> MatchUsers(const User& f, bool limitToOne){
  std::vector<User> res;

  for(const auto& user: s_Users){
    // matching
    if(!f.UserID.empty() && f.UserID != user.UserID) continue;
    if(!f.Email.empty() && f.Email != user.Email) continue;
    if(!f.Password.empty() && f.Password != user.Password) continue;
    if(!f.ContactNumber.empty() && f.ContactNumber != user.ContactNumber) continue;
    if(f.Faculty != Faculty::Unknown && f.Faculty != user.Faculty) continue;
    if(f.Status != UserStatus::Unknown && f.Status != user.Status) continue;
    if(f.Role != UserRole::Unknown && f.Role != user.Role) continue;
    
    res.emplace_back(user);
    if(limitToOne) break;
  }

  return res;
}