#include "common.hpp"
#include "database/user.hpp"
#include "terminal/cli.hpp"

void ui_profile(const User& user){
  while(true){
    cli_clear();
    cli_header("Profile");
    cli_field("Organization ID", user.UserID);
    cli_field("Email", user.Email);
    cli_field("Password", std::string(user.Password.size(), '*'));
    cli_field("Contact Number", user.ContactNumber);
    cli_field("Faculty", FacultyToString(user.Faculty));
    cli_field("Status", (user.Status == UserStatus::Suspended ? "Suspended" : "Active"));

    if(gCurrentUser.UserID != user.UserID) return;

    cli_separator(10);
    auto choice = cli_menu({"Edit Information", "Go Back"});
  
    if(choice == 0)
      ui_edit_profile();
    else if(choice == 1)
      break;
  }

}

void ui_edit_profile(){
  cli_clear();
  cli_header("Edit profile");
  auto choice = cli_menu({"Email", "Password", "Contact Number", "Faculty", "Go Back"});

  switch (choice) {
  case 0: 
  while(true) {
    std::string email;
    cli_input("Email", email);
    auto exists = MatchUsers(User{"", email}).size() != 0;

    if(!exists) {
      gCurrentUser.Email = email;
      break;
    }
    else 
      cli_error("Email Address Already Exists");
  }
  break;
  case 1:
    cli_input("Password", gCurrentUser.Password);
    break;
  case 2:
    cli_input("Contact Number", gCurrentUser.ContactNumber);
    break;
  case 3: {
    auto choice = UI_FACULTY_MENU();
    gCurrentUser.Faculty = static_cast<Faculty>(choice + 1);
    break;
  }
  default:
    break;
  }
}



void ui_student_menu(){
  cli_clear();
  cli_header("Welcome, " + gCurrentUser.UserID);

  uint32_t choice = cli_menu({"Profile", "User Vehicles", "Parking Applications", "Parking Passes", "Exit"});
  switch (choice) {
    case 0:
      ui_profile(gCurrentUser);
      break;
    case 1:
    case 2:
    case 3:
    case 4:
      exit(0);
  }
}
