#pragma once

// Authentication
#include "database/user.hpp"
#include "terminal/cli.hpp"
void Authenticate();
void Login();
void Register();

// Menu
void ShowStudentMenu();
inline void ShowStaffMenu(){};

// Common
inline void ShowProfile(){
  cli_header("Profile");
  cli_field("Organization ID", gCurrentUser.UserID);
  cli_field("Email", gCurrentUser.Email);
  cli_field("Password", std::string(gCurrentUser.Password.size(), '*'));
  cli_field("Contact Number", gCurrentUser.ContactNumber);
  cli_field("Faculty", FacultyToString(gCurrentUser.Faculty));
  cli_field("Status", (gCurrentUser.Status == UserStatus::Suspended ? "Suspended" : "Active"));
}