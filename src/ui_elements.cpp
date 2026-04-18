#include "ui_elements.hpp"
#include "cli_core.hpp"
#include "database.hpp"

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
  
  int userid = std::stoi(validator("User ID: ", [](const std::string& in){
    if(!is_integer(in))
      return "User id can only contain numbers!";
    return "";
  }));

  
  std::string password = cli_input("Password: ");

  User user{};
  bool found = db_find_by_id<User>(userid, &user);
  if(!found || strcmp(user.password, password.c_str()) != 0){
    cli_error("Incorrect email or password!");
    cli_press_enter();
    return {};
  }

  return user;
}

User ui_register(){
  cli_clear();
  cli_header("Create a User");

  int userid = std::stoi(validator("User ID (Organization ID): ", [](const std::string& in) {
    if(!is_integer(in) || in.size() < 4) 
      return "User id should be at least 4 numbers, can cannot contain any letters";
    return "";
  }));

  if(db_find_by_id<User>(userid, nullptr)){
    cli_error("User already exists! Try logging in...");
    cli_press_enter();
    return {};
  }

  std::string password = validator("Password: ", validate_password);
  std::string contact = validator("Contact Numer: ", validate_contact_number);
  std::string name = validator("Full Name: ", validate_name);
  int age = std::stoi(validator("Age: ", validate_age));

  cli_subheader("Faculty");
  auto choice = cli_menu({std::begin(C_FACULTIES), std::end(C_FACULTIES)});
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

  if(!db_add_record(user))
    throw std::runtime_error("Unexpected Failure: Failed to add entry to Database");
  
  return user;
}

void ui_student() {
  cli_clear();
  cli_header("Welocme " << gCurrentUser.fullname << '!');

  switch (cli_menu({"Student Profile", "Registered Vehicles", "Parking Passes", "Applications", "Exit"})) {
    case 0:
      ui_view_profile(gCurrentUser);
      break;
    case 1:
      ui_view_vehicles(gCurrentUser);
      break;
    case 2:
      ui_view_passes(gCurrentUser);
      break;
    case 3:
      ui_view_applications(gCurrentUser);
      break;
    case 4:
      exit(0);
  }
}

void ui_staff(){
  cli_clear();
  cli_header("Welocme " << gCurrentUser.fullname << '!');
}

void ui_view_profile(const User& user){
  for(;;){
    cli_clear();
    cli_header("Profile");
    cli_field((user.role == eUserRole::Student ? "Student " : "Staff ") << "ID", user.id);
    cli_field("Name", user.fullname);
    cli_field("Age", user.age);
    cli_field("Contact Number", user.contactNumber);
    cli_field("Faculty", user.faculty);
    cli_field("Status", (user.status == eUserStatus::Suspended ? "Suspended" : "Active"));
    cli_field("Role", (user.role == eUserRole::Student ? "Student" : "Staff"));

    if(user.id == gCurrentUser.id){
      cli_separator(10);
      switch(cli_menu({"Edit", "Back"})){
        case 0: 
          ui_edit_current_profile(); 
          break;
        case 1: 
          return;
      }
    }
  }
}

void ui_edit_current_profile(){
  cli_clear();
  cli_header("Edit Profile");
  
  std::string name, contact, password, faculty;
  int age = 0;

  bool modified = false;
  switch (cli_menu({"Name", "Age", "Contact Number", "Password", "Faculty", "Back"})) {
  case 0:
    name = validator("New Name: ", validate_name);
    modified = (name != gCurrentUser.fullname);
    break;
  case 1:
    age = std::stoi(validator("New Age: ", validate_age));
    modified = (age != gCurrentUser.age);
    break;
  case 2:
    contact = validator("New Contact Number: ", validate_contact_number);
    modified = (contact != gCurrentUser.contactNumber);
    break;
  case 3:
    password = validator("New Password: ", validate_password);
    modified = (password != gCurrentUser.password);
    break;
  case 4:
    faculty = C_FACULTIES[cli_menu({std::begin(C_FACULTIES), std::end(C_FACULTIES)})];
    modified = (faculty != gCurrentUser.faculty);
    break;
  case 5:
    return;
  }

  if(!cli_boolean("Confirm changes?") || !modified)
    return;

  if(!name.empty()){
    name.resize(32);
    std::copy(name.begin(), name.end(), &gCurrentUser.fullname[0]);
  }

  if(!contact.empty()){
    contact.resize(12);
    std::copy(contact.begin(), contact.end(), &gCurrentUser.contactNumber[0]);
  }

  if(!password.empty()){
    password.resize(12);
    std::copy(password.begin(), password.end(), &gCurrentUser.password[0]);
  }

  if(!faculty.empty()){
    faculty.resize(10);
    std::copy(faculty.begin(), faculty.end(), &gCurrentUser.faculty[0]);
  }

  if(age != 0)
    gCurrentUser.age = age;
  
  if(!db_update_record(gCurrentUser))
    throw std::runtime_error("Unexpected Error: Cannot update user!");
}

