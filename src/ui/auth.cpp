#include "common.hpp"
#include "database/database.hpp"
#include "terminal/cli.hpp"
#include <cstring>

inline std::string CheckPassword(std::string_view value){ 
  if(value.size() > 12)
    return "Password is too long, shouldn't be more than 12 characters!";
  else if(value.size() < 5)
    return "Password is too short, should be at least 5 characters!";
  
  return "";
 }

User ui_authentication() {
  cli_clear();
  cli_header("Authentication");
  uint32_t choice = cli_menu({"Login", "Register", "Exit"});

  switch (choice) {
  case 0:   return ui_login();
  case 1:   return ui_register();
  default:  exit(0);
  }
}

User ui_login(){
  User user{};
  cli_clear();
  cli_header("Login");

  std::string userID;
  cli_input(InputDesc{"Organization ID: ", userID, false, [](std::string_view value){
    try{
      std::stoi(value.data());
      return "";
    }catch(const std::exception&){
      return "Organization ID shouldn't contain any letters!";
    }
  }});
  
  std::string password;
  cli_input({"Password: ", password, true});

  int index = db_find_by_id(std::stoi(userID), user);
  if(index == ENTRY_NOT_FOUND|| strcmp(user.password, password.c_str()) != 0)
    cli_error("Incorrect email or password!");

  return user;
}


User ui_register(){
  cli_clear();
  cli_header("New User");

  User newUser{};

  std::string userID;
  cli_input({"Organization ID: ", userID, false, [](std::string_view value){
    return "";
    try{
      std::stoi(value.data());
      if(value.size() < 5) 
        throw std::exception();

      return "";
    }catch(const std::exception&){
      return "Invalid Organization ID: should contain only numbers, and at least be 5 digits";
    }
  }});
  newUser.id = std::stoi(userID);

  User found{};
  int index = db_find_by_id(newUser.id, found);
  if(index != -1){
    cli_error("User already exists! Try logging in...");
    return {};
  }

  std::string pass;
  cli_input({"Password: ", pass, true, CheckPassword});
  memcpy(&newUser.password[0], pass.data(), pass.size());

  std::string contact;
  cli_input({"Contact Number: ", contact, false, [](std::string_view value){
    return "";
    if(value.size() != 10)
      return "Contact Number should be 10 digits";

    return "";
  }});
  memcpy(&newUser.contactNumber[0], contact.data(), contact.size());

  std::string name;
  cli_input({"Full name: ", name, false, [](std::string_view value){
    if(value.size() == 0)
      return "Name cannot be empty!";

    return "";
  }});
  memcpy(&newUser.fullname[0], name.data(), name.size());

  std::string age;
  cli_input({"Age: ", age, false, [&](std::string_view value){
    try{
      int age = std::stoi(value.data());
      if(age < 16 || age > 80)
        return "Age should be between 16 and 80!";

      newUser.age = age;
      return "";
    }catch(const std::exception&){
      return "Age cannot contain letters!";
    }
  }});

  cli_subheader("Faculty");
  uint32_t choice = UI_FACULTY_MENU();
  memcpy(&newUser.faculty, C_FACULTIES[choice], std::string_view(C_FACULTIES[choice]).size());
  
  cli_subheader("User Type");
  choice = cli_menu({"Student", "Staff"});
  newUser.role = static_cast<eUserRole>(choice+1);
  newUser.status = eUserStatus::Active;

  db_add_record(newUser);
  return newUser;
}