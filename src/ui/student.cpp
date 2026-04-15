#include "common.hpp"
#include "database/database.hpp"
#include "terminal/cli.hpp"


void ui_student_menu(){
  cli_clear();
  cli_header("Welcome, " + gCurrentUser.UserID);

  auto choice = cli_menu({"Profile", "Vehicles", "Parking Applications", "Parking Pass", "Exit"});
  switch (choice) {
    case 0:
      ui_profile(gCurrentUser);
      break;
    case 1:
      ui_vehicles(gCurrentUser);
      break;
    case 2:
      ui_user_applications(gCurrentUser);
      break;
    case 3:
      ui_view_user_pass(gCurrentUser);
      break;
    case 4:
      exit(0);
  }
}


void ui_profile(const User& user){
  REPEATED_BEGIN()
  cli_clear();
  cli_header("Profile");
  cli_field("User ID", user.UserID);
  cli_field("Email", user.Email);
  cli_field("Password", std::string(user.Password.size(), '*'));
  cli_field("Contact Number", user.ContactNumber);
  cli_field("Faculty", user.Faculty);
  cli_field("Status", (user.Status == eUserStatus::Suspended ? "Suspended" : "Active"));

  if(gCurrentUser.UserID != user.UserID) 
    return;

  cli_separator(10);
  auto choice = cli_menu({"Edit", "Back"});
  if(choice == 1)
    return;

  ui_edit_profile();

  REPEATED_END()
}

void ui_edit_profile(){
  REPEATED_BEGIN()
  cli_clear();
  cli_header("Edit Profile");
  bool modified = false;
  
  auto choice = cli_menu({"Email", "Password", "Contact Number", "Faculty", "Back"});
  switch (choice) {

  case 0: {
    std::string email;
    cli_input("Email: ", email);
    auto exists = db_match_entry(User{"", email}).size() != 0;
    if(exists){
      cli_error("Email Address Already Exists");
      std::cin.get();
      break;
    }

    gCurrentUser.Email = email;
    modified = true;
    break;
  }

  case 1:
    cli_input("Password: ", gCurrentUser.Password);
    modified = true;
    break;
  
  case 2:
    cli_input("Contact Number: ", gCurrentUser.ContactNumber);
    modified = true;
    break;
  

  case 3: {
    auto fac = UI_FACULTY_MENU();
    if(gCurrentUser.Faculty == C_FACULTIES[fac]) 
      break;

    gCurrentUser.Faculty = C_FACULTIES[fac];
    modified = true;
    break;
  }

  case 4:
    return;
  }

  if(modified) {
    db_update_entry(gCurrentUser);
    return;
  }

  REPEATED_END()
}

void ui_vehicles(const User &user){
  REPEATED_BEGIN()
  cli_clear();
  cli_header("Vehicles");

  std::vector<Vehicle> registerd = db_match_entry(Vehicle{gCurrentUser.UserID});
  cli_field("Registered", std::to_string(registerd.size()));

  std::vector<std::vector<std::string>> values;
  for(const auto& vh: registerd)
    values.push_back({vh.LicensePlate, vh.Model, vh.Owner});
  cli_table({"License Plate", "Model", "Owner"}, values);
  cli_separator(5);

  auto choice = cli_menu({"Add", "Edit", "Go Back"});
    switch (choice) {
    case 0:
      ui_add_vehicle();
      break;
    case 1: {
      cli_clear();
      std::vector<const char*> plates;
      for(const auto& vh: registerd)
        plates.emplace_back(vh.LicensePlate.c_str());

      plates.emplace_back("Cancel");
      auto vh = cli_menu(plates);
      if(vh != plates.size() - 1)
        ui_edit_vehicle(registerd[vh]);

      break;
    }
    case 2:
      return;
    }
  REPEATED_END()
}

void ui_add_vehicle() {
  cli_clear();
  cli_header("Add a new Vehicle");

  Vehicle veh{gCurrentUser.UserID};
  while(true){
    cli_input("License Plate: ", veh.LicensePlate);

    auto exists = !db_match_entry(veh).empty();
    if(!exists) break;
    
    cli_error("License plate already registered!");
  }

  cli_input("Model: ", veh.Model);
  cli_input("Owner (default: " + gCurrentUser.UserID + "): ", veh.Owner);
  if(veh.Owner.empty()) 
    veh.Owner = gCurrentUser.UserID;

  db_add_entry(veh);
}

void ui_vehicle_details(const Vehicle& vh){
  cli_header(vh.LicensePlate);
  cli_field("Owner", vh.Owner);
  cli_field("Model", vh.Model);
}

void ui_edit_vehicle(const Vehicle& vh){}

void ui_user_applications(const User& user){}

void ui_view_user_pass(const User& user){}


