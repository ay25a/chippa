#include "terminal/formatter.hpp"
#include "ui/common.hpp"
#include "database/database.hpp"
#include "database/user.hpp"
#include <iostream>


sANSIFormatter gFmt;

int main(int argc, char **argv) {
  // Choice to enable/disable formatter
  // Choice to enable/disable interactive tui (to be implemented)
  InitializeDatabase();
  LoadUsers();

  while(!gCurrentUser) 
    Authenticate();

  if(gCurrentUser->Role == eUserRole::Student){
    // Show student menu
  }
  else{
    // show staff menu
  }
  return 0;
}