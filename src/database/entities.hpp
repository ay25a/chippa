/** @file entities.hpp
 *  @brief Stores the Entites signatures that are going to be stored as records in the Database.
 *
 *  DESIGN OVERVIEW:
 *  ================
 *  This is a part of the Database module, all structs have fixed size, 
 *  alignment, and padding in different compilers.
 */
#pragma once

#include <cstring>
#include <cstdint>

enum class eUserStatus: uint16_t { Unknown = 0, Active, Suspended };
enum class eUserRole: uint16_t { Unknown = 0, Student, Staff };

constexpr const char* C_FACULTIES[] = {"LCK_FES", "FCI", "FAM", "FEd", "FCS", "MK_FMHS"};

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
    if(f.plate[0] != '\0' && strcmp(f.model, model) != 0) return false;
    if(f.model[0] != '\0' && strcmp(f.plate, plate) != 0) return false;
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
  int userid;
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