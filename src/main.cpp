#include "terminal/formatter.hpp"
#include "ui/common.hpp"
#include "database/database.hpp"

sANSIFormatter gFmt;
User gCurrentUser = {};

int main(int argc, char **argv) {
  db_load(eDatabaseEntity::User);

  while(gCurrentUser.UserID.empty()) 
    gCurrentUser = ui_authentication();

  if(gCurrentUser.Role == eUserRole::Student) {
    while(true) ui_student_menu();
  }
  else ui_staff_menu();
  
  return 0;
}