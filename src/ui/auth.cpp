#include "common.hpp"
#include "terminal/cli.hpp"
#include "database/user.hpp"

void Authenticate() {
  cli_clear();
  cli_header("Authentication");
  uint32_t choice = cli_menu({"Login", "Register", "Exit"});

  switch (choice) {
  case 0: // Login
    Login();
    break;
  case 1: // Register
    Register();
    break;
  default:
    exit(0);
  }
}

void Login(){
  for(;;){
    cli_clear();
    cli_header("Login User");

    std::string email;
    cli_input("Email Address: ", email);

    std::string password;
    cli_input("Password: ", password, true);

    auto found = MatchUsers({"", email, password}, true);
    if(found.size() != 0) {
      gCurrentUser = found[0];
      break;
    }
    
    cli_error("Wrong email or password!");
    
    if(!cli_bool("Try again?"))
      break;
  }
}


void Register(){
  cli_clear();
  cli_header("New User");

  User newUser;
  newUser.Status = UserStatus::Active;

  while(true){
    cli_input("Organization ID: ", newUser.UserID);
    auto exists = MatchUsers({newUser.UserID}, true);
    if(exists.size() > 0){
      cli_error("a User already exists with the same organization id");
      continue;
    }
    break;
  }

  while(true){
    cli_input("Email: ", newUser.Email);
    auto exists = MatchUsers({"", newUser.Email}, true);
    if(exists.size() > 0){
      cli_error("Email address already exists!");
      continue;
    }
    break;
  }

  cli_input("Password: ", newUser.Password, true);
  cli_input("Contact Number: ", newUser.ContactNumber);

  cli_subheader("Faculty");
  uint32_t choice = cli_menu({"LCK_FES", "FCI", "FAM", "FCS", "FEd", "MK_FMHS"});
  newUser.Faculty = static_cast<Faculty>(choice+1);
  
  cli_subheader("User Type");
  choice = cli_menu({"Student", "Staff"});
  newUser.Role = static_cast<UserRole>(choice+1);
  
  CreateUser(newUser);
  gCurrentUser = newUser;
}