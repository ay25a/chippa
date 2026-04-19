/// @file ui_elements.hpp
/// @brief Contains all Terminal pages for both students and staff
#pragma once

#include <string>
#include <functional>
#include "cli_core.hpp"
#include "entities.hpp"

// **************************************
// Authentication Related
// **************************************

/// @brief Throw on unexpectef failure (Programming Bug)
#define EXPECT(cond) if(!cond) throw std::runtime_error("Unexpected Error!")

/// @brief The current active user. 
/// @note Should be set as a global variable in `main.cpp`.
extern User gCurrentUser;

/// @brief Login page using user ID and password.
/// @return true on success, and false on failure
/// @note On Success, sets the gCurrentUser to the found user
extern bool ui_login();

/// @brief Attempts to Create a new user by filling a `User` struct
/// @return true on success, and false on failure
/// @note On Success, sets the gCurrentUser to the newly created user
extern bool ui_register();

// **************************************
// Main Menu
// **************************************

/// @brief Shows the main student menu
extern void ui_student_menu();

/// @brief Shows the main staff menu
extern void ui_staff_menu();


// **************************************
// Profile
// **************************************

// Shared
extern void ui_view_profile_core(const User& user);
extern void ui_edit_current_profile();

// Role
extern void ui_student_view_profile();
extern void ui_staff_view_profile(const User& user);

// **************************************
// Vehicles
// **************************************

// Shared
extern void ui_view_vehicles_core(const User& user);

// Student Related
extern void ui_student_view_vehicles();
extern void ui_add_vehicle();
extern void ui_delete_vehicle();

// **************************************
// Applications
// **************************************

extern void ui_view_applications(const std::vector<ParkingApplication>& apps);

extern void ui_view_active_application(const std::vector<ParkingApplication> &apps);

extern void ui_student_applications(const User& user);

extern void ui_staff_view_applications();

// **************************************
// Passes
// **************************************

extern void ui_view_passes_core(const User &user);

extern void ui_view_active_pass(const std::vector<ParkingPass>& passes);
extern void ui_new_application();
extern void ui_view_passes(const User& user);


// **************************************
// Helpers
// **************************************
inline std::string get_valid_input(const std::string& prompt, std::function<std::string(const std::string&)> validate){
  for(;;){
    std::string in = cli_input(prompt);

    std::string error = validate(in);
    if(error.empty())
      return in;

    cli_error(error);
  }
}

inline std::string userid_validator(const std::string& in){
  if(!is_integer(in) || in.size() < 4) 
      return "User id should be at least 4 numbers, can cannot contain any letters";
  return "";
}

inline std::string password_validator(const std::string& in){
  return (in.size() < 5 || in.size() >= 12) ? "Password length should be 5 to 11 characters" : "";
}

inline std::string contact_validator(const std::string& in){
  return in.size() < 10 || !is_integer(in) ? "Invalid contact number" : "";
}

inline std::string name_validator(const std::string& in){
  return in.size() < 3 || in.size() > 31 ? "Name length should be 3 to 31 characters" : "";
}

inline std::string age_validator(const std::string& in){
  if(!is_integer(in))
    return "Age can only be numbers";
  int age = std::stoi(in);
  return (age < 16 || age > 99) ? "Age should be between 16 and 99" : "";
}

inline const char* eto_string(ePassStatus status){
  switch (status) {
  case ePassStatus::Active:     return "Active";
  case ePassStatus::Expired:    return "Expired";
  default:                      return "unknown";
  }
}

inline const char* eto_string(ePassDuration duration){
  switch (duration) {
  case ePassDuration::OneMonth:     return "1 Month";
  case ePassDuration::TwoMonths:    return "2 Months";
  case ePassDuration::ThreeMonths:  return "3 Months";
  default:                          return "unknown";
  }
}

inline const char* eto_string(eApplicationStatus status){
  switch (status) {
  case eApplicationStatus::Rejected:          return "Rejected";
  case eApplicationStatus::Completed:         return "Completed";
  case eApplicationStatus::PendingPayment:    return "Pending for Payment";
  case eApplicationStatus::WaitingForReview:  return "Waiting for Review";
  default:                                    return "Unknown";
  }
}

inline int date_to_int() {
  std::time_t t = std::time(nullptr);
  std::tm* now = std::localtime(&t);

  int year  = now->tm_year + 1900;
  int month = now->tm_mon + 1;
  int day   = now->tm_mday;

  return year * 10000 + month * 100 + day;
}

inline std::time_t int_to_timet(int date) {
  std::tm tm = {};
  tm.tm_year = (date / 10000) - 1900;
  tm.tm_mon  = ((date / 100) % 100) - 1;
  tm.tm_mday = date % 100;
  tm.tm_isdst = -1; 

  return std::mktime(&tm);
}

inline int get_days_betwen(int date1, int date2) {
  std::time_t t1 = int_to_timet(date1);
  std::time_t t2 = int_to_timet(date2);

  double seconds = std::difftime(t2, t1);
  return static_cast<int>(seconds / (60 * 60 * 24));
}

inline std::string date_to_string(int date){
  std::stringstream ss;
  ss  << date / 10000 << "-" << std::setw(2) << std::setfill('0')
      << (date / 100) % 100 << "-" << std::setw(2) << std::setfill('0')
      << date % 100;
  return ss.str();
}