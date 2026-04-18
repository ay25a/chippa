/// @file entity.hpp
/// Contains all the structs that represents a Database record
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

#include <cstring>
#include <cstdint>

// User
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
  unsigned short age = 0;
  eUserStatus status = eUserStatus::Unknown;
  eUserRole role = eUserRole::Unknown;

  bool match(const User& f) const {
    if(f.id != 0 && f.id != id) return false;
    if(f.password[0] != '\0' && strcmp(f.password, password) != 0) return false;
    if(f.contactNumber[0] != '\0' && strcmp(f.contactNumber, contactNumber) != 0) return false;
    if(f.fullname[0] != '\0' && strcmp(f.fullname, fullname) != 0) return false;
    if(f.faculty[0] != '\0' && strcmp(f.faculty, faculty) != 0) return false;
    if(f.age != 0 && f.age != age) return false;
    if(f.status != eUserStatus::Unknown && f.status != status) return false;
    if(f.role != eUserRole::Unknown && f.role != role) return false;
    return true;
  }
};

// Vehicle
struct Vehicle {
  static constexpr const char* FILE = "vehicles.dat";
  int id = 0;
  int userid = 0;
  char model[8] = "";
  char plate[8] = "";

  bool match(const Vehicle& f) const {
    if(f.id != 0 && f.id != id) return false;
    if(f.userid != 0 && f.userid != userid) return false;
    if(f.plate[0] != '\0' && strcmp(f.plate, model) != 0) return false;
    if(f.model[0] != '\0' && strcmp(f.model, plate) != 0) return false;
    return true;
  }
};

// Parking Pass
enum class ePassDuration: uint16_t { Unknown = 0, OneMonth, TwoMonths, ThreeMonths };
enum class ePassStatus: uint16_t { Unknown = 0, Active, Suspended, Expired };

struct ParkingPass{
  static constexpr const char* FILE = "passes.dat";
  int id = 0;
  int userid = 0;
  int appid = 0;
  int issueDate = 0;
  ePassDuration duration = ePassDuration::Unknown;
  ePassStatus status = ePassStatus::Unknown; 

  bool match(const ParkingPass& f) const {
    if(f.id != 0 && f.id != id) return false;
    if(f.userid != 0 && f.userid != userid) return false;
    if(f.appid != 0 && f.appid != appid) return false;
    if(f.issueDate != 0 && f.issueDate != issueDate) return false;
    if(f.duration != ePassDuration::Unknown && f.duration != duration) return false;
    if(f.status != ePassStatus::Unknown && f.status != status) return false;
    return true;
  }
};

// Parking Application
enum class eApplicationStatus: uint16_t { Unknown = 0, Rejected, Completed, PendingPayment, WaitingForReview };

struct ParkingApplication{
  static constexpr const char* FILE = "applications.dat";
  int id = 0;
  int userid = 0;
  int oldPassID = 0, newPassID = 0;
  int submissionDate = 0, closedDate = 0;
  ePassDuration duration = ePassDuration::Unknown;
  eApplicationStatus status = eApplicationStatus::Unknown;

  bool match(const ParkingApplication& f) const {
    if(f.id != 0 && f.id != id) return false;
    if(f.userid != 0 && f.userid != userid) return false;
    if(f.oldPassID != 0 && f.oldPassID != oldPassID) return false;
    if(f.newPassID != 0 && f.newPassID != newPassID) return false;
    if(f.submissionDate != 0 && f.submissionDate != submissionDate) return false;
    if(f.closedDate != 0 && f.closedDate != closedDate) return false;
    if(f.duration != ePassDuration::Unknown && f.duration != duration) return false;
    if(f.status != eApplicationStatus::Unknown && f.status != status) return false;
    return true;
  }
};