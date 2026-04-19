#include "database.hpp"
#include "ui_elements.hpp"

User gCurrentUser = {};
bool Fmt::Enabled = true;

void InitPasses();

int main(int argc, char **argv) {
  // if(!cli_boolean("Does this text display correctly?"))
  //  Fmt::Enabled = false;

  bool authenticated = false;
  while(!authenticated){
    cli_clear();
    cli_header("Login or Create a new Account!");

    switch (cli_menu({"Login", "Create Account", "Exit"})) {
    case 0:
      authenticated = ui_login();
      break;
    case 1:   
      authenticated = ui_register();
      break;
    default:  
      return 0;
    }
  }

  InitPasses();

  if(gCurrentUser.role == eUserRole::Student){
    if(gCurrentUser.status == eUserStatus::Suspended){
      cli_error("Your account has been suspended!");
      return 1;
    }

    for(;;) 
      ui_student_menu();
  }
  else if(gCurrentUser.role == eUserRole::Staff)
    for(;;) 
      ui_staff_menu(); 

  return 0;
}

void InitPasses() {
  ParkingPass filter{};
  filter.status = ePassStatus::Active;

  std::vector<ParkingPass> passes = db_find<ParkingPass>(filter);
  std::vector<ParkingPass> modified;
  int currentDate = date_to_int();

  for(auto& p: passes){
    int days = get_days_betwen(currentDate, p.issueDate);
    if(days <= 0){
      p.status = ePassStatus::Expired;
      modified.push_back(p);
    }
  }

  for(const auto& mod: modified)
    EXPECT(db_update_record(mod));
}