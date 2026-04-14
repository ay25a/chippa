#pragma once

#include <string>

// Users
enum class eUserStatus { Unknown = 0, Active, Suspended };
enum class eUserRole { Unknown = 0, Student, Staff };

constexpr const char* C_FACULTIES[] = {"LCK_FES", "FCI", "FAM", "FEd", "FCS", "MK_FMHS"};

struct User {
  std::string UserID;
  std::string Email;
  std::string Password;
  std::string ContactNumber;
  std::string Faculty;
  eUserStatus Status;
  eUserRole Role;
};

// Vehicle
struct Vehicle{
  std::string UserID;
  std::string Owner;
  std::string Model;
  std::string LicensePlate;
};

// Parking Pass
enum class ePassDuration { Unknown = 0, OneMonth, TwoMonths, ThreeMonths };
enum class ePassStatus { Unknown = 0, Active, Suspended, Expired };

struct ParkingPass{
  std::string UserID, ApplicationID, PassID;
  ePassDuration Duration;
  unsigned int IssueDate = 0, ExpiryDate = 0;
  ePassStatus Status;
};

// Parking Application
enum class eApplicationStatus { Unknown = 0, Rejected, Completed, PendingPayment, WaitingForReview };

struct ParkingApplication{
  std::string ApplicationID;
  std::string UserID;
  std::string OldPassID, NewPassID;
  ePassDuration Duration;
  unsigned int SubmissionDate = 0, ClosedDate = 0;
  std::string Note;
  eApplicationStatus Status;
};