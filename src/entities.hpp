/// @file entity.hpp
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
/// - Entities should contain a `match` method with a parameter of the same type and returns a boolean, 
///   used to deduce if the entity values match with the filter values.
/// MATCH FUNCTION:
/// The `match` function performs partial matching:
/// - Fields with default values (0 or '\0') are ignored
/// - Only explicitly set fields are compared
#pragma once

#include <cstdint>

// **************************************
// User Related
// **************************************
constexpr const char* C_FACULTIES[] = {"LCK_FES", "FCI", "FAM", "FEd", "FCS", "MK_FMHS"};
enum class eUserStatus: uint16_t { Unknown = 0, Active, Suspended };
enum class eUserRole: uint16_t { Unknown = 0, Student, Staff };

struct User {
  static constexpr const char* FILE = "users.dat";
  int id = 0;
  char password[12] = "";
  char contactNumber[12] = "";
  char fullname[32] = "";
  char faculty[10] = "";
  uint16_t age = 0;
  eUserStatus status = eUserStatus::Unknown;
  eUserRole role = eUserRole::Unknown;
};

struct Vehicle {
  static constexpr const char* FILE = "vehicles.dat";
  int id = 0;
  int userid = 0;
  char model[8] = "";
  char plate[8] = "";
};

// **************************************
// Parking Related
// **************************************
enum class ePassDuration: uint16_t { Unknown = 0, OneMonth, TwoMonths, ThreeMonths };
enum class ePassStatus: uint16_t { Unknown = 0, Active, Expired };
enum class eApplicationStatus: uint16_t { Unknown = 0, Rejected, Completed, PendingPayment, WaitingForReview };
struct ParkingApplication{
  static constexpr const char* FILE = "applications.dat";
  int id = 0;
  int userid = 0;
  int newPassID = 0;
  int submissionDate = 0, closedDate = 0;
  ePassDuration duration = ePassDuration::Unknown;
  eApplicationStatus status = eApplicationStatus::Unknown;
};

struct ParkingPass{
  static constexpr const char* FILE = "passes.dat";
  int id = 0;
  int userid = 0;
  int appid = 0;
  int issueDate = 0;
  ePassDuration duration = ePassDuration::Unknown;
  ePassStatus status = ePassStatus::Unknown; 
};