#include "UI.hpp"

User gCurrentUser = {};
bool Fmt::Enabled = true;

int main(void) {
  if(!cli_boolean("Does this text display correctly?"))
    Fmt::Enabled = false;

  while(AuthenticationPage() == ePageState::Continue);

  MenuPage();
  return 0;
}