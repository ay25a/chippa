#include "ui_elements.hpp"
#include "cli_core.hpp"
#include "database.hpp"
#include <map>

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

static std::vector<User> ui_staff_list_students(){
  User filter{};
  filter.role = eUserRole::Student;
  return db_find(filter);
}

static bool ui_staff_choose_student(User &student){
  auto students = ui_staff_list_students();
  if(students.empty())
    return false;

  std::vector<std::string> choices;
  for(const auto &s : students)
    choices.push_back(std::to_string(s.id) + " - " + s.fullname);

  size_t index = cli_menu(choices);
  student = students[index];
  return true;
}

static std::vector<ParkingApplication> ui_staff_list_applications(){
  return db_find(ParkingApplication{});
}

static std::vector<ParkingPass> ui_staff_list_passes(){
  return db_find(ParkingPass{});
}

static void ui_staff_list_students_page(){
  cli_clear();
  cli_header("Registered Students");

  auto students = ui_staff_list_students();
  if(students.empty()){
    cli_text("No students were found.");
    cli_press_enter();
    return;
  }

  std::vector<std::vector<std::string>> rows;
  for(const auto &s : students){
    rows.push_back({
      std::to_string(s.id),
      s.fullname,
      s.faculty,
      s.status == eUserStatus::Active ? "Active" : "Suspended"
    });
  }

  cli_table({"ID", "Name", "Faculty", "Status"}, rows);
  cli_press_enter();
}

static void ui_staff_view_passes_page();
static void ui_staff_generate_reports_page();
static void ui_staff_view_applications_page();

static void ui_staff_dashboard(){
  auto students = ui_staff_list_students();
  auto applications = ui_staff_list_applications();
  auto passes = ui_staff_list_passes();

  int waitingForReview = 0;
  int completed = 0;
  int rejected = 0;
  int pendingPayment = 0;
  for(const auto &app : applications){
    switch(app.status){
      case eApplicationStatus::WaitingForReview: waitingForReview++; break;
      case eApplicationStatus::Completed: completed++; break;
      case eApplicationStatus::Rejected: rejected++; break;
      case eApplicationStatus::PendingPayment: pendingPayment++; break;
      default: break;
    }
  }

  int activePasses = 0;
  int expiredPasses = 0;
  for(const auto &pass : passes){
    if(pass.status == ePassStatus::Active)
      activePasses++;
    else if(pass.status == ePassStatus::Expired)
      expiredPasses++;
  }

  cli_header("Staff Dashboard");
  cli_field("Registered Students", static_cast<int>(students.size()));
  cli_field("Total Applications", static_cast<int>(applications.size()));
  cli_field("Pending review", waitingForReview);
  cli_field("Completed applications", completed);
  cli_field("Rejected applications", rejected);
  cli_field("Pending payments", pendingPayment);
  cli_field("Active passes", activePasses);
  cli_field("Expired passes", expiredPasses);
  if(waitingForReview > 0)
    cli_warning("There are " << waitingForReview << " applications waiting for review.");
  cli_separator(10);
}

void ui_student_menu() {
  cli_clear();
  cli_header("Welcome " << gCurrentUser.fullname << '!');

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
      ui_student_view_applications();
      break;
    case 4:
      exit(0);
  }
}

