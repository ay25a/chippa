#include "terminal/formatter.hpp"
#include "ui/common.hpp"
#include "database/database.hpp"
#include "database/user.hpp"


sANSIFormatter gFmt;

int main(int argc, char **argv) {
  InitializeDatabase();
  LoadUsers();

  while(gCurrentUser.UserID == "") 
    Authenticate();

  if(gCurrentUser.Role == UserRole::Student) {
    while(true) ShowStudentMenu();
  }
  else ShowStaffMenu();
  
  return 0;
}