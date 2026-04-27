/// @file entities.hpp
/// @brief Contains all the structures that represent a database record. All structures are 
/// aligned manually for compatibility with different compilers on different operating systems. 
///
/// IMPORTANT:
/// These structures are written directly into their respective file using their binary layout; 
/// Therefore, any change in field order, data types, data sizes, or addition/removal of fields 
/// will invalidate existing records.
///
/// ENTITIES SPECIFICATIONS:
/// Every entity in the file has two main important attributes: 
/// - integer “id”: Essential for sorting and binary searching the records in the database. 
/// - const char* “FILE”: Contains the file name where the entity will be stored, 
///   should be static, and preferably a compile time constant (constexpr). 
#pragma once

#include <array>
#include <cstdint>

/// @brief Contains allowed values to be stored in "User.faculty"
constexpr std::array<const char*, 6> C_FACULTIES = { "LCK_FES", "FCI", "FAM", "FEd", "FCS", "MK_FMHS" };

/// @brief Represents user's status
enum class eUserStatus: uint16_t { Active = 0, Suspended };

/// @brief Represents user's role
enum class eUserRole: uint16_t { Student = 0, Staff };

/// @brief Represents a User record
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

/// @brief Represents a Vehcile record
struct Vehicle {
  static constexpr const char* FILE = "vehicles.dat";

  int id = 0;
  int userid = 0; // < Should be the same as an existing User.id
  char model[8] = {0};
  char plate[8] = {0};
};

/// @brief Contains allowed values to be stored in "ParkingPass.duration" and
/// "ParkingApplication.duration"
constexpr std::array<uint16_t, 3> C_PASS_DURATION = { 30, 60, 90 };

/// @brief Represents an application's current status
enum class eApplicationStatus: uint16_t { Completed = 0, Rejected, PendingPayment, WaitingForReview };

/// @brief Represents a Parking Application record
struct ParkingApplication{
  static constexpr const char* FILE = "applications.dat";

  int id = 0;
  int userid = 0; // < Should be the same as an existing User.id
  int newPassID = 0; // < Should be the same as an existing ParkingPass.id
  int submissionDate = 0, closedDate = 0;
  uint16_t duration = 0;
  eApplicationStatus status = eApplicationStatus::Completed;
};

/// @brief Represents a pass's current status
enum class ePassStatus: uint16_t { Active = 0, Expired };

/// @brief Represents a Parking Pass record
struct ParkingPass{
  static constexpr const char* FILE = "passes.dat";

  int id = 0;
  int userid = 0; // < Should be the same as an existing User.id
  int appid = 0; // < Should be the same as an existing ParkingApplication.id
  int issueDate = 0;
  uint16_t duration = 0;
  ePassStatus status = ePassStatus::Active; 
};