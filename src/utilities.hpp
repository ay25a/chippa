/// @file utilities.hpp
/// @brief a Basic module that contains all reusable utilities, 
/// including UI functions and objects, Enum conversion, and date related utilities. 
#pragma once

#include "entities.hpp"
#include <cstdio>
#include <ctime>
#include <cmath>
#include <string>

/// @brief Expects a condition to be true, used for fatal unexpected crashes
#define EXPECT(cond) if(!cond) throw std::runtime_error("Unexpected Error! " #cond)

/// @namespace date
/// @brief Contains all date related functions
namespace date {

  /// @brief Returns the current date as integer
  static int current() {
    std::time_t t = std::time(nullptr);
    std::tm now;
#ifdef _WIN32
    localtime_s(&now, &t);
#else
    localtime_r(&t, &now);
#endif

    int year  = now.tm_year + 1900;
    int month = now.tm_mon + 1;
    int day   = now.tm_mday;

    return year * 10000 + month * 100 + day;
  }

  /// @brief a Helper function, returns an integer date
  /// as std::time_t
  /// @note doesn't check if the parameter `date` is invalid
  static std::time_t to_timet(int date) {
  std::tm tm = {};
  tm.tm_year = (date / 10000) - 1900;
  tm.tm_mon  = ((date / 100) % 100) - 1;
  tm.tm_mday = date % 100;
  tm.tm_isdst = -1; 

  return std::mktime(&tm);
  }

  /// @brief Gets the days between 2 integer dates
  /// @note Order of parameters doesn't matter
  static int days_between(int date1, int date2) {
    std::time_t t1 = to_timet(date1);
    std::time_t t2 = to_timet(date2);

    double seconds = std::difftime(t2, t1);
    return static_cast<int>(std::abs(seconds / (60 * 60 * 24)));
  }

  /// @brief Converts an integer date to a string in "YYYY-MM-DD"
  /// @param date integer date to convert (does not check for invalid parameter)
  /// @return a C string pointer to the local static string representing the converted date
  static const char* to_string(int date){
    static char buffer[11]; // "YYYY-MM-DD"

    int year  = date / 10000;
    int month = (date / 100) % 100;
    int day   = date % 100;

    std::snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", year, month, day);
    return buffer;
  }

  /// @brief Returns the number of days remaining until a specified duration (in days) 
  /// has passed since startDate.
  /// @param startDate The starting date as an integer.
  /// @param duration The duration in days.
  /// @return The number of days remaining until the duration has passed since startDate.
  /// (can be negative if already passed).
  inline int days_until(int startDate, int duration){
    int days_passed = date::days_between(startDate, date::current());
    return duration - days_passed;
  }
}

/// @namespace utils
/// @brief General purpose utility functions wrapper
namespace utils {
  // *********************************** 
  // Enum-Related 
  // ***********************************
  constexpr const char* to_string(eUserStatus status){
    switch (status) {
    case eUserStatus::Active:       return "Active";
    case eUserStatus::Suspended:    return "Suspended";
    default:                        return "Unknown Value";
    }
  }

  constexpr const char* to_string(eUserRole status){
    switch (status) {
    case eUserRole::Staff:      return "Staff";
    case eUserRole::Student:    return "Student";
    default:                    return "Unknown Value";
    }
  }

  constexpr const char* to_string(ePassStatus status){
    switch (status) {
    case ePassStatus::Active:    return "Active";
    case ePassStatus::Expired:   return "Expired";
    default:                     return "Unknown Value";
    }
  }

  constexpr const char* to_string(eApplicationStatus status){
    switch (status) {
    case eApplicationStatus::Completed:         return "Completed";
    case eApplicationStatus::PendingPayment:    return "Pending Payment";
    case eApplicationStatus::Rejected:          return "Rejected";
    case eApplicationStatus::WaitingForReview:  return "Waiting For Review";
    default:                                    return "Unknown Value";
    }
  }

  // *********************************** 
  // UI-Related 
  // *********************************** 

  /// @brief Indicator to whether the page should be exited or entered again
  enum class ePageState{ Continue = 0, Exit = 1 };

  /// @brief Checks if the string consists only of digits
  /// @return true if all characters are digits and the string is not empty, false
  /// otherwise
  inline bool is_integer(const std::string &str) {
    for (const char ch : str)
      if (!std::isdigit(ch))
        return false;
    return !str.empty();
  }

  /// @brief validate a `User.id`
  /// @param in the input string to check
  /// @return Error message if the input is not between 5-7 characters,
  /// or if the input is not a valid integer. Returns empty string otherwise.
  inline std::string userid_validator(const std::string& in){
    if (in.size() < 5 || in.size() > 7)
      return "User ID has to be 5 to 7 numbers";
  
    return !is_integer(in) ? "User ID can contain only numbers!" : "";
  }

  /// @brief validate a `User.password`
  /// @param in the input string to check
  /// @return Error message if the input is not between 5-11 characters, 
  // or an empty string otherwise.
  inline std::string password_validator(const std::string& in){
    return (in.size() < 5 || in.size() > 11) ? "Password length should be 5 to 11 characters" : "";
  }

  /// @brief validate a `User.contactNumber`
  /// @param in the input string to check
  /// @return Error message if the input is less than 10 numbers,
  /// or if the input is not a valid integer. Returns empty string otherwise.
  inline std::string contact_validator(const std::string& in){
    return in.size() < 10 || !is_integer(in) ? "Invalid contact number" : "";
  }

  /// @brief validate a `User.name`
  /// @param in the input string to check
  /// @return Error message if the input is not between 3-31 characters,
  /// an empty string otherwise.
  inline std::string name_validator(const std::string& in){
    return in.size() < 3 || in.size() > 31 ? "Name length should be 3 to 31 characters" : "";
  }

  /// @brief validate a `User.age`
  /// @param in the input string to check
  /// @return Error message if the input is not between 16-99,
  /// or if the input is not a valid number. Returns an empty string otherwise.
  inline std::string age_validator(const std::string& in){
    if(in.size() > 7 || !is_integer(in))
      return "Age can only be numbers between 16 and 99";
    int age = std::stoi(in);
    return (age < 16 || age > 99) ? "Age should be between 16 and 99" : "";
  }
}