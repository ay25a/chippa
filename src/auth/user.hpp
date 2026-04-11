#pragma once

#include <string>

enum class eUserStatus { Unknown = 0, Active, Suspended };
enum class eUserRole { Unknown = 0, Student, Staff };
enum class eFaculty {
  Unknown = 0,
  LCK_FES,
  FCI,
  FAM,
};

struct sUser {
  std::string UserID;
  std::string Email;
  std::string Password;
  std::string ContactNumber;
  eFaculty Faculty = eFaculty::Unknown;
  eUserStatus Status = eUserStatus::Unknown;
  eUserRole Role = eUserRole::Unknown;
};