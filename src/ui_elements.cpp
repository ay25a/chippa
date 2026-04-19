#include "ui_elements.hpp"
#include "cli_core.hpp"
#include "database.hpp"

// **************************************
// Authentication Related
// **************************************

bool ui_login(){
  cli_clear();
  cli_header("Login to your Account");
  
  int userid = std::stoi(get_valid_input("User ID: ", userid_validator));
  std::string password = cli_input("Password: ");

  User user{};
  bool found = db_find_by_id<User>(userid, &user);
  if(!found || strcmp(user.password, password.c_str()) != 0){
    cli_error("Incorrect email or password!");
    cli_press_enter();
    return false;
  }

  gCurrentUser = user;
  return true;
}

bool ui_register(){
  cli_clear();
  cli_header("Create a User");

  int userid = std::stoi(get_valid_input("User ID (Organization ID): ", userid_validator));
  if(db_find_by_id<User>(userid, nullptr)){
    cli_error("User already exists! Try logging in...");
    cli_press_enter();
    return false;
  }

  std::string password = get_valid_input("Password: ", password_validator);
  std::string contact = get_valid_input("Contact Numer: ", contact_validator);
  std::string name = get_valid_input("Full Name: ", name_validator);
  int age = std::stoi(get_valid_input("Age: ", age_validator));

  cli_subheader("Faculty");
  auto choice = cli_menu({std::begin(C_FACULTIES), std::end(C_FACULTIES)});
  const std::string& faculty = C_FACULTIES[choice];

  cli_subheader("User Type");
  auto role = cli_menu({"Student", "Staff"}) + 1;

  // Fill the user struct
  User user{};
  user.id = userid;
  std::copy(password.begin(), password.end(), &user.password[0]);
  std::copy(contact.begin(), contact.end(), &user.contactNumber[0]);
  std::copy(name.begin(), name.end(), &user.fullname[0]);
  std::copy(faculty.begin(), faculty.end(), &user.faculty[0]);
  user.age = age;
  user.status = eUserStatus::Active;
  user.role = static_cast<eUserRole>(role);

  EXPECT(db_add_record(user));
  gCurrentUser = user;
  return true;
}

// **************************************
// Main Menu
// **************************************

static void InitStudent(){
  ParkingPass filter{};
  filter.status = ePassStatus::Active;
  auto active_pass = db_find(filter);
  
  if(active_pass.size() == 0)
    return;

  auto days = get_days_betwen(active_pass[0].issueDate, date_to_int());
  if(days <= 7)
    cli_warning("Your pass is about to expire in " << days << " Days!");
}

