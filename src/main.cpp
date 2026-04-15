#include "terminal/formatter.hpp"
#include "ui/common.hpp"
#include "database/database.hpp"

sANSIFormatter gFmt;
User gCurrentUser = {};

int main(int argc, char **argv) {
  db_load(eDatabaseEntity::User);

  while(gCurrentUser.UserID.empty()) 
    gCurrentUser = ui_authentication();

  db_load(eDatabaseEntity::Vehicle);
  db_load(eDatabaseEntity::Application);
  db_load(eDatabaseEntity::Pass);

  if(gCurrentUser.Role == eUserRole::Student)
    while(true) ui_student_menu();
  else 
    while (true) ui_staff_menu();
  
  return 0;
}