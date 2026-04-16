#include "terminal/cli.hpp"
#include "terminal/formatter.hpp"
#include "ui/common.hpp"
#include "database/database.hpp"

sANSIFormatter gFmt;
User gCurrentUser = {};

int main(int argc, char **argv) {
  //if(!cli_bool("Does this text display correctly?"))
  //  DisableFormatter();

  while(gCurrentUser.id == 0) 
    gCurrentUser = ui_authentication();
/*
  db_load_vehicles();
  db_load_passes();
  db_load_applications();

  if(gCurrentUser.Role == eUserRole::Student)
    while(true) ui_student_menu();
  else 
    while (true) ui_staff_menu();
  */
  return 0;
}