#pragma once

#include <string>
#include <vector>

enum class UserStatus { Unknown = 0, Active, Suspended };
enum class UserRole { Unknown = 0, Student, Staff };
enum class Faculty {
  Unknown = 0,
  LCK_FES,
  FCI,
  FAM,
  FCS,
  FEd,
  MK_FMHS,
};

inline std::string FacultyToString(Faculty f){
  switch (f) {
  case Faculty::LCK_FES: return "LCK_FES";
  case Faculty::FCI: return "FCI";
  case Faculty::FAM: return "FAM";
  case Faculty::FCS: return "FCS";
  case Faculty::FEd: return "FEd";
  case Faculty::MK_FMHS: return "MK_FMHS";
  default: return "Unknown";
  }
}

struct User {
  std::string UserID;
  std::string Email;
  std::string Password;
  std::string ContactNumber;
  Faculty Faculty = Faculty::Unknown;
  UserStatus Status = UserStatus::Unknown;
  UserRole Role = UserRole::Unknown;
};

extern User gCurrentUser;
void LoadUsers();
void CreateUser(const User& user);
std::vector<User> MatchUsers(const User& filter, bool limitToOne = false);