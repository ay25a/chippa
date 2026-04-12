#pragma once

#include <string>
#include <cstdint>
#include <vector>

enum class eUserStatus { Unknown = 0, Active, Suspended };
enum class eUserRole { Unknown = 0, Student, Staff };
enum class eFaculty {
  Unknown = 0,
  LCK_FES,
  FCI,
  FAM,
  FCS,
  FEd,
  MK_FMHS,
};

struct sUser {
  sUser(std::string_view id = "", std::string_view email = "", std::string_view pass = "",
        std::string_view cn = "", uint32_t faculty = 0, uint32_t status = 0, uint32_t role = 0)
        : UserID(id), Email(email), Password(pass), ContactNumber(cn),
          Faculty(static_cast<eFaculty>(faculty)), Status(static_cast<eUserStatus>(status)),
          Role(static_cast<eUserRole>(role)) {};

  std::string UserID;
  std::string Email;
  std::string Password;
  std::string ContactNumber;
  eFaculty Faculty = eFaculty::Unknown;
  eUserStatus Status = eUserStatus::Unknown;
  eUserRole Role = eUserRole::Unknown;
};

extern sUser* gCurrentUser;
void LoadUsers();
void CreateUser(const sUser& user);
std::vector<sUser*> MatchUsers(const sUser& filter, bool limitToOne=false);
void SetActiveUser(std::string_view uid);
std::string Parse(const sUser& user);