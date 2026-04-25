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

  int userid = std::stoi(cli_valid_input("User ID: ", userid_validator));
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

  user.id = std::stoi(cli_valid_input("Organization ID: ", userid_validator));
  if (exists(user.id)) {
    cli_error("User already exists! Try logging in...");
    return;
  }

  string password = cli_valid_input("Password: ", password_validator);
  std::copy(password.begin(), password.end(), &user.password[0]);

  string contact = cli_valid_input("Contact Number: ", contact_validator);
  std::copy(contact.begin(), contact.end(), &user.contactNumber[0]);

  string name = cli_valid_input("Full Name: ", name_validator);
  std::copy(name.begin(), name.end(), &user.fullname[0]);

  user.age = std::stoi(cli_valid_input("Age: ", age_validator));

  cli_subheader("Faculty");
  auto choice = cli_menu({std::begin(C_FACULTIES), std::end(C_FACULTIES)});
  const string &faculty = C_FACULTIES[choice];
  std::copy(faculty.begin(), faculty.end(), &user.faculty[0]);

  cli_subheader("User Type");
  auto role = cli_menu({"Student", "Staff"}) + 1;
  user.role = static_cast<eUserRole>(role);

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

vector<ParkingApplication *> FilterActiveApplications(vector<ParkingApplication> &apps) {
  vector<ParkingApplication *> res;
  if(is_staff()) {
    for(auto& app: apps)
      if(app.status == eApplicationStatus::WaitingForReview)
        res.emplace_back(&app);

    return res;
  }

  for (auto &app : apps) 
    if (app.status == eApplicationStatus::PendingPayment || app.status == eApplicationStatus::WaitingForReview)
      res.emplace_back(&app);

  return res;
}

inline void ViewActiveApplications(const vector<ParkingApplication *> active) {
  vector<vector<string>> values;
  for (const auto &app : active) {
    values.push_back({
        std::to_string(app->id),
        std::to_string(app->userid),
        date::to_string(app->submissionDate),
        utils::to_string(app->duration),
        utils::to_string(app->status),
    });
  }

  cli_table({"ID", "User ID", "Submission Date", "Requested Duration", "Status"}, values);
}

