#include "terminal/cli.hpp"

void LoginUser();
void Authenticate() {
  cli_clear();
  cli_header("Authentication");
  auto choice = cli_menu({"Login", "Register", "Exit"});
}

int main(int argc, char **argv) {
  // Choice to enable/disable formatter
  // Choice to enable/disable interactive tui (to be implemented)
  //Authenticate();
  
  Authenticate();
  return 0;
}

/*
void AuthenticateUser() {
  constexpr unsigned short MODE_LOGIN = 1;
  constexpr unsigned short MODE_REGISTER = 2;
  constexpr unsigned short MODE_EXIT = 3;

  bool isExit = false;
  int choice = 0;
  while (!isExit) {
    std::cout << "\033[0H"
              << "(" << MODE_LOGIN << ") Login\n"
              << "(" << MODE_REGISTER << ") Register\n"
              << "(" << MODE_EXIT << ") Exit\n";

    std::cout << "Choose menu item: ";
    std::cin >> choice;

    switch (choice) {
    case MODE_LOGIN:
      isExit = LoginUser();
      break;
    case MODE_REGISTER:
      isExit = RegisterUser();
      break;
    case MODE_EXIT:
      std::cout << "Exiting...      \n";
      isExit = true;
      break;
    default:
      std::cout << "Invalid choice! \n";
      break;
    }
  }
}

bool LoginUser() {
  CLEAN_SCREEN();
  std::string email, password;

  std::cout << "Email: ";
  std::cin >> email;

  std::cout << "Password: ";
  std::cin >> password;

  sUser user = MatchUser({"", email, password});
  if (user.UserID.empty())
    return false;

  gCurrentUser = user;
  return true;
}

sUser MatchUser(const sUser &filter) {
  std::string content = ReadFile(gDatabasePath.string() + DB_USERS_PATH);
  for (size_t i = 0; i < content.size(); ++i) {
    std::vector<std::string> attrs = ReadAttributes(content.c_str(), i);
    assert(attrs.size() == 7);
    asd|asd|asd
    filter = {"kuykuyqw", "email", "[ass]"}
    "asd", "asd", "asd"

    if (!filter.UserID.empty() && attrs[0] != filter.UserID)
      continue;
    if (!filter.Email.empty() && attrs[1] != filter.Email)
      continue;
    if (!filter.Password.empty() && attrs[2] != filter.Password)
      continue;
    if (!filter.ContactNumber.empty() && attrs[3] != filter.ContactNumber)
      continue;
    if (filter.Faculty != eFaculty::Unknown &&
        static_cast<eFaculty>(std::stoi(attrs[4])) != filter.Faculty)
      continue;
    if (filter.Status != eUserStatus::Unknown &&
        static_cast<eUserStatus>(std::stoi(attrs[5])) != filter.Status)
      continue;
    if (filter.Role != eUserRole::Unknown &&
        static_cast<eUserRole>(std::stoi(attrs[6])) != filter.Role)
      continue;

    return {
        attrs[0],
        attrs[1],
        attrs[2],
        attrs[3],
        static_cast<eFaculty>(std::stoi(attrs[4])),
        static_cast<eUserStatus>(std::stoi(attrs[5])),
        static_cast<eUserRole>(std::stoi(attrs[6])),
    };
  }

  return {};
}

bool RegisterUser() {}
*/