#include "UI.hpp"
#include <map>
#include <set>
#include <algorithm>
#include <cstring>

using std::string;
using std::vector;

// **************************************
// Authentication Related Pages
// **************************************

void LoginPage() {
  User user{};
  const auto login = [&user](int id, string &password) {
    bool found = db_find_by_id<User>(id, &user);
    return found && strcmp(user.password, password.c_str()) == 0;
  };

  cli_clear();
  cli_header("Login to your Account");

  int userid = std::stoi(cli_valid_input("User ID: ", utils::userid_validator));
  string password = cli_input("Password: ");

  if (login(userid, password))
    gCurrentUser = user;
  else
    cli_error("Incorrect email or password!");
}

void RegisterPage() {
  const auto exists = [](int id) { return db_find_by_id<User>(id, nullptr); };
  User user{};
  user.status = eUserStatus::Active;

  cli_clear();
  cli_header("Create a User");

  user.id = std::stoi(cli_valid_input("Organization ID: ", utils::userid_validator));
  if (exists(user.id)) {
    cli_error("User already exists! Try logging in...");
    return;
  }

  string password = cli_valid_input("Password: ", utils::password_validator);
  std::copy(password.begin(), password.end(), &user.password[0]);

  string contact = cli_valid_input("Contact Number: ", utils::contact_validator);
  std::copy(contact.begin(), contact.end(), &user.contactNumber[0]);

  string name = cli_valid_input("Full Name: ", utils::name_validator);
  std::copy(name.begin(), name.end(), &user.name[0]);

  user.age = std::stoi(cli_valid_input("Age: ", utils::age_validator));

  cli_subheader("Faculty");
  auto choice = cli_menu({std::begin(C_FACULTIES), std::end(C_FACULTIES)});
  const string &faculty = C_FACULTIES[choice];
  std::copy(faculty.begin(), faculty.end(), &user.faculty[0]);

  cli_subheader("User Type");
  user.role = static_cast<eUserRole>(cli_menu({"Student", "Staff"}));

  EXPECT(db_add_record(user));
  gCurrentUser = user;
}

ePageState AuthenticationPage() {
  const vector<string> menu { "Login", "Create Account", "Exit" };

  cli_clear();
  cli_header("Login or Create a new Account!");
  switch (cli_menu(menu)) {
  case 0:
    LoginPage();
    break;
  case 1:
    RegisterPage();
    break;
  default:
    exit(0);
  }

  return gCurrentUser.id == 0 ? ePageState::Continue : ePageState::Exit;
}

// **************************************
// Application Related Pages
// **************************************

ePageState ApplicationsPage(const User *user) {
  const vector<string> menu { "View Application", "History", "Back"};
  cli_clear();
  cli_header("Applications");

  auto apps = db_find<ParkingApplication>([&user](const ParkingApplication& e){ 
    return !user || user->id == e.userid;
  });

  if (apps.size() == 0) {
    cli_error("No Applications found!");
    return ePageState::Exit;
  }

  auto active = db_find<ParkingApplication>([&user](const ParkingApplication& e){
    return (!user || user->id == e.userid) && e.closedDate == 0 && 
    (is_staff() ? e.status == eApplicationStatus::WaitingForReview : true);
  });

  // View Active Applications
  {
    vector<std::array<string, 5>> values;
    for (const auto &app : active) {
      values.push_back({
          std::to_string(app.id),
          std::to_string(app.userid),
          date::to_string(app.submissionDate),
          std::to_string(app.duration) + " Days",
          utils::to_string(app.status),
      });
    }

    cli_table<5>({"ID", "User ID", "Submission Date", "Requested Duration", "Status"}, values);
  }

  switch (cli_menu(menu)) {
  case 0: {
    if (active.size() == 0) {
      cli_error("No Applications to visit!");
      return ePageState::Continue;
    }

    vector<string> choices;
    for (const auto &app : active)
      choices.emplace_back(std::to_string(app.id));

    ApplicationDetailsPage(active[cli_menu(choices)]);
    return ePageState::Continue;
  }
  case 1:
    ApplicationsHistoryPage(apps);
    return ePageState::Continue;
  }

  return ePageState::Exit;
}

