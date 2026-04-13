#include "terminal/formatter.hpp"
#include "ui/common.hpp"
#include "database/database.hpp"
#include "database/user.hpp"


sANSIFormatter gFmt;

int main(int argc, char **argv) {
  InitializeDatabase();
  LoadUsers();

  while(gCurrentUser.UserID == "") 
    ui_authentication();

  if(gCurrentUser.Role == UserRole::Student) {
    while(true) ui_student_menu();
  }
  else ui_staff_menu();
  
  return 0;
}