void ui_staff_menu(){
  cli_clear();
  ui_staff_dashboard();
  cli_header("Welcome " << gCurrentUser.fullname << '!');

  switch(cli_menu({
    "View Students",
    "View Student Profile",
    "View Student Vehicles",
    "View Applications",
    "View Parking Passes",
    "Generate Reports",
    "My Profile",
    "Exit"
  })){
    case 0:
      ui_staff_list_students_page();
      break;
    case 1:{
      User student{};
      if(!ui_staff_choose_student(student)){
        cli_text("No students are available.");
        cli_press_enter();
      } else {
        ui_staff_view_profile(student);
      }
      break;
    }
    case 2:{
      User student{};
      if(!ui_staff_choose_student(student)){
        cli_text("No students are available.");
        cli_press_enter();
      } else {
        cli_clear();
        cli_header(student.fullname << "'s Vehicles");
        ui_view_vehicles_core(student);
        cli_press_enter();
      }
      break;
    }
    case 3:
      ui_staff_view_applications_page();
      break;
    case 4:
      ui_staff_view_passes_page();
      break;
    case 5:
      ui_staff_generate_reports_page();
      break;
    case 6:
      ui_staff_view_profile(gCurrentUser);
      break;
    case 7:
      exit(0);
  }
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

void ui_staff_view_profile(const User& user){
  for(;;){
    ui_view_profile_core(user);
    auto vehicles = db_find<Vehicle>({0, user.id});
    cli_field("Registered vehicles", vehicles.size());
    cli_separator(10);
    switch(cli_menu({"View Vehicles", "Back"})){
      case 0:
        cli_clear();
        cli_header(user.fullname << "'s Vehicles");
        ui_view_vehicles_core(user);
        cli_press_enter();
        break;
      case 1:
        return;
    }
  }
}

static void ui_staff_review_application(ParkingApplication app){
  User applicant{};
  if(!db_find_by_id<User>(app.userid, &applicant))
    std::strcpy(applicant.fullname, "Unknown");

  for(;;){
    cli_clear();
    cli_header("Application #" << app.id);
    cli_field("Student", applicant.fullname);
    cli_field("Submitted", date_to_string(app.submissionDate));
    cli_field("Duration", eto_string(app.duration));
    cli_field("Status", eto_string(app.status));
    cli_field("Old Pass ID", app.oldPassID);
    cli_field("New Pass ID", app.newPassID);
    cli_separator(10);

    if(app.status != eApplicationStatus::WaitingForReview){
      cli_text("This application cannot be approved or rejected because it is not waiting for review.");
      cli_press_enter();
      return;
    }

    switch(cli_menu({"Approve", "Reject", "Back"})){
      case 0:{
        if(!cli_boolean("Approve this application?"))
          break;

        if(app.oldPassID != 0){
          ParkingPass oldPass{};
          if(db_find_by_id<ParkingPass>(app.oldPassID, &oldPass) && oldPass.status == ePassStatus::Active){
            oldPass.status = ePassStatus::Expired;
            db_update_record(oldPass);
          }
        }

        ParkingPass pass{};
        pass.id = db_get_next_id<ParkingPass>();
        pass.userid = app.userid;
        pass.appid = app.id;
        pass.issueDate = date_to_int();
        pass.duration = app.duration;
        pass.status = ePassStatus::Active;

        EXPECT(db_add_record(pass));

        app.newPassID = pass.id;
        app.status = eApplicationStatus::Completed;
        app.closedDate = date_to_int();
        EXPECT(db_update_record(app));

        cli_success("Application approved and a new parking pass was generated.");
        cli_press_enter();
        return;
      }
      case 1:{
        if(!cli_boolean("Reject this application?"))
          break;

        app.status = eApplicationStatus::Rejected;
        app.closedDate = date_to_int();
        EXPECT(db_update_record(app));

        cli_success("Application rejected.");
        cli_press_enter();
        return;
      }
      case 2:
        return;
    }
  }
}

static void ui_staff_view_applications_page(){
  for(;;){
    cli_clear();
    cli_header("Parking Applications");

    auto applications = ui_staff_list_applications();
    if(applications.empty()){
      cli_text("No applications found.");
      cli_press_enter();
      return;
    }

    std::vector<std::vector<std::string>> rows;
    std::vector<std::string> options;
    for(const auto &app : applications){
      rows.push_back({
        std::to_string(app.id),
        std::to_string(app.userid),
        eto_string(app.duration),
        eto_string(app.status),
        date_to_string(app.submissionDate)
      });
      options.push_back(std::to_string(app.id) + " - " + std::to_string(app.userid) + " [" + eto_string(app.status) + "]");
    }

    cli_table({"ID", "Student ID", "Duration", "Status", "Submitted"}, rows);
    cli_separator(10);

    switch(cli_menu({"Review Application", "Back"})){
      case 0:{
        size_t index = cli_menu(options);
        ui_staff_review_application(applications[index]);
        break;
      }
      case 1:
        return;
    }
  }
}

void ui_staff_view_vehicles(){
  User student{};
  if(!ui_staff_choose_student(student)){
    cli_text("No students are available.");
    cli_press_enter();
    return;
  }

  cli_clear();
  cli_header(student.fullname << "'s Vehicles");
  ui_view_vehicles_core(student);
  cli_press_enter();
}

static void ui_staff_view_passes_page(){
  for(;;){
    cli_clear();
    cli_header("Parking Passes");

    switch(cli_menu({"View All Passes", "View Passes By Student", "Back"})){
      case 0:{
        auto passes = ui_staff_list_passes();
        if(passes.empty()){
          cli_text("No parking passes found.");
        } else {
          std::vector<std::vector<std::string>> rows;
          for(const auto &p : passes){
            rows.push_back({
              std::to_string(p.id),
              std::to_string(p.userid),
              date_to_string(p.issueDate),
              eto_string(p.duration),
              eto_string(p.status)
            });
          }
          cli_table({"Pass ID", "Student ID", "Issue Date", "Duration", "Status"}, rows);
        }
        cli_press_enter();
        break;
      }
      case 1:{
        User student{};
        if(!ui_staff_choose_student(student)){
          cli_text("No students are available.");
          cli_press_enter();
          break;
        }
        auto passes = db_find<ParkingPass>({0, student.id});
        if(passes.empty()){
          cli_text("This student has no parking passes.");
        } else {
          std::vector<std::vector<std::string>> rows;
          for(const auto &p : passes){
            rows.push_back({
              std::to_string(p.id),
              date_to_string(p.issueDate),
              eto_string(p.duration),
              eto_string(p.status)
            });
          }
          cli_table({"Pass ID", "Issue Date", "Duration", "Status"}, rows);
        }
        cli_press_enter();
        break;
      }
      case 2:
        return;
    }
  }
}

static void ui_staff_generate_reports_page(){
  cli_clear();
  cli_header("Staff Reports");

  auto students = ui_staff_list_students();
  auto applications = ui_staff_list_applications();
  auto passes = ui_staff_list_passes();

  int totalStudents = static_cast<int>(students.size());
  int waiting = 0;
  int completed = 0;
  int rejected = 0;
  int pendingPayment = 0;
  for(const auto &app : applications){
    switch(app.status){
      case eApplicationStatus::WaitingForReview: waiting++; break;
      case eApplicationStatus::Completed: completed++; break;
      case eApplicationStatus::Rejected: rejected++; break;
      case eApplicationStatus::PendingPayment: pendingPayment++; break;
      default: break;
    }
  }

  int activePasses = 0;
  int expiredPasses = 0;
  for(const auto &pass : passes){
    if(pass.status == ePassStatus::Active)
      activePasses++;
    else if(pass.status == ePassStatus::Expired)
      expiredPasses++;
  }

  std::map<std::string, int> facultyCounts;
  for(const auto &s : students)
    facultyCounts[std::string(s.faculty)]++;

  cli_field("Total Students", totalStudents);
  cli_field("Total Applications", static_cast<int>(applications.size()));
  cli_field("Waiting for Review", waiting);
  cli_field("Completed Applications", completed);
  cli_field("Rejected Applications", rejected);
  cli_field("Pending Payments", pendingPayment);
  cli_field("Active Passes", activePasses);
  cli_field("Expired Passes", expiredPasses);
  cli_separator(10);

  cli_text("Students by Faculty:");
  if(facultyCounts.empty())
    cli_text("  No student records available.");
  else
    for(const auto &entry : facultyCounts)
      cli_field("  " + entry.first, entry.second);

  cli_press_enter();
}

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

void ui_student_view_vehicles(){
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

// **************************************
// Applications
// **************************************

static void ui_student_view_application_details(const ParkingApplication &app){
  cli_clear();
  cli_header("Application " << app.id);
  cli_field("Student ID", app.userid);
  cli_field("Submission Date", date_to_string(app.submissionDate));
  {
    std::string closedDate = app.closedDate == 0 ? "Ongoing" : date_to_string(app.closedDate);
    cli_field("Closed Date", closedDate);
  }
  cli_field("Duration", eto_string(app.duration));
  cli_field("Status", eto_string(app.status));
  cli_field("Old Pass ID", app.oldPassID);
  cli_field("New Pass ID", app.newPassID);
  cli_press_enter();
}

void ui_student_view_applications(){
  for(;;){
    cli_clear();
    cli_header("My Parking Pass Applications");

    ParkingApplication filter{};
    filter.userid = gCurrentUser.id;
    auto applications = db_find<ParkingApplication>(filter);

    if(applications.empty()){
      cli_text("You don't have any applications yet.");
      if(cli_boolean("Submit a new parking pass application?"))
        ui_new_application();
      return;
    }

    std::vector<std::vector<std::string>> rows;
    std::vector<std::string> options;
    for(const auto &app : applications){
      rows.push_back({
        std::to_string(app.id),
        date_to_string(app.submissionDate),
        app.closedDate == 0 ? "Ongoing" : date_to_string(app.closedDate),
        eto_string(app.duration),
        eto_string(app.status)
      });
      options.push_back(std::to_string(app.id) + " - " + eto_string(app.status));
    }

    cli_table({"ID", "Submitted", "Closed", "Duration", "Status"}, rows);
    cli_separator(10);
    switch(cli_menu({"View Details", "Submit an Application", "Back"})){
      case 0:{
        size_t index = cli_menu(options);
        ui_student_view_application_details(applications[index]);
        break;
      }
      case 1:
        ui_new_application();
        break;
      case 2:
        return;
    }
  }
}

// **************************************
// Passes
// **************************************

static void ui_view_passes_history(const std::vector<ParkingPass> &passes){
  if(passes.empty()){
    cli_text("No pass history available.");
    return;
  }

  std::vector<std::vector<std::string>> rows;
  for(const auto &p : passes){
    rows.push_back({
      std::to_string(p.id),
      date_to_string(p.issueDate),
      eto_string(p.duration),
      eto_string(p.status)
    });
  }

  cli_table({"Pass ID", "Issue Date", "Duration", "Status"}, rows);
}

void ui_view_passes_core(const User &user){
  auto passes = db_find<ParkingPass>({0, user.id});
  if(passes.empty()){
    cli_text("No parking pass records found.");
    return;
  }

  std::vector<std::vector<std::string>> rows;
  for(const auto &p : passes){
    rows.push_back({
      std::to_string(p.id),
      date_to_string(p.issueDate),
      eto_string(p.duration),
      eto_string(p.status)
    });
  }

  cli_table({"Pass ID", "Issue Date", "Duration", "Status"}, rows);
}

void ui_new_application(){
  cli_clear();
  cli_header("New Application");

  cli_subheader("Requested Pass Duration");
  ePassDuration duration = static_cast<ePassDuration>(cli_menu({"1 Month", "2 Months", "3 Months"}) + 1);

  if(!cli_boolean("The application will be sent for review by the staff. Confirm? "))
    return;

  auto passes = db_find<ParkingPass>({0, gCurrentUser.id});

  ParkingApplication app{};
  app.id = db_get_next_id<ParkingApplication>();
  app.userid = gCurrentUser.id;
  if(!passes.empty())
    app.oldPassID = passes[0].id;
  app.submissionDate = date_to_int();
  app.closedDate = 0;
  app.duration = duration;
  app.status = eApplicationStatus::WaitingForReview;

  if(!db_add_record(app))
    throw std::runtime_error("Unexpected Failure: Cannot add a new record!");

  cli_success("Application sent for review.");
  cli_press_enter();
}

void ui_view_passes(const User& user){
  for(;;){
    cli_clear();
    if(gCurrentUser.id == user.id)
      cli_header("My Parking Passes");
    else
      cli_header(user.fullname + std::string("'s Parking Passes"));

    auto passes = db_find<ParkingPass>({0, user.id});
    if(passes.empty()){
      if(gCurrentUser.id == user.id){
        cli_text("You don't have any parking passes yet.");
        if(cli_boolean("Apply for a pass?"))
          ui_new_application();
      } else {
        cli_text(std::string(user.fullname) + " has no parking passes.");
        cli_press_enter();
      }
      return;
    }

    bool hasActive = false;
    for(const auto &p : passes){
      if(p.status == ePassStatus::Active){
        cli_table({"Pass ID", "Issue Date", "Duration", "Status"}, {{
          std::to_string(p.id),
          date_to_string(p.issueDate),
          eto_string(p.duration),
          eto_string(p.status)
        }});
        hasActive = true;
        break;
      }
    }

    if(!hasActive)
      cli_text("No active parking pass found.");

    cli_separator(10);
    if(gCurrentUser.id == user.id){
      switch(cli_menu({"View Pass History", "Apply for a pass", "Back"})){
        case 0:
          ui_view_passes_history(passes);
          cli_press_enter();
          break;
        case 1:
          ui_new_application();
          break;
        case 2:
          return;
      }
    } else {
      switch(cli_menu({"View Pass History", "Back"})){
        case 0:
          ui_view_passes_history(passes);
          cli_press_enter();
          break;
        case 1:
          return;
      }
    }
  }
}