void ApplicationDetailsPage(ParkingApplication app) {
  vector<string> menu = { "Back" };
  if (is_staff() && app.status == eApplicationStatus::WaitingForReview)
    menu = { "Approve", "Back" };
  else if (is_student() && app.status == eApplicationStatus::PendingPayment)
    menu = {"Pay", "Back"};

  cli_clear();
  cli_header("Application " << app.id);
  cli_field("Student ID", app.userid);
  cli_field("Submission Date", date::to_string(app.submissionDate));
  cli_field("Duration", app.duration << " Days");
  cli_field("Status", utils::to_string(app.status));

  const auto approve = [&app](){
    app.status = eApplicationStatus::PendingPayment;
    EXPECT(db_update_record(app));
    cli_success("Application Approved!");
  };

  const auto pay = [&app](){
    ParkingPass pass {};
    pass.id = db_get_next_id<ParkingPass>();
    pass.userid = gCurrentUser.id;
    pass.appid = app.id;
    pass.issueDate = date::current();
    pass.duration = app.duration;
    pass.status = ePassStatus::Active;
    EXPECT(db_add_record(pass));
    
    app.status = eApplicationStatus::Completed;
    app.newPassID = pass.id;
    app.closedDate = date::current();
    EXPECT(db_update_record(app));
    
    cli_success("A new pass has been issued successfully!");
  };

  if(cli_menu(menu) == 0){
    if(menu.size() == 1)
      return;
    else if(is_staff())
      approve();
    else if(cli_boolean("Confirm payment?"))
      pay();
  }
}

void ApplicationsHistoryPage(const vector<ParkingApplication> &apps) {
  cli_clear();
  cli_header("Applications History");

  vector<std::array<string, 4>> values;
  for (const auto &app : apps)
    values.push_back({
        std::to_string(app.id),
        date::to_string(app.submissionDate),
        app.closedDate == 0 ? "Ongoing" : date::to_string(app.closedDate),
        utils::to_string(app.status),
    });
  

  cli_table<4>({"ID", "Submission Date", "Closed Date", "Status"}, values);
  cli_press_enter();
}

ePageState PassesPage(const User *user) {
  const vector<string> menu { "History", "Exit" };
  cli_clear();
  cli_header("Passes");

  auto activePasses = db_find<ParkingPass>([&user](const ParkingPass& p){
    return p.status == ePassStatus::Active && (!user ? true : user->id == p.userid);
  });

  if (is_student() && activePasses.empty()) {
    auto activeApps = db_find<ParkingApplication>([](const ParkingApplication& e){
      return gCurrentUser.id == e.userid && e.closedDate == 0;
    });

    if (activeApps.empty() && cli_boolean("Apply for a new Pass? ")) {
      NewApplicationPage();
      return ePageState::Continue;
    }
  } 

  {
    vector<std::array<string, 5>> values;
    for (const auto &p : activePasses) 
      values.push_back({
        std::to_string(p.id),
        std::to_string(p.userid),
        date::to_string(p.issueDate),
        std::to_string(date::days_until(p.issueDate, p.duration)) + " Days",
        std::to_string(p.duration) + " Days",
      });

    cli_table<5>({"ID", "User ID", "Issue Date", "Remaining", "Duration"}, values);
  }

  if(cli_menu(menu) == 0){
    PassHistoryPage(user);
    return ePageState::Continue;
  }

  return ePageState::Exit;
}

void NewApplicationPage() {
  cli_clear();
  cli_header("New Pass Application");

  cli_subheader("Requested Duration");
  uint16_t dur = C_PASS_DURATION[cli_menu({"1 Month", "2 Months", "3 Months"})];

  if (!cli_boolean("Confirm submitting the application for staff to review?"))
    return;

  ParkingApplication app{};
  app.id = db_get_next_id<ParkingApplication>();
  app.userid = gCurrentUser.id;
  app.duration = dur;
  app.submissionDate = date::current();
  app.closedDate = 0;
  app.newPassID = 0;
  app.status = eApplicationStatus::WaitingForReview;

  EXPECT(db_add_record(app));
  cli_success("Application submitted successfully");
}

void PassHistoryPage(const User *user) {
  cli_clear();
  cli_header("Pass History");

  auto passes = db_find<ParkingPass>([&user](const ParkingPass& p){
    return !user || user->id == p.userid;
  });

  vector<std::array<string, 5>> values;
  for (const auto &p : passes) {
    values.push_back({
        std::to_string(p.id),
        std::to_string(p.userid),
        date::to_string(p.issueDate),
        std::to_string(p.duration) + " Days",
        utils::to_string(p.status),
    });
  }

  cli_table<5>({"ID", "User ID", "Issue Date", "Duration", "Status"}, values);
  cli_press_enter();
}

ePageState VehiclesPage() {
  const vector<string> menu { "Add", "Remove", "Back" };
  cli_clear();
  cli_header("My Vehicles");

  auto vhs = db_find<Vehicle>([](const Vehicle& v){
    return v.userid == gCurrentUser.id;
  });
  cli_field("Registered vehicles", vhs.size());

  vector<std::array<string, 2>> values;
  for (const auto &v : vhs)
    values.push_back({v.plate, v.model});
  cli_table<2>({"Plate", "Model"}, values);
  
  switch (cli_menu(menu)) {
  case 0:
    NewVehiclePage();
    return ePageState::Continue;
  case 1:
    RemoveVehiclePage(vhs);
    return ePageState::Continue;
  }

  return ePageState::Exit;
}

