#include "UI.hpp"

User gCurrentUser = {};
bool Fmt::Enabled = true;

/// @brief a Function from `seed_database.cpp` file used to genereate dummy data
/// to the database.
extern void seed_database(int count = 100);

/// @brief Since on old terminal ANSI escape codes doesn't work correctly
void ShowDemo(){
  cli_header("Header");
  cli_subheader("Subheader");
  if(!cli_boolean("Does this text display correctly?"))
    Fmt::Enabled = false;
}

int main(void) {
  ShowDemo();
  if(cli_boolean("Generate dummy data? "))
    seed_database();
  
  while(AuthenticationPage() == ePageState::Continue);

  MenuPage();
  return 0;
}