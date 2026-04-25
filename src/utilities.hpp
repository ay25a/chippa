#pragma once

#include "entities.hpp"
#include <cstdio>
#include <ctime>
#include <string>

namespace date {
  static int current() {
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);

    int year  = now->tm_year + 1900;
    int month = now->tm_mon + 1;
    int day   = now->tm_mday;

    return year * 10000 + month * 100 + day;
  }

  static std::time_t to_timet(int date) {
  std::tm tm = {};
  tm.tm_year = (date / 10000) - 1900;
  tm.tm_mon  = ((date / 100) % 100) - 1;
  tm.tm_mday = date % 100;
  tm.tm_isdst = -1; 

  return std::mktime(&tm);
  }

  static int days_between(int date1, int date2) {
    std::time_t t1 = to_timet(date1);
    std::time_t t2 = to_timet(date2);

    double seconds = std::difftime(t2, t1);
    return static_cast<int>(seconds / (60 * 60 * 24));
  }

  static const char* to_string(int date){
    static char buffer[11]; // "YYYY-MM-DD"

    int year  = date / 10000;
    int month = (date / 100) % 100;
    int day   = date % 100;

    std::snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", year, month, day);
    return buffer;
  }

  inline int days_until(int startDate, int duration){
    return duration - days_between(startDate, current());
  }
}

namespace utils {
  constexpr const char* to_string(eUserStatus status){
    switch (status) {
    case eUserStatus::Active:       return "Active";
    case eUserStatus::Suspended:    return "Suspended";
    default:                        return "Unknown";
    }
  }

  constexpr const char* to_string(eUserRole status){
    switch (status) {
    case eUserRole::Staff:      return "Staff";
    case eUserRole::Student:    return "Student";
    default:                    return "Unknown";
    }
  }

  constexpr const char* to_string(ePassStatus status){
    switch (status) {
    case ePassStatus::Active:    return "Active";
    case ePassStatus::Expired:   return "Expired";
    default:                     return "Unknown";
    }
  }

  constexpr const char* to_string(eApplicationStatus status){
    switch (status) {
    case eApplicationStatus::Completed:         return "Completed";
    case eApplicationStatus::PendingPayment:    return "Pending Payment";
    case eApplicationStatus::Rejected:          return "Rejected";
    case eApplicationStatus::WaitingForReview:  return "Waiting For Review";
    default:                                    return "Unknown";
    }
  }

  constexpr const char* to_string(ePassDuration d){
    switch (d) {
    case ePassDuration::OneMonth:     return "1 Month";
    case ePassDuration::TwoMonths:    return "2 Months";
    case ePassDuration::ThreeMonths:  return "3 Months";
    default:                          return "Unknown";
    }
  }

  constexpr uint16_t to_int(ePassDuration duration){
    return static_cast<uint16_t>(duration) * 30;
  }
}

#define EXPECT(cond) if(!cond) throw std::runtime_error("Unexpected Error! " #cond)

enum class ePageState{ Continue = 0, Exit = 1 };

/// @brief Checks if the string consists only of digits
/// @param str The string to check
/// @return true if all characters are digits and the string is not empty, false
/// otherwise
inline bool is_integer(const std::string &str) {
  for (const char ch : str)
    if (!std::isdigit(ch))
      return false;
  return !str.empty();
}

inline std::string userid_validator(const std::string& in){
  if (in.size() < 5 || in.size() > 7)
    return "User ID has to be 5 to 7 numbers";
  
  return !is_integer(in) ? "User ID can contain only numbers!" : "";
}

inline std::string password_validator(const std::string& in){
  return (in.size() < 5 || in.size() >= 12) ? "Password length should be 5 to 11 characters" : "";
}

inline std::string contact_validator(const std::string& in){
  return in.size() < 10 || !is_integer(in) ? "Invalid contact number" : "";
}

inline std::string name_validator(const std::string& in){
  return in.size() < 3 || in.size() > 31 ? "Name length should be 3 to 31 characters" : "";
}

inline std::string age_validator(const std::string& in){
  if(in.size() > 7 || !is_integer(in))
    return "Age can only be numbers between 16 and 99";
  int age = std::stoi(in);
  return (age < 16 || age > 99) ? "Age should be between 16 and 99" : "";
}