void ui_student_menu() {
  cli_clear();
  cli_header("Welocme " << gCurrentUser.fullname << '!');

  InitStudent();
  switch (cli_menu({"My Profile", "My Vehicles", "My Passes", "My Applications", "Exit"})) {
    case 0:
      ui_student_view_profile();
      break;
    case 1:
      ui_student_view_vehicles();
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

void ui_staff_menu(){
  cli_clear();
  cli_header("Welocme " << gCurrentUser.fullname << '!');
}

// **************************************
// Profile
// **************************************

// Shared

void ui_view_profile_core(const User& user){
  cli_clear();
  cli_header("Profile");
  cli_field((user.role == eUserRole::Student ? "Student " : "Staff ") << "ID", user.id);
  cli_field("Name", user.fullname);
  cli_field("Age", user.age);
  cli_field("Contact Number", user.contactNumber);
  cli_field("Faculty", user.faculty);
  cli_field("Status", (user.status == eUserStatus::Suspended ? "Suspended" : "Active"));
  cli_field("Role", (user.role == eUserRole::Student ? "Student" : "Staff"));
}

void ui_edit_current_profile(){
  cli_clear();
  cli_header("Edit Profile");
  
  switch (cli_menu({"Name", "Age", "Contact Number", "Password", "Faculty", "Back"})) {
  case 0:{
    std::string name = get_valid_input("New Name: ", name_validator);
    name.resize(32);

    std::copy(name.begin(), name.end(), &gCurrentUser.fullname[0]);
    break;
  }
  case 1:{
    int age = std::stoi(get_valid_input("New Age: ", age_validator));

    gCurrentUser.age = age;
    break;
  }
  case 2: {
    std::string contact = get_valid_input("New Contact Number: ", contact_validator);
    contact.resize(12);
    
    std::copy(contact.begin(), contact.end(), &gCurrentUser.contactNumber[0]);
    break;
  }
  case 3:{
    std::string password = get_valid_input("New Password: ", password_validator);
    password.resize(12);

    std::copy(password.begin(), password.end(), &gCurrentUser.password[0]);
    break;
  }
  case 4:{
    std::string faculty = C_FACULTIES[cli_menu({std::begin(C_FACULTIES), std::end(C_FACULTIES)})];
    faculty.resize(10);

    std::copy(faculty.begin(), faculty.end(), &gCurrentUser.faculty[0]);
    break;
  }
  case 5:
    return;
  }

  if(!cli_boolean("Confirm changes?"))
    return;

  EXPECT(db_update_record(gCurrentUser));
}

// Role

void ui_student_view_profile(){
  for(;;){
    ui_view_profile_core(gCurrentUser);
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

void ui_staff_view_profile(const User& user){}


// **************************************
// Vehicles
// **************************************

// Shared

void ui_view_vehicles_core(const User& user){
  auto vhs = db_find<Vehicle>({0, user.id});
  cli_field("Registered vehicles", vhs.size());

  if(!vhs.empty()){
    std::vector<std::vector<std::string>> values;
    for(const auto& v: vhs)
      values.push_back({v.plate, v.model});

    cli_table({"Plate", "Model"}, values);
  }
}

// Student

void ui_student_view_vehicles(const User &user){
  for(;;){
    cli_clear();
    cli_header("My Vehicles");

    ui_view_vehicles_core(gCurrentUser);
    cli_separator(10);
    switch (cli_menu({"Register a Vehicle", "Delete a Vehicle", "Back"})) {
    case 0:
      ui_add_vehicle();
      break;
    case 1:
      ui_delete_vehicle();
      break;
    case 2:
      return;
    }
  }
}

void ui_add_vehicle() {
  cli_clear();
  cli_header("Register a new Vehicle");

  std::string plate = get_valid_input("License Plate (ex. AEB0194): ", [](const std::string& in){
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

  std::string model = get_valid_input("Model (company name.): ", [](const std::string& in){
    if(in.size() < 2 || in.size() > 8)
      return "Car model length should be 2 to 8 characters";
    return "";
  });

  
  Vehicle vh{id, gCurrentUser.id};
  std::copy(plate.begin(), plate.end(), &vh.plate[0]);
  std::copy(model.begin(), model.end(), &vh.model[0]);

  EXPECT(db_add_record(vh));
}

void ui_delete_vehicle(){
  cli_clear();
  Vehicle filter{};
  filter.userid = gCurrentUser.id;

  auto vehciles = db_find(filter);
  if(vehciles.empty()){
    cli_text("Nothing here to delete...");
    cli_press_enter();
    return;
  }

  std::vector<std::string> plates(vehciles.size());
  for(size_t i = 0; i < plates.size(); ++i)
    plates[i] = vehciles[i].plate;

  auto index = cli_menu(plates);
  if(!cli_boolean("Confirm deletion of " + plates[index] + "? "))
    return;

  EXPECT(db_delete_record<Vehicle>(vehciles[index].id));
}

// Staff

void ui_staff_view_vehicles(){}


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
          ui_new_application();
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
          enum_to_string(p.duration), status(p.status)
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
          enum_to_string(p.duration), status(p.status)
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

void ui_new_application(){
  cli_clear();
  cli_header("New Application");

  cli_subheader("Requested Pass Duration");
  ePassDuration druation = static_cast<ePassDuration>(cli_menu({"1 Month", "2 Months", "3 Months"}));

  if(!cli_boolean("The application will be sent for review by the staff. Confirm? "))
    return;

  auto passes = db_find(ParkingPass{0, gCurrentUser.id, 0, 0, ePassDuration::Unknown, ePassStatus::Active});
  
  ParkingApplication app{};
  app.id = db_get_next_id<ParkingApplication>();
  app.userid = gCurrentUser.id;
  if(passes.size() != 0)
    app.oldPassID = passes[0].id;
  app.submissionDate = current_date_to_int();
  app.closedDate = 0;
  app.duration = druation;
  app.status = eApplicationStatus::WaitingForReview;
  
  if(!db_add_record(app))
    throw std::runtime_error("Unexpected Failure: Cannot add a new record!");

  cli_text("Application Sent!");
  cli_press_enter();
}