ePageState ApplicationsPage(const User *user) {
  const vector<string> menu { "View Application", "History", "Back"};

  auto apps = db_find<ParkingApplication>([&user](const ParkingApplication& e){ 
    return !user || user->id == e.userid;
  });

  auto active = FilterActiveApplications(apps);

  if (apps.size() == 0) {
    cli_error("No Applications found!");
    return ePageState::Exit;
  }

  cli_clear();
  cli_header("Applications");

  if (active.empty())
    cli_warning("No Active Applications");
  else
    ViewActiveApplications(active);

  switch (cli_menu(menu)) {
  case 0: {
    if (active.size() == 0) {
      cli_error("No Applications to visit!");
      return ePageState::Continue;
    }

    vector<string> choices;
    for (const auto &app : active)
      choices.emplace_back(std::to_string(app->id));

    ApplicationDetailsPage(*active[cli_menu(choices)]);
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
  if (is_staff())
    menu = { "Approve", "Back" };
  else if (app.status == eApplicationStatus::PendingPayment)
    menu = {"Pay", "Back"};

  cli_clear();
  cli_header("Application " << app.id);
  cli_field("Student ID", app.userid);
  cli_field("Submission Date", date::to_string(app.submissionDate));
  cli_field("Duration", utils::to_string(app.duration));
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

  vector<vector<string>> values;
  for (const auto &app : apps)
    values.push_back({
        std::to_string(app.id),
        date::to_string(app.submissionDate),
        app.closedDate == 0 ? "Ongoing" : date::to_string(app.closedDate),
        utils::to_string(app.status),
    });
  

  cli_table({"ID", "Submission Date", "Closed Date", "Status"}, values);
  cli_press_enter();
}

inline void ViewActivePasses(const vector<ParkingPass> &passes) {
  vector<vector<string>> values;
  for (const auto &p : passes) 
    values.push_back({
        std::to_string(p.id),
        std::to_string(p.userid),
        date::to_string(p.issueDate),
        std::to_string(date::days_until(p.issueDate, utils::to_int(p.duration))),
        utils::to_string(p.duration),
    });

  cli_table({"ID", "User ID", "Issue Date", "Remaining", "Duration"}, values);
}

ePageState PassesPage(const User *user) {
  const vector<string> menu { "History", "Exit" };
  cli_clear();
  cli_header("Passes");

  auto active = db_find<ParkingPass>([&user](const ParkingPass& p){
    return p.status == ePassStatus::Active && (!user ? true : user->id == p.userid);
  });

  if (is_student() && active.empty()) {
    cli_text("You don't have any active passes");

    auto apps = db_find<ParkingApplication>([](const ParkingApplication& e){
      return gCurrentUser.id == e.userid;
    });

    bool activeApp = !FilterActiveApplications(apps).empty();
    if (!activeApp && cli_boolean("Apply for a new Pass? ")) {
      NewApplicationPage();
      return ePageState::Continue;
    }
  } else if (is_staff() && active.empty())
    cli_text("No Active passes in the system");
  else
    ViewActivePasses(active);

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
  ePassDuration dur = static_cast<ePassDuration>(cli_menu({"1 Month", "2 Months", "3 Months"}) + 1);

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

  ParkingPass filter{};
  filter.userid = user ? user->id : 0;
  auto passes = db_find<ParkingPass>([&user](const ParkingPass& p){
    return !user || user->id == p.userid;
  });

  vector<vector<string>> values;
  for (const auto &p : passes) {
    values.push_back({
        std::to_string(p.id),
        std::to_string(p.userid),
        date::to_string(p.issueDate),
        utils::to_string(p.duration),
        utils::to_string(p.status),
    });
  }

  cli_table({"ID", "User ID", "Issue Date", "Duration", "Status"}, values);
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

  if (!vhs.empty()) {
    vector<vector<string>> values;
    for (const auto &v : vhs)
      values.push_back({v.plate, v.model});

    cli_table({"Plate", "Model"}, values);
  }

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
        if (in.size() != 6 || !is_integer(in.substr(3, 5)))
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

ePageState ProfilePage(const User user) {
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
  cli_field("Name", user.fullname);
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
    string name = cli_valid_input("New Name: ", name_validator);
    copy(name, &newUser.fullname[0], 32);
    break;
  }
  case 1: {
    int age = std::stoi(cli_valid_input("New Age: ", age_validator));
    newUser.age = age;
    break;
  }
  case 2: {
    string contact = cli_valid_input("New Contact Number: ", contact_validator);
    copy(contact, &newUser.contactNumber[0], 12);
    break;
  }
  case 3: {
    string password = cli_valid_input("New Password: ", password_validator);
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
  cli_press_enter();
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

  vector<vector<string>> values;
  for (const auto &s : students) {
    values.push_back({
        std::to_string(s.id),
        s.fullname,
        s.faculty,
        utils::to_string(s.status),
    });
  }

  cli_table({"ID", "Name", "Faculty", "Status"}, values);
  cli_separator(20);

  if (cli_menu(menu) == 0) {
    vector<string> choices;
    for (const auto &s : students)
      choices.emplace_back(std::to_string(s.id));

    return ProfilePage(students[cli_menu(choices)]);
  }

  return ePageState::Exit;
}

void GenerateReport() {
  enum class eReportType { Total, Annual, Monthly };

  const vector<string> menu{ "Total Report", "Annual Report", "Monthly Report" };
  cli_clear();
  cli_header("Select Report Type");
  eReportType reportType = static_cast<eReportType>(cli_menu(menu));

  // --- Date context ---
  int currentDate = date::current();
  int currentYear = currentDate / 10000;
  int currentMonth = (currentDate / 100) % 100;

  auto dateFilter = [&](int d) {
    if (reportType == eReportType::Total) return true;
    int y = d / 10000;
    int m = (d / 100) % 100;
    if (reportType == eReportType::Annual) return y == currentYear;
    return (y == currentYear && m == currentMonth);
  };

  // --- Fetch filtered datasets ---
  auto passes = db_find<ParkingPass>([&](const ParkingPass& p) {
    return dateFilter(p.issueDate);
  });

  auto apps = db_find<ParkingApplication>([&](const ParkingApplication& a) {
    return dateFilter(a.submissionDate);
  });

  // --- Aggregation ---
  size_t totalPasses = passes.size();
  size_t totalActivePasses = 0;
  size_t totalExpiredPasses = 0;

  size_t approvedApplications = 0;
  size_t rejectedApplications = 0;

  std::map<int, std::vector<ParkingPass>> userPasses;

  for (const auto& p : passes) {
    totalActivePasses += (p.status == ePassStatus::Active);
    totalExpiredPasses += (p.status == ePassStatus::Expired);
    userPasses[p.userid].push_back(p);
  }

  for (const auto& a : apps) {
    approvedApplications += (a.status == eApplicationStatus::Completed);
    rejectedApplications += (a.status == eApplicationStatus::Rejected);
  }

  // --- Renewal + Interval ---
  size_t totalRenewedPasses = 0;
  double totalInterval = 0.0;
  size_t intervalCount = 0;

  for (auto& up : userPasses) {
    if (up.second.size() <= 1) continue;

    std::sort(up.second.begin(), up.second.end(),
      [](const ParkingPass& a, const ParkingPass& b) {
        return a.issueDate < b.issueDate;
      });

    totalRenewedPasses += up.second.size() - 1;

    for (size_t i = 1; i < up.second.size(); ++i) {
      totalInterval += date::days_between(up.second[i - 1].issueDate, up.second[i].issueDate);
      intervalCount++;
    }
  }

  double avgRenewalInterval =
    intervalCount ? (totalInterval / intervalCount) : 0.0;

  // --- Utilization ---
  size_t usersWithPass = userPasses.size();
  size_t totalVehicles = db_get_records<Vehicle>().size();

  double avgUtilization =
    usersWithPass ? static_cast<double>(totalVehicles) / usersWithPass : 0.0;

  // --- Display ---
  cli_clear();
  string header = "Parking System Report";

  if (reportType == eReportType::Annual)
    header += " (Annual - " + std::to_string(currentYear) + ")";
  else if (reportType == eReportType::Monthly)
    header += " (Monthly - " + std::to_string(currentYear) + "-" +
              (currentMonth < 10 ? "0" : "") + std::to_string(currentMonth) + ")";

  cli_header(header);

  cli_field("Total number of passes", totalPasses);
  cli_field("Total number of active passes", totalActivePasses);
  cli_field("Total number of expired passes", totalExpiredPasses);
  cli_field("Total number of renewed parking passes", totalRenewedPasses);
  cli_field("Average renewal interval (days)", avgRenewalInterval);

  cli_field("Total applications", apps.size());
  cli_field("Approved applications", approvedApplications);
  cli_field("Rejected applications", rejectedApplications);

  cli_field("Average car utilization (vehicles per user)", avgUtilization);

  cli_press_enter();
}