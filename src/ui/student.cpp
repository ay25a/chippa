#include "common.hpp"
#include "database/user.hpp"
#include "terminal/cli.hpp"

void ShowStudentProfile(){
  cli_clear();
  ShowProfile();
  cli_separator(10);
    
  uint32_t choice = cli_menu({"Edit Information", "Go Back"});
  switch (choice) {
    case 0:
      break;
    case 1:
      break;
  }
}

void ShowStudentMenu(){
  cli_clear();
  cli_header("Welcome, " + gCurrentUser.UserID);

  uint32_t choice = cli_menu({"Profile", "User Vehicles", "Parking Applications", "Parking Passes", "Exit"});
  switch (choice) {
    case 0:
      ShowStudentProfile();
      break;
    case 1:
    case 2:
    case 3:
    case 4:
      exit(0);
  }
}
