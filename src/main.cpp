#include "ui_elements.hpp"

User gCurrentUser = {};
bool Fmt::Enabled = true;

int main(int argc, char **argv) {
  // if(!cli_boolean("Does this text display correctly?"))
  //  Fmt::Enabled = false;

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