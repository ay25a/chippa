#pragma once

#include "cli_core.hpp"
#include "database.hpp"
#include "entities.hpp"

/// @brief The current active user.
/// @note Should be set as a global variable in `main.cpp`.
extern User gCurrentUser;

const inline bool is_staff(){ return gCurrentUser.role == eUserRole::Staff; }
const inline bool is_student(){ return gCurrentUser.role == eUserRole::Student; }

//*******************************************
// Auth
//*******************************************

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

extern ePageState ProfilePage(const User user);
extern void EditProfilePage();

extern ePageState StudentsPage();

extern void GenerateReport();

// =============================================================================
// Main Menu Pages
// =============================================================================

static void ViewStudentNotifications() {
  ParkingPass filter{};
  filter.userid = gCurrentUser.id;
  filter.status = ePassStatus::Active;

  std::vector<ParkingPass> passes = db_find(filter);
  if (passes.empty())
    return;

  int remaining = date::days_until(passes[0].issueDate, utils::to_int(passes[0].duration));
  if (remaining <= 7)
    cli_warning("Your pass is about to expire in " << remaining << " Days!");
}

static ePageState StudentMenuPage() {
  const std::vector<std::string> menu {
    "Parking Pass", "Applications", "My Vehicles", "My Profile", "Exit" };

  cli_clear();
  cli_field(gCurrentUser.fullname << "\t\t", gCurrentUser.faculty);
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

static void ViewStaffNotifications() {
  ParkingApplication filter{};
  filter.status = eApplicationStatus::WaitingForReview;

  std::vector<ParkingApplication> apps = db_find(filter);
  if (apps.empty())
    return;

  cli_warning("There is " << apps.size() << " pending applications");
}

static ePageState StaffMenuPage() {
  const std::vector<std::string> menu { 
    "View Students", "View Applications", "View Passes",
    "Generate Report", "My Profile", "Exit"};

  cli_clear();
  cli_header("Dashboard");
  cli_field(gCurrentUser.fullname << "\t\t", gCurrentUser.faculty);
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

static void MenuPage() {
  ParkingPass filter{};
  filter.status = ePassStatus::Active;
  std::vector<ParkingPass> passes = db_find<ParkingPass>(filter);
  int currentDate = date::current();

  std::vector<ParkingPass> modified;
  for (auto &p : passes) {
    int elapsed = date::days_between(p.issueDate, currentDate);
    if (elapsed >= utils::to_int(p.duration)) {
      p.status = ePassStatus::Expired;
      modified.push_back(p);
    }
  }

  for (const auto &mod : modified)
    EXPECT(db_update_record(mod));

  if (gCurrentUser.role == eUserRole::Student) {
    if (gCurrentUser.status == eUserStatus::Suspended) {
      cli_error("Your account has been suspended!");
      return;
    }

    while (StudentMenuPage() == ePageState::Continue);
  } else
    while (StaffMenuPage() == ePageState::Continue);
}