void ui_view_vehicles(const User &user){
  for(;;){
    cli_clear();
    cli_header("Vehicles");

    auto vhs = db_find<Vehicle>({0, user.id});
    cli_field("Registered vehicles", vhs.size());

    if(!vhs.empty()){
      std::vector<std::vector<std::string>> values;
      for(const auto& v: vhs)
        values.push_back({v.plate, v.model});

      cli_table({"Plate", "Model"}, values);
    }
    else if(gCurrentUser.id == user.id)
      cli_separator(10);

    if(gCurrentUser.id != user.id) {
      cli_press_enter();
      return;
    }

    switch (cli_menu({"Register a Vehicle", "Delete a Vehicle", "Back"})) {
    case 0:
      ui_add_vehicle();
      break;
    case 1:
      ui_delete_vehicle(vhs);
      break;
    case 2:
      return;
    }
  }
}

void ui_add_vehicle() {
  cli_clear();
  cli_header("Register a new Vehicle");

  std::string plate = validator("License Plate (ex. AEB0194): ", [](const std::string& in){
    if(in.size() != 7 || !is_integer(in.substr(3, 6)))
      return "Invalid license plate number";
    return "";
  });

  std::hash<std::string> hasher;
  int id = static_cast<int>(hasher(plate)) + gCurrentUser.id;
  if(db_find_by_id<Vehicle>(id, nullptr)){
    cli_error("Vehicle already Exists!");
    cli_press_enter();
    return;
  }

  std::string model = validator("Model (company name.): ", [](const std::string& in){
    if(in.size() < 2 || in.size() > 8)
      return "Car model length should be 2 to 8 characters";
    return "";
  });

  
  Vehicle vh{id, gCurrentUser.id};
  std::copy(plate.begin(), plate.end(), &vh.plate[0]);
  std::copy(model.begin(), model.end(), &vh.model[0]);

  if(!db_add_record(vh))
    throw std::runtime_error("Unexpcted Error: Failed to add a new Vehicle");
}

void ui_delete_vehicle(const std::vector<Vehicle>& userVehicles){
  cli_clear();
  std::vector<std::string> plates(userVehicles.size());
  for(size_t i = 0; i < plates.size(); ++i)
    plates[i] = userVehicles[i].plate;

  auto index = cli_menu(plates);
  if(!cli_boolean("Confirm deletion of " + plates[index] + "? "))
    return;

  if(!db_delete_record<Vehicle>(userVehicles[index].id))
    throw std::runtime_error("Unexpected Error: Couldn't delete a vehicle");
}

void ui_view_passes(const User& user){
  for(;;){
    cli_clear();

    if(gCurrentUser.id == user.id)
      cli_header("My Parking Passes");
    else
     cli_header(user.fullname << "'s Parking Passes");

    auto passes = db_find<ParkingPass>({0, user.id});
    if(passes.empty()) {
      if(gCurrentUser.id == user.id){
        cli_text("You don't have any parking passes yet!");
        if(cli_boolean("Apply for a pass?")) 
          return;
      }
      else{
        cli_text(gCurrentUser.fullname << " doesn't have any passes");
        cli_press_enter();
      }
      return;
    }

    for(const auto& p : passes){
      if(p.status == ePassStatus::Active){
        cli_table({"ID", "Issue Date", "Expiry Date", "Duration", "Status"}, {{
          std::to_string(p.id), std::to_string(p.issueDate), std::to_string(p.issueDate), 
          enum_to_string(p.duration), enum_to_string(p.status)
        }});
        break;
      }
    }
  
    cli_separator(10);
    switch(cli_menu({"View History", "Back"})){
    case 0: {
      cli_clear();
      std::vector<std::vector<std::string>> values;
      for(const auto& p: passes){
        if(p.status == ePassStatus::Active) 
          continue;

        values.push_back({
          std::to_string(p.id), std::to_string(p.issueDate), 
          enum_to_string(p.duration), enum_to_string(p.status)
        });
      }

      cli_table({"ID", "Issue Date", "Duration", "Status"}, values);
      cli_press_enter();
      break;
    }
    case 1:
      return;
    }
  }
}

void ui_view_applications(const User& user){
  for(;;){
    cli_clear();

    if(user.id == gCurrentUser.id)
      cli_header("My Parking Pass Applications");
    else
     cli_header(user.fullname << "'s Parking Pass Applications");

    auto applications = db_find<ParkingApplication>({0, user.id});
    if(applications.empty()) {
      if(gCurrentUser.id == user.id)
        cli_text("You haven't made any application yet!");
      else
        cli_text(user.fullname << " hasn't made any applications");

      cli_press_enter();
      return;
    }

    for(const auto& app : applications){
      if(app.status == eApplicationStatus::PendingPayment || app.status == eApplicationStatus::WaitingForReview){
        cli_table({"Submission Date", "Duration", "Status"}, {{
          std::to_string(app.submissionDate), enum_to_string(app.duration), enum_to_string(app.status)
        }});
        break;
      }
    }
  
    cli_separator(10);
    switch(cli_menu({"View History", "Back"})){
    case 0:{
      cli_clear();
      std::vector<std::vector<std::string>> values;
      for(const auto& app: applications){
        values.push_back({
          std::to_string(app.id), std::to_string(app.submissionDate),
          std::to_string(app.closedDate), enum_to_string(app.duration), 
          enum_to_string(app.status)});
      }

      cli_table({"ID", "Submission Date", "Closed Date", "Duration", "Status"}, values);
      cli_press_enter();
      break;
    }
    case 1: 
      return;
    }
  }
}

