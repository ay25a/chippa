#include "common.hpp"
#include "database/database.hpp"
#include "terminal/cli.hpp"

User ui_authentication() {
  cli_clear();
  cli_header("Authentication");
  uint32_t choice = cli_menu({"Login", "Register", "Exit"});

  switch (choice) {
  case 0:   return ui_login();
  case 1:   return ui_register();
  default:  exit(0);
  }
}

User ui_login(){
  for(;;){
    cli_clear();
    cli_header("Login User");

    std::string email;
    cli_input("Email Address: ", email);

    std::string password;
    cli_input("Password: ", password, true);

    auto found = db_match_entry(User{"", email, password}, true);
    if(found.size() != 0) 
      return found[0];
    
    cli_error("Wrong email or password!");
    
    if(!cli_bool("Try again?"))
      break;
  }

  return {};
}


User ui_register(){
  cli_clear();
  cli_header("New User");

  User newUser;
  newUser.Status = eUserStatus::Active;

  while(true){
    cli_input("Organization ID: ", newUser.UserID);
    auto exists = db_match_entry(User{newUser.UserID}, true);
    if(exists.size() > 0){
      cli_error("a User already exists with the same organization id");
      continue;
    }
    break;
  }

  while(true){
    cli_input("Email: ", newUser.Email);
    auto exists = db_match_entry(User{"", newUser.Email}, true);
    if(exists.size() > 0){
      cli_error("Email address already exists!");
      continue;
    }
    break;
  }

  cli_input("Password: ", newUser.Password, true);
  cli_input("Contact Number: ", newUser.ContactNumber);

  cli_subheader("Faculty");
  uint32_t choice = UI_FACULTY_MENU();
  newUser.Faculty = C_FACULTIES[choice];
  
  cli_subheader("User Type");
  choice = cli_menu({"Student", "Staff"});
  newUser.Role = static_cast<eUserRole>(choice+1);

  db_add_entry(newUser);
  return newUser;
}