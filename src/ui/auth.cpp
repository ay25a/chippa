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

    // Match
    auto found = MatchUsers(sUser("", email, password), true);
    if(found.size() != 0) {
      gCurrentUser = found[0];
      break;
    }
    
    cli_error("Wrong email or password!");
    
    if(!cli_bool("Try again? "))
      break;
  }
}


void Register(){
  cli_clear();
  cli_header("New User");

  sUser newUser;
  cli_input("Organization ID: ", newUser.UserID);
  cli_input("Email: ", newUser.Email);
  // match
  cli_input("Password: ", newUser.Password, true);
  // validate
  cli_input("Contact Number: ", newUser.ContactNumber);

  cli_subheader("Faculty");
  uint32_t choice = cli_menu({"LCK_FES", "FCI", "FAM"});
  newUser.Faculty = static_cast<eFaculty>(choice+1);

  newUser.Status = eUserStatus::Active;

  cli_subheader("User Type");
  choice = cli_menu({"Student", "Staff"});
  newUser.Role = static_cast<eUserRole>(choice+1);

  CreateUser(newUser);
  SetActiveUser(newUser.UserID);
}