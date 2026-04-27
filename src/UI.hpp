/// @file UI.hpp
/// @brief Combines all project modules into a single functional user 
/// interface, including authentication, parking passes and pass applications functionalities. 
///
/// Furthermore, it contains three main categories: 
/// - Authentication: Includes logging in and registration of new users. 
/// - Staff pages: Includes viewing and editing staff’s own profile, viewing students, 
///   viewing applications and passes, approving applications, suspending students, 
///   and generating analytical reports. 
/// - Student pages: Includes viewing and editing student’s own profile, viewing student’s own 
///   applications and passes, adding and removing vehicles, and applying for new parking passes.  
#pragma once

#include "cli_core.hpp"
#include "database.hpp"
#include "entities.hpp"

using utils::ePageState;

/// @brief The current active user.
/// @note Should be set as a global variable in `main.cpp`.
extern User gCurrentUser;
 
const inline bool is_staff(){ return gCurrentUser.role == eUserRole::Staff; }
const inline bool is_student(){ return gCurrentUser.role == eUserRole::Student; }

// *******************************************
// Auth
// *******************************************

extern void LoginPage();
extern void RegisterPage();
extern ePageState AuthenticationPage();

// =============================================================================
// UI Pages
// =============================================================================

extern std::vector<ParkingApplication *>
FilterActiveApplications(std::vector<ParkingApplication> &apps);

extern ePageState ApplicationsPage(const User *user);
extern void ApplicationDetailsPage(ParkingApplication app);
extern void
ApplicationsHistoryPage(const std::vector<ParkingApplication> &apps);

extern ePageState PassesPage(const User *user);
extern void NewApplicationPage();
extern void PassHistoryPage(const User *user);

extern ePageState VehiclesPage();
extern void NewVehiclePage();
extern void RemoveVehiclePage(const std::vector<Vehicle> &vehicles);

extern ePageState ProfilePage(const User& user);
extern void EditProfilePage();

extern ePageState StudentsPage();


/// @brief Generats a report based on a time interval and faculty
extern void GenerateReport();

// =============================================================================
// Main Menu Pages
// =============================================================================

/// @brief Prints a warning if student's pass is about to expire in 7 days
static void ViewStudentNotifications() {
  int remaining = 0;
  auto passes = db_find<ParkingPass>([&remaining](const ParkingPass& p){
    remaining = date::days_until(p.issueDate, p.duration);
    return p.userid == gCurrentUser.id && p.status == ePassStatus::Active && remaining <= 7;
  });

  if (!passes.empty())
    cli_warning("Your pass is about to expire in " << remaining << " Days!");
}

static ePageState StudentMenuPage() {
  const std::vector<std::string> menu {
    "Parking Pass", "Applications", "My Vehicles", "My Profile", "Exit" };

  cli_clear();
  cli_header(gCurrentUser.name << "\t\t" << gCurrentUser.faculty);
  ViewStudentNotifications();

  switch (cli_menu(menu)) {
  case 0:
    while (PassesPage(&gCurrentUser) == ePageState::Continue);
    break;
  case 1:
    while (ApplicationsPage(&gCurrentUser) == ePageState::Continue);
    break;
  case 2:
    while (VehiclesPage() == ePageState::Continue);
    break;
  case 3:
    while (ProfilePage(gCurrentUser) == ePageState::Continue);
    break;
  case 4:
    return ePageState::Exit;
  }

  return ePageState::Continue;
}

/// @brief Prints a warning with the amount of pending applications
static void ViewStaffNotifications() {
  auto apps = db_find<ParkingApplication>([](const ParkingApplication& p){
    return p.status == eApplicationStatus::WaitingForReview;
  });

  if (!apps.empty())
    cli_warning("There is " << apps.size() << " pending applications");
}

static ePageState StaffMenuPage() {
  const std::vector<std::string> menu { 
    "View Students", "View Applications", "View Passes",
    "Generate Report", "My Profile", "Exit"};

  cli_clear();
  cli_header(gCurrentUser.name << "\t\t" << gCurrentUser.faculty);
  ViewStaffNotifications();

  switch (cli_menu(menu)) {
  case 0:
    while (StudentsPage() == ePageState::Continue);
    break;
  case 1:
    while (ApplicationsPage(nullptr) == ePageState::Continue);
    break;
  case 2:
    while (PassesPage(nullptr) == ePageState::Continue);
    break;
  case 3:
    GenerateReport();
    break;
  case 4:
    while (ProfilePage(gCurrentUser) == ePageState::Continue);
    break;
  case 5:
    return ePageState::Exit;
  }

  return ePageState::Continue;
}

/// @brief Refereshes the Passes expiry, then prints a menu based on
/// the logged in user's role
static void MenuPage() {
  auto expired = db_find<ParkingPass>([](const ParkingPass& p){
    return p.status == ePassStatus::Active && date::days_until(p.issueDate, p.duration) <= 0;
  });


  for (auto &p : expired){
    p.status = ePassStatus::Expired;
    EXPECT(db_update_record(p));
  }

  if (is_student() && gCurrentUser.status == eUserStatus::Suspended) {
    cli_error("Your account has been suspended!");
    return;
  }

  while (is_student() && StudentMenuPage() == ePageState::Continue);
  while (is_staff() && StaffMenuPage() == ePageState::Continue);
}