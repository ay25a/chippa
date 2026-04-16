#include "terminal/formatter.hpp"
#include "ui/elements.hpp"

sANSIFormatter gFmt;
User gCurrentUser = {};

int main(int argc, char **argv) {
  //if(!cli_bool("Does this text display correctly?"))
  //  DisableFormatter();

  for(;;){
    if(ui_authentication()) 
      break;
  }

  if(gCurrentUser.role == eUserRole::Student)
    for(;;) ui_student();
  else
    for(;;) ui_staff(); 

  return 0;
}