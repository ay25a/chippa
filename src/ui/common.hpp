#pragma once

#include "database/entities.hpp"

// Helpers
#define UI_FACULTY_MENU() cli_menu(std::vector<std::string>(std::begin(C_FACULTIES), std::end(C_FACULTIES)))
#define REPEATED_BEGIN() for(;;) {
#define REPEATED_BREAK() break
#define REPEATED_END() }

// Authentication
extern User gCurrentUser;
bool ui_authentication();
User ui_login();
User ui_register();

// Menu
void ui_profile(const User& user);
void ui_edit_profile();

void ui_vehicles(const User& user);
void ui_add_vehicle();
void ui_edit_vehicle(const Vehicle& vh);

void ui_user_applications(const User& user);
void ui_make_application();
void ui_view_application(const ParkingApplication& app);

void ui_view_pass(const ParkingPass& user);
void ui_view_user_pass(const User& user);

void ui_view_applications();
void ui_view_passes();

void ui_student_menu();
inline void ui_staff_menu() {}