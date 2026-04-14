#pragma once

#include "database/entities.hpp"

#define UI_FACULTY_MENU() cli_menu(std::vector<const char*>(std::begin(C_FACULTIES), std::end(C_FACULTIES)))

extern User gCurrentUser;

// Authentication
User ui_authentication();
User ui_login();
User ui_register();

// Menu
void ui_profile(const User& user);
void ui_edit_profile();
void ui_student_menu();
inline void ui_staff_menu() {}