void NewVehiclePage() {
  cli_clear();
  cli_header("Add Vehicle");

  string plate = cli_valid_input("License Plate (ex. AEB019): ", [](const string &in) {
        if (in.size() != 6 || !utils::is_integer(in.substr(3, 5)))
          return "Invalid license plate number";
        return "";
      });

  std::hash<string> hasher;
  int id = static_cast<int>(hasher(plate)) + gCurrentUser.id;
  if (db_find_by_id<Vehicle>(id, nullptr)) {
    cli_error("Vehicle already Exists!");
    cli_press_enter();
    return;
  }

  string model = cli_valid_input("Model (company name.): ", [](const string &in) {
        if (in.size() < 2 || in.size() > 8)
          return "Car model length should be 2 to 8 characters";
        return "";
      });

  Vehicle vh{id, gCurrentUser.id};
  std::copy(plate.begin(), plate.end(), &vh.plate[0]);
  std::copy(model.begin(), model.end(), &vh.model[0]);

  EXPECT(db_add_record(vh));
}

void RemoveVehiclePage(const vector<Vehicle> &vhs) {
  if (vhs.empty()) {
    cli_error("Nothing here to delete!");
    return;
  }

  vector<string> plates(vhs.size());
  for (size_t i = 0; i < vhs.size(); ++i)
    plates[i] = vhs[i].plate;

  int choice = cli_menu(plates);
  if (cli_boolean("Confirm deletion of " + plates[choice] + "? "))
    EXPECT(db_delete_record<Vehicle>(vhs[choice].id));
}

static void ChangeUserStatus(User user, eUserStatus status) {
  if (!cli_boolean("Change student status to " + std::string(utils::to_string(status)) + "? "))
    return;

  user.status = status;
  EXPECT(db_update_record(user));
}

ePageState ProfilePage(const User& user) {
  vector<string> menu { "Edit", "Back" };
  if (is_staff() && user.id != gCurrentUser.id) {
    if (user.status == eUserStatus::Active)
      menu = {"Suspend", "Exit"};
    else
      menu = {"Activate", "Exit"};
  }

  cli_clear();
  cli_header("Profile");
  cli_field(utils::to_string(user.role) << " ID", user.id);
  cli_field("Name", user.name);
  cli_field("Age", user.age);
  cli_field("Contact Number", user.contactNumber);
  cli_field("Faculty", user.faculty);
  cli_field("Status", utils::to_string(user.status));

  if(cli_menu(menu) == 0){
    if(gCurrentUser.id == user.id)
      EditProfilePage();
    else
      ChangeUserStatus(user, user.status == eUserStatus::Active ? eUserStatus::Suspended : eUserStatus::Active);

    return ePageState::Continue;
  }

  return ePageState::Exit;
}

void EditProfilePage() {
  const vector<string> menu{"Name",     "Age",     "Contact Number",
                            "Password", "Faculty", "Back"};
  User newUser = gCurrentUser;
  const auto copy = [](string &src, char *dest, int size) {
    src.resize(size);
    std::copy(src.begin(), src.end(), dest);
  };

  cli_clear();
  cli_header("Edit Profile");

  switch (cli_menu(menu)) {
  case 0: {
    string name = cli_valid_input("New Name: ", utils::name_validator);
    copy(name, &newUser.name[0], 32);
    break;
  }
  case 1: {
    int age = std::stoi(cli_valid_input("New Age: ", utils::age_validator));
    newUser.age = age;
    break;
  }
  case 2: {
    string contact = cli_valid_input("New Contact Number: ", utils::contact_validator);
    copy(contact, &newUser.contactNumber[0], 12);
    break;
  }
  case 3: {
    string password = cli_valid_input("New Password: ", utils::password_validator);
    copy(password, &newUser.password[0], 12);
    break;
  }
  case 4: {
    string faculty =
        C_FACULTIES[cli_menu({std::begin(C_FACULTIES), std::end(C_FACULTIES)})];
    copy(faculty, &newUser.faculty[0], 10);
    break;
  }
  case 5:
    return;
  }

  if (!cli_boolean("Confirm changes?"))
    return;

  gCurrentUser = newUser;
  EXPECT(db_update_record(gCurrentUser));

  cli_success("User Information Updated!");
}

ePageState StudentsPage() {
  const vector<string> menu{"Go To Profile", "Exit"};

  const auto students = db_find<User>([](const User& u){
    return u.role == eUserRole::Student;
  });

  if (students.empty()) {
    cli_warning("The system doesn't have any students yet!");
    cli_press_enter();
    return ePageState::Exit;
  }

  cli_clear();
  cli_header("Students");

  vector<std::array<string, 4>> values;
  for (const auto &s : students) {
    values.push_back({
        std::to_string(s.id),
        s.name,
        s.faculty,
        utils::to_string(s.status),
    });
  }

  cli_table<4>({"ID", "Name", "Faculty", "Status"}, values);
  cli_separator(20);

  if (cli_menu(menu) == 0) {
    vector<string> choices;
    for (const auto &s : students)
      choices.emplace_back(std::to_string(s.id));

    return ProfilePage(students[cli_menu(choices)]);
  }

  return ePageState::Exit;
}

enum class eReportType { Total = 0, Annual, Monthly };
static void GetReportFilters(eReportType& type, std::string& faculty){
  cli_clear();
  cli_subheader("Select Report Type");
  type = static_cast<eReportType>(cli_menu({"Total Report", "Annual Report", "Monthly Report"}));

  vector<string> facultyMenu{C_FACULTIES.begin(), C_FACULTIES.end()};
  facultyMenu.emplace_back("All Faculties");

  cli_subheader("Filter by Faculty");
  int choice = cli_menu(facultyMenu);

  if(choice < C_FACULTIES.size())
    faculty = C_FACULTIES[choice];
}

void GenerateReport() {
  // Get Filters
  eReportType repType = eReportType::Total;
  std::string faculty;
  GetReportFilters(repType, faculty);

  int now = date::current();
  int year = now / 10000;
  int month = (now / 100) % 100;

  const auto dateFilter = [&](int date) {
    switch (repType) {
    case eReportType::Total: return true;
    case eReportType::Annual: return (date / 10000) == year;
    case eReportType::Monthly: return (((date / 10000) == year)) && ((date / 100) % 100 == month);
    }
  };

  const auto userMatchesFaculty = [&](int userId) {
    if (faculty.empty()) return true;
    User u{};
    return db_find_by_id<User>(userId, &u) && u.faculty == faculty;
  };

  // --- Fetch filtered datasets ---
  auto passes = db_find<ParkingPass>([&](const ParkingPass& p) {
    return dateFilter(p.issueDate) && userMatchesFaculty(p.userid);
  });

  auto apps = db_find<ParkingApplication>([&](const ParkingApplication& a) {
    return dateFilter(a.submissionDate) && userMatchesFaculty(a.userid);
  });

  // --- Aggregation ---
  size_t active = 0, expired = 0;
  size_t approved = 0, rejected = 0;

  std::map<int, vector<ParkingPass>> userPasses;

  for (const auto& p : passes) {
    active  += (p.status == ePassStatus::Active);
    expired += (p.status == ePassStatus::Expired);
    userPasses[p.userid].push_back(p);
  }

  for (const auto& a : apps) {
    approved += (a.status == eApplicationStatus::Completed);
    rejected += (a.status == eApplicationStatus::Rejected);
  }

  // --- Renewal + interval ---
  size_t renewals = 0, count = 0;
  double totalInterval = 0.0;

  for (auto& p : userPasses) {
    auto& vec = p.second;
    if (vec.size() <= 1) continue;

    std::sort(vec.begin(), vec.end(),
      [](const auto& a, const auto& b) {
        return a.issueDate < b.issueDate;
      });

    renewals += vec.size() - 1;

    for (size_t i = 1; i < vec.size(); ++i) {
      totalInterval += date::days_between(vec[i - 1].issueDate, vec[i].issueDate);
      count++;
    }
  }

  double avgInterval = count ? totalInterval / count : 0.0;

  // --- Utilization ---
  size_t users = userPasses.size();
  size_t vehicles = db_get_records<Vehicle>().size();
  double utilization = users ? (double)vehicles / users : 0.0;

  // --- Header ---
  cli_clear();
  string header = "Parking System Report";

  if (repType == eReportType::Annual)
    header += " (Annual - " + std::to_string(year) + ")";
  else if (repType == eReportType::Monthly)
    header += " (Monthly - " + std::to_string(year) + "-" +
              (month < 10 ? "0" : "") + std::to_string(month) + ")";

  if (!faculty.empty())
    header += " [" + faculty + "]";

  cli_header(header);

  // --- Output ---
  cli_field("Total passes", passes.size());
  cli_field("Active passes", active);
  cli_field("Expired passes", expired);
  cli_field("Renewed passes", renewals);
  cli_field("Avg renewal interval (days)", avgInterval);

  cli_field("Total applications", apps.size());
  cli_field("Approved applications", approved);
  cli_field("Rejected applications", rejected);

  cli_field("Avg car utilization", utilization);

  cli_press_enter();
}