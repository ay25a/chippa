#include "UI.hpp"

User gCurrentUser = {};
bool Fmt::Enabled = true;

int main(int argc, char **argv) {
  if(!cli_boolean("Does this text display correctly?"))
    Fmt::Enabled = false;

  while(AuthenticationPage() == ePageState::Continue);

  MenuPage();
  return 0;
}