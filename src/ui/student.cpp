#include "common.hpp"
#include "database/database.hpp"
#include "terminal/cli.hpp"

void ui_profile(const User& user){
  while(true){
    cli_clear();
    cli_header("Profile");
    cli_field("Organization ID", user.UserID);
    cli_field("Email", user.Email);
    cli_field("Password", std::string(user.Password.size(), '*'));
    cli_field("Contact Number", user.ContactNumber);
    cli_field("Faculty", user.Faculty);
    cli_field("Status", (user.Status == eUserStatus::Suspended ? "Suspended" : "Active"));

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

  bool modified = false;
  switch (choice) {
  case 0: 
  while(true) {
    std::string email;
    cli_input("Email", email);
    auto exists = db_match_entry(User{"", email}).size() != 0;

    if(!exists) {
      gCurrentUser.Email = email;
      modified = true;
      break;
    }
    else 
      cli_error("Email Address Already Exists");
  }
  break;
  case 1:
    cli_input("Password", gCurrentUser.Password);
    modified = true;
    break;
  case 2:
    cli_input("Contact Number", gCurrentUser.ContactNumber);
    modified = true;
    break;
  case 3: {
    auto choice = UI_FACULTY_MENU();
    if(gCurrentUser.Faculty == C_FACULTIES[choice]) 
      break;
    gCurrentUser.Faculty = C_FACULTIES[choice];
    modified = true;
    break;
  }
  default:
    break;
  }
  
  if(modified)
    db_update_entry(gCurrentUser);
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
