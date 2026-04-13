#pragma once

#include "database/user.hpp"
#include "terminal/cli.hpp"

#define UI_FACULTY_MENU() cli_menu({"LCK_FES", "FCI", "FAM", "FCS", "FEd", "MK_FMHS"})

// Authentication
void ui_authentication();
void ui_login();
void ui_register();

// Menu
void ui_profile(const User& user);
void ui_edit_profile();
void ui_student_menu();
inline void ui_staff_menu() {}