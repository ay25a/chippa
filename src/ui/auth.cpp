#include "elements.hpp"
#include "database/database.hpp"
#include "terminal/cli.hpp"

bool ui_authentication() {
  cli_clear();
  cli_header("Authentication");

  switch (cli_menu({"Login", "Register", "Exit"})) {
  case 0:   
    gCurrentUser = ui_login();
    break;
  case 1:   
    gCurrentUser = ui_register();
    break;
  default:  
    exit(0);
  }

  return gCurrentUser.id != 0;
}

User ui_login(){
  cli_clear();
  cli_header("Login");
  
  
  int userid = cli_input_valid<int>("User ID: ", [](const std::string& in, int& out) {
    return !StringToInt(in, out) ? "User id can only contain numbers!" : "";
  });
  
  std::string password;
  cli_input("Password: ", password);

  User user{};
  bool found = db_find_by_id<User>(userid, &user);
  if(!found || strcmp(user.password, password.c_str()) != 0){
    cli_error("Incorrect email or password!");
    cli_confirm();
    return {};
  }

  return user;
}

User ui_register(){
  cli_clear();
  cli_header("New User");

  int userid = cli_input_valid<int>("User ID (Organization ID): ", [](const std::string& in, int& out) {
    if(!StringToInt(in, out)) 
      return "User id can only contain numbers!";

    return in.size() < 5 ? "Invalid user id!" : "";
  });

  if(db_find_by_id<User>(userid, nullptr)){
    cli_error("User already exists! Try logging in...");
    cli_confirm();
    return {};
  }

  std::string password = cli_input_valid<std::string>("Password: ", validate_password);
  std::string contact = cli_input_valid<std::string>("Contact Numer: ", validate_contact_number);
  std::string name = cli_input_valid<std::string>("Full Name: ", validate_full_name);
  int age = cli_input_valid<int>("Age: ", validate_age);

  cli_subheader("Faculty");
  auto choice = UI_FACULTY_MENU();
  const std::string& faculty = C_FACULTIES[choice];

  cli_subheader("User Type");
  auto role = cli_menu({"Student", "Staff"}) + 1;

  User user {userid};
  std::copy(password.begin(), password.end(), &user.password[0]);
  std::copy(contact.begin(), contact.end(), &user.contactNumber[0]);
  std::copy(name.begin(), name.end(), &user.fullname[0]);
  std::copy(faculty.begin(), faculty.end(), &user.faculty[0]);
  user.age = age;
  user.status = eUserStatus::Active;
  user.role = static_cast<eUserRole>(role);

  if(!db_add_record(user)){
    cli_error("Failed to add entry to database!");
    cli_confirm();
    return {};
  }
  
  return user;
}

std::string validate_password(const std::string& in, std::string &out){
  if(in.size() < 5 || in.size() >= 12)
      return "Password needs to be from 5 to 11 characters!";
  out = in;
  return "";
}

std::string validate_contact_number(const std::string& in, std::string &out){
  int dummy = 0;
  if(!StringToInt(in, dummy) || in.size() != 10)
    return "Please enter valid contact number!";
  out = in;
  return "";
}

std::string validate_full_name(const std::string& in, std::string &out){
  if(in.size() < 3 || in.size() > 31)
    return "Name needs to be at least 3 letters and at most 31 letters!";
  out = in;
  return "";
}

std::string validate_age(const std::string& in, int &out){
  if(!StringToInt(in, out))
    return "Age can only contain numbers!";
  return (out < 16 || out > 99) ? "Age should be between 16 and 99" : "";
}