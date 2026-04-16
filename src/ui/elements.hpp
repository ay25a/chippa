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

extern std::string validate_password(std::string_view in, std::string& out);
extern std::string validate_contact_number(std::string_view in, std::string& out);
extern std::string validate_full_name(std::string_view in, std::string& out);
extern std::string validate_age(std::string_view in, int& out);

// Authentication
extern User gCurrentUser;
extern bool ui_authentication();
extern User ui_login();
extern User ui_register();

// Menu
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
  if(modified = modified || !name.empty(); !name.empty()){
    name.resize(32);
    std::copy(name.begin(), name.end(), &gCurrentUser.fullname[0]);
  }

  if(modified = modified || !contact.empty(); !contact.empty()){
    contact.resize(12);
    std::copy(contact.begin(), contact.end(), &gCurrentUser.contactNumber[0]);
  }

  if(modified = modified || !password.empty(); !password.empty()){
    password.resize(12);
    std::copy(password.begin(), password.end(), &gCurrentUser.password[0]);
  }

  if(modified = modified || !faculty.empty(); !faculty.empty()){
    faculty.resize(10);
    std::copy(faculty.begin(), faculty.end(), &gCurrentUser.faculty[0]);
  }

  if(modified = modified || age != 0; age != 0)
    gCurrentUser.age = age;

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