/// @file entities.hpp
/// @brief Contains all the structs that represents a database record (aligned manually)
///
/// IMPORTANT:
/// These structs are written directly to binary files using their raw memory layout.
/// Therefore:
/// - Field order must NOT be changed
/// - Field types and sizes must NOT be changed
/// - Adding/removing fields will break existing data files
///
/// ENTITIES SPECIFICATIONS:
/// - Entities should contain an integer `id` used for sorting and binary search.
/// - Entities should contain a static string `FILE` used for database file path (ideally constexpr).
#pragma once

#include <array>
#include <cstdint>

// **************************************
// User Related
// **************************************
constexpr std::array<const char*, 6> C_FACULTIES = { "LCK_FES", "FCI", "FAM", "FEd", "FCS", "MK_FMHS" };
enum class eUserStatus: uint16_t { Active = 0, Suspended };
enum class eUserRole: uint16_t { Student = 0, Staff };

struct User {
  static constexpr const char* FILE = "users.dat";
  int id = 0;
  char password[12] = {0};
  char contactNumber[12] = {0};
  char name[32] = {0};
  char faculty[10] = {0};
  uint16_t age = 0;
  eUserStatus status = eUserStatus::Active;
  eUserRole role = eUserRole::Student;
};

struct Vehicle {
  static constexpr const char* FILE = "vehicles.dat";
  int id = 0;
  int userid = 0;
  char model[8] = {};
  char plate[8] = {};
};

// **************************************
// Parking Related
// **************************************
constexpr std::array<uint16_t, 3> C_PASS_DURATION = { 30, 60, 90 };
enum class eApplicationStatus: uint16_t { Completed = 0, Rejected, PendingPayment, WaitingForReview };

struct ParkingApplication{
  static constexpr const char* FILE = "applications.dat";
  int id = 0;
  int userid = 0;
  int newPassID = 0;
  int submissionDate = 0, closedDate = 0;
  uint16_t duration = 0;
  eApplicationStatus status = eApplicationStatus::Completed;
};

enum class ePassStatus: uint16_t { Active = 0, Expired };
struct ParkingPass{
  static constexpr const char* FILE = "passes.dat";
  int id = 0;
  int userid = 0;
  int appid = 0;
  int issueDate = 0;
  uint16_t duration = 0;
  ePassStatus status = ePassStatus::Active; 
};