#ifdef _WIN32
#include <windows.h>
#include <limits.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif
#include <fstream>

#include "database.hpp"

// Initialization
#define USERS_PATH  "Users.txt"
#define VEHICLES_PATH "Vehicles.txt"
#define APPLICATIONS_PATH "Applications.txt"
#define PASSES_PATH "Passes.txt"

std::vector<User> gUsers;
std::vector<Vehicle> gVehicles;
std::vector<ParkingApplication> gApplications;
std::vector<ParkingPass> gPasses;

// Helpers
static std::string ReadFile(std::string_view path);
static void WriteToFile(std::string_view path, std::string_view content, bool append = true);

// Loading
std::vector<std::string> ReadAttributes(std::string_view content, size_t &i);
static void LoadUsers();
static void LoadVehicles();
static void LoadApplications();
static void LoadPasses();

void db_load(eDatabaseEntity ent){
  switch (ent) {
  case eDatabaseEntity::User:         LoadUsers(); break;
  case eDatabaseEntity::Vehicle:      LoadVehicles(); break;
  case eDatabaseEntity::Application:  LoadApplications(); break;
  case eDatabaseEntity::Pass:         LoadPasses(); break;
  }
}

// Adding Entry
static std::string Parse(const User& user);
static std::string Parse(const Vehicle& v);
static std::string Parse(const ParkingPass& pass);
static std::string Parse(const ParkingApplication& app);

void db_add_entry(const User& user){ WriteToFile(USERS_PATH, Parse(user)); }
void db_add_entry(const Vehicle& v){ WriteToFile(VEHICLES_PATH, Parse(v)); }
void db_add_entry(const ParkingApplication& app){ WriteToFile(APPLICATIONS_PATH, Parse(app)); }
void db_add_entry(const ParkingPass& pass){ WriteToFile(PASSES_PATH, Parse(pass)); }


// Updating Entries
void db_update_entry(const User& modified){
  std::string content = "";
  for(auto& user: gUsers){
    if(user.UserID == modified.UserID) user = modified;

    content += Parse(user);
  }

  WriteToFile(USERS_PATH, content, false);
}

void db_update_entry(const Vehicle& modified, bool del){
  std::string content = "";
  for(auto& v: gVehicles){
    if(v.LicensePlate == modified.LicensePlate){
      if(del) continue;
      else v = modified;
    }

    content += Parse(v);
  }

  WriteToFile(VEHICLES_PATH, content, false);
  if(del) db_load(eDatabaseEntity::Vehicle);
}

void db_update_entry(const ParkingApplication& modified){
  std::string content = "";
  for(auto& app: gApplications){
    if(app.ApplicationID == modified.ApplicationID) app = modified;
    
    content += Parse(app);
  }

  WriteToFile(APPLICATIONS_PATH, content, false);
}

void db_update_entry(const ParkingPass& modified){
  std::string content = "";
  for(auto& pass: gPasses){
    if(pass.PassID == modified.PassID) pass = modified;
    

    content += Parse(pass);
  }

  WriteToFile(PASSES_PATH, content, false);
}

// Finding Entries
std::vector<User> db_match_entry(const User& f, bool limitToOne){
  std::vector<User> res;
  for(const auto& user: gUsers){
    if(!f.UserID.empty() && f.UserID != user.UserID) continue;
    if(!f.Email.empty() && f.Email != user.Email) continue;
    if(!f.Password.empty() && f.Password != user.Password) continue;
    if(!f.ContactNumber.empty() && f.ContactNumber != user.ContactNumber) continue;
    if(f.Faculty.empty() && f.Faculty != user.Faculty) continue;
    if(f.Status != eUserStatus::Unknown && f.Status != user.Status) continue;
    if(f.Role != eUserRole::Unknown && f.Role != user.Role) continue;
    
    res.push_back(user);
    if(limitToOne) break;
  }

  return res;
}

std::vector<Vehicle> db_match_entry(const Vehicle& f, bool limitToOne){
  std::vector<Vehicle> res;
  for(const auto& ve: gVehicles){
    if(!f.UserID.empty() && ve.UserID != f.UserID) continue;
    if(!f.LicensePlate.empty() && ve.LicensePlate != f.LicensePlate) continue;
    if(!f.Owner.empty() && ve.Owner != f.Owner) continue;
    if(!f.Model.empty() && ve.Model != f.Model) continue;
    
    res.push_back(ve);
    if(limitToOne) break;
  }

  return res;
}

std::vector<ParkingApplication> db_match_entry(const ParkingApplication& f, bool limitToOne){
  std::vector<ParkingApplication> res;
  for(const auto& app: gApplications){
    if(!f.ApplicationID.empty() && app.ApplicationID != f.ApplicationID) continue;
    if(!f.UserID.empty() && app.UserID != f.UserID) continue;
    if(!f.OldPassID.empty() && app.OldPassID != f.OldPassID) continue;
    if(!f.NewPassID.empty() && app.NewPassID != f.NewPassID) continue;
    if(f.Duration != ePassDuration::Unknown && app.Duration != f.Duration) continue;
    if(f.SubmissionDate != 0 && app.SubmissionDate != f.SubmissionDate) continue;
    if(f.ClosedDate != 0 && app.ClosedDate != f.ClosedDate) continue;
    if(f.Status != eApplicationStatus::Unknown && app.Status != f.Status) continue;

    res.push_back(app);
    if(limitToOne) break;
  }

  return res;
}

std::vector<ParkingPass> db_match_entry(const ParkingPass& f, bool limitToOne){
  std::vector<ParkingPass> res;
  for(const auto& pass: gPasses){
    if(!f.PassID.empty() && pass.PassID != f.PassID) continue;
    if(!f.UserID.empty() && pass.UserID != f.UserID) continue;
    if(!f.ApplicationID.empty() && pass.ApplicationID != f.ApplicationID) continue;
    if(f.Duration != ePassDuration::Unknown && pass.Duration != f.Duration) continue;
    if(f.IssueDate != 0 && pass.IssueDate != f.IssueDate) continue;
    if(f.ExpiryDate != 0 && pass.ExpiryDate != f.ExpiryDate) continue;
    if(f.Status != ePassStatus::Unknown && pass.Status != f.Status) continue;

    res.push_back(pass);
    if(limitToOne) break;
  }

  return res;
}



// Helpers
std::string ReadFile(std::string_view path){
  std::ifstream file(path.data());

  if(!file.is_open()){
    WriteToFile(path.data(), "");
    return "";
  }

  file.seekg(0, std::ios_base::end);

  std::streampos pos = file.tellg();
  size_t size = static_cast<size_t>(pos);
  file.seekg(0, std::ios_base::beg);

  std::string res;
  res.resize(size);
  file.read(res.data(), size);
  file.close();

  return res;
}

void WriteToFile(std::string_view path, std::string_view content, bool append){
  std::ofstream file(path.data(), append ? std::ios_base::app : std::ios_base::trunc);
  file.write(content.data(), content.size());
  file.close();
}

// Loading
std::vector<std::string> ReadAttributes(std::string_view content, size_t &i) {
  std::vector<std::string> res;
    
  size_t end = content.find(DB_ENTRY_DILM, i);
  if (end == std::string_view::npos) 
    end = content.size();
    
  std::string_view section = content.substr(i, end - i);
  i = end;

  while (!section.empty()) {
    size_t delim = section.find(DB_ATTR_DILM);
    res.emplace_back(section.substr(0, delim));
    if (delim == std::string_view::npos) 
      break;

    section.remove_prefix(delim + 1);
  }
  return res;
}


static void LoadUsers(){
  std::string content = ReadFile(USERS_PATH);
  for(size_t i = 0; i < content.size(); ++i){
    std::vector<std::string> attrs = ReadAttributes(content, i);

    try {
      User u{attrs[0], attrs[1], attrs[2], attrs[3]};
      u.Faculty = C_FACULTIES[std::stoi(attrs[4])];
      u.Status = static_cast<eUserStatus>(std::stoi(attrs[5]));
      u.Role = static_cast<eUserRole>(std::stoi(attrs[6]));

      gUsers.push_back(u);
    } catch(const std::exception&){
      // error
    }
  } 
}

static void LoadVehicles(){
  std::string content = ReadFile(VEHICLES_PATH);
  for(size_t i = 0; i < content.size(); ++i){
    std::vector<std::string> attrs = ReadAttributes(content, i);

    try {
      Vehicle v{attrs[0], attrs[1], attrs[2], attrs[3]};
      gVehicles.push_back(v);
    } catch(const std::exception&){
      // error
    }
  }
}

static void LoadApplications(){
  std::string content = ReadFile(APPLICATIONS_PATH);
  for(size_t i = 0; i < content.size(); ++i){
    std::vector<std::string> attrs = ReadAttributes(content, i);

    try {
      ParkingApplication app{ 
        attrs[0], attrs[1], 
        attrs[2], attrs[3], 
        static_cast<ePassDuration>(std::stoi(attrs[4])), 
        static_cast<unsigned int>(std::stoi(attrs[5])),
        static_cast<unsigned int>(std::stoi(attrs[6])),
        attrs[7],
        static_cast<eApplicationStatus>(std::stoi(attrs[8])),
      };

      gApplications.push_back(app);
    } catch(const std::exception&){
      // error
    }
  }
}

static void LoadPasses(){
  std::string content = ReadFile(PASSES_PATH);
  for(size_t i = 0; i < content.size(); ++i){
    std::vector<std::string> attrs = ReadAttributes(content, i);

    try {
      ParkingPass pass{
        attrs[0], attrs[1], attrs[2],
        static_cast<ePassDuration>(std::stoi(attrs[3])),
        static_cast<unsigned int>(std::stoi(attrs[4])),
        static_cast<unsigned int>(std::stoi(attrs[5])),
        static_cast<ePassStatus>(std::stoi(attrs[6])),
      };

      gPasses.push_back(pass);
    } catch(const std::exception&){
      // error
    }
  }
}

// Adding Entry
std::string Parse(const User& user){
  return  user.UserID + DB_ATTR_DILM +  user.Email + DB_ATTR_DILM +
          user.Password + DB_ATTR_DILM + user.ContactNumber + DB_ATTR_DILM +
          user.Faculty + DB_ATTR_DILM + 
          std::to_string(static_cast<int>(user.Status)) + DB_ATTR_DILM +
          std::to_string(static_cast<int>(user.Role)) + DB_ENTRY_DILM;
}

std::string Parse(const Vehicle& v){
  return  v.UserID + DB_ATTR_DILM + v.Owner + DB_ATTR_DILM + 
          v.Model + DB_ATTR_DILM + v.LicensePlate + DB_ENTRY_DILM;
}

std::string Parse(const ParkingPass& pass){
  return  pass.PassID + DB_ATTR_DILM + pass.ApplicationID + DB_ATTR_DILM +
          pass.UserID + DB_ATTR_DILM + 
          std::to_string(static_cast<int>(pass.Duration)) + DB_ATTR_DILM +
          std::to_string(pass.IssueDate) + DB_ATTR_DILM +
          std::to_string(pass.ExpiryDate) + DB_ATTR_DILM + 
          std::to_string(static_cast<int>(pass.Status)) + DB_ENTRY_DILM;
}

std::string Parse(const ParkingApplication& app){
  return  app.ApplicationID + DB_ATTR_DILM + app.UserID + DB_ATTR_DILM +
          app.OldPassID + DB_ATTR_DILM + app.NewPassID + DB_ATTR_DILM +
          std::to_string(static_cast<int>(app.Duration)) + DB_ATTR_DILM + 
          std::to_string(app.SubmissionDate) + DB_ATTR_DILM +
          std::to_string(app.ClosedDate) + DB_ATTR_DILM + 
          app.Note + DB_ATTR_DILM +
          std::to_string(static_cast<int>(app.Status)) + DB_ENTRY_DILM;
}