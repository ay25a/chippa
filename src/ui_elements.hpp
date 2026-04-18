#pragma once

#include "entities.hpp"
#include "cli_core.hpp"
#include <string>
#include <functional>

// Helpers
inline std::string validator(const std::string& prompt, std::function<std::string(const std::string&)> validate){
  for(;;){
    std::string in = cli_input(prompt);

    std::string error = validate(in);
    if(error.empty())
      return in;

    cli_error(error);
  }
}

inline std::string validate_password(const std::string& in){
  return (in.size() < 5 || in.size() >= 12) ? "Password length should be 5 to 11 characters" : "";
}

inline std::string validate_contact_number(const std::string& in){
  return in.size() < 10 || !is_integer(in) ? "Invalid contact number" : "";
}

inline std::string validate_name(const std::string& in){
  return in.size() < 3 || in.size() > 31 ? "Name length should be 3 to 31 characters" : "";
}

inline std::string validate_age(const std::string& in){
  if(!is_integer(in))
    return "Age can only be numbers";
  int age = std::stoi(in);
  return (age < 16 || age > 99) ? "Age should be between 16 and 99" : "";
}

inline const char* enum_to_string(ePassStatus status){
  switch (status) {
  case ePassStatus::Active:     return "Active";
  case ePassStatus::Suspended:  return "Suspended";
  case ePassStatus::Expired:    return "Expired";
  default:                      return "unknown";
  }
}

inline const char* enum_to_string(ePassDuration duration){
  switch (duration) {
  case ePassDuration::OneMonth:     return "1 Month";
  case ePassDuration::TwoMonths:    return "2 Months";
  case ePassDuration::ThreeMonths:  return "3 Months";
  default:                          return "unknown";
  }
}

inline const char* enum_to_string(eApplicationStatus status){
  switch (status) {
  case eApplicationStatus::Rejected:          return "Rejected";
  case eApplicationStatus::Completed:         return "Completed";
  case eApplicationStatus::PendingPayment:    return "Pending for Payment";
  case eApplicationStatus::WaitingForReview:  return "Waiting for Review";
  default:                                    return "Unknown";
  }
}

// Authentication
extern User gCurrentUser;
extern bool ui_authentication();
extern User ui_login();
extern User ui_register();

// Student related Menu
extern void ui_view_profile(const User& user);
extern void ui_edit_current_profile();

extern void ui_view_vehicles(const User& user);
extern void ui_add_vehicle();
extern void ui_delete_vehicle(const std::vector<Vehicle>& userVehicles);;

extern void ui_view_passes(const User& user);
extern void ui_view_applications(const User& user);

extern void ui_student();

extern void ui_staff();

