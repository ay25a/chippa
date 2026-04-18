#pragma once

#include "database/database.hpp"
#include "database/entities.hpp"
#include "terminal/cli.hpp"
#include <string>

// Helpers
#define UI_FACULTY_MENU() cli_menu(std::vector<std::string>(std::begin(C_FACULTIES), std::end(C_FACULTIES)))
#define FOR_LOOP_BEGIN() for(;;) {
#define FOR_LOOP_BREAK() break
#define FOR_LOOP_END() }

extern std::string validate_password(const std::string& in, std::string& out);
extern std::string validate_contact_number(const std::string& in, std::string& out);
extern std::string validate_full_name(const std::string& in, std::string& out);
extern std::string validate_age(const std::string& in, int& out);

// Authentication
extern User gCurrentUser;
extern bool ui_authentication();
extern User ui_login();
extern User ui_register();

// Student related Menu
extern void ui_add_vehicle();
extern void ui_delete_vehicle(const std::vector<Vehicle>& owned);
extern void ui_vehicles(const User &user);

extern void ui_passes(const User& user);
extern void ui_applications(const User& user);

extern void ui_student();

extern void ui_staff();


inline void ui_edit_profile(){
  cli_clear();
  cli_header("Edit Profile");
  
  std::string name, contact, password, faculty;
  int age = 0;
  switch (cli_menu({"Name", "Age", "Contact Number", "Password", "Faculty", "Back"})) {
  case 0:
    name = cli_input_valid<std::string>("New Name: ", validate_full_name);
    break;
  case 1:
    age = cli_input_valid<int>("New Age: ", validate_age);
    break;
  case 2:
    contact = cli_input_valid<std::string>("New Contact Number: ", validate_contact_number);
    break;
  case 3:
    password = cli_input_valid<std::string>("New Password: ", validate_password);
    break;
  case 4:
    faculty = C_FACULTIES[UI_FACULTY_MENU()];
    break;
  case 5:
    return;
  }

  if(!cli_boolean("Confirm changes?"))
    return;

  bool modified = false;
  if(!name.empty()){
    modified = true;
    name.resize(32);
    std::copy(name.begin(), name.end(), &gCurrentUser.fullname[0]);
  }

  if(!contact.empty()){
    modified = true;
    contact.resize(12);
    std::copy(contact.begin(), contact.end(), &gCurrentUser.contactNumber[0]);
  }

  if(!password.empty()){
    modified = true;
    password.resize(12);
    std::copy(password.begin(), password.end(), &gCurrentUser.password[0]);
  }

  if(!faculty.empty()){
    modified = true;
    faculty.resize(10);
    std::copy(faculty.begin(), faculty.end(), &gCurrentUser.faculty[0]);
  }

  if(age != 0){
    modified = true;
    gCurrentUser.age = age;
  }

  if(modified)
    if(!db_update_record(gCurrentUser)){
      cli_error("Cannot write to the database file!");
      cli_confirm();
    }
}

inline void ui_profile(const User& user){
  FOR_LOOP_BEGIN()
  cli_clear();
  cli_header("Profile");
  cli_field((user.role == eUserRole::Student ? "Student " : "Staff ") << "ID", user.id);
  cli_field("Name", user.fullname);
  cli_field("Age", user.age);
  cli_field("Contact Number", user.contactNumber);
  cli_field("Faculty", user.faculty);
  cli_field("Status", (user.status == eUserStatus::Suspended ? "Suspended" : "Active"));
  cli_field("Role", (user.role == eUserRole::Student ? "Student" : "Staff"));

  if(user.id == gCurrentUser.id){
    cli_separator(10);
    switch(cli_menu({"Edit", "Back"})){
      case 0: ui_edit_profile(); break;
      case 1: return;
    }
  }
  FOR_LOOP_END()
}

constexpr const char* PASS_DURATION_STRING(ePassDuration d){
  switch(d){
    case ePassDuration::OneMonth: return "1 Month";
    case ePassDuration::TwoMonths: return "2 Months";
    case ePassDuration::ThreeMonths: return "3 Months";
    default: return "Unknown";
  }
}

constexpr const char* PASS_STATUS_STRING(ePassStatus s){
  switch(s){
    case ePassStatus::Active: return "Active";
    case ePassStatus::Suspended: return "Suspended";
    case ePassStatus::Expired: return "Expired";
    default: return "Unknown";
  }
}