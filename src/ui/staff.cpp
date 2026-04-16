#include "elements.hpp"
#include "terminal/cli.hpp"

void ui_staff() {
  cli_clear();
  cli_header("Welocme " << gCurrentUser.fullname << '!');

  switch (cli_menu({"Staff Profile", "Exit"})) {
    case 0:
      ui_profile(gCurrentUser);
      break;
    case 1:
      exit(0);
  }
}