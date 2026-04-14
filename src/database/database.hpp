#pragma once

#include <vector>
#include "entities.hpp"


extern std::vector<User> gUsers;
extern std::vector<Vehicle> gVehicles;
extern std::vector<ParkingApplication> gApplications;
extern std::vector<ParkingPass> gPasses;

#define DB_ATTR_DILM  '|'
#define DB_ENTRY_DILM '\n'

enum class eDatabaseEntity { User = 0, Vehicle, Application, Pass };
void db_load(eDatabaseEntity ent);

void db_add_entry(const User& user);
void db_add_entry(const Vehicle& v);
void db_add_entry(const ParkingApplication& app);
void db_add_entry(const ParkingPass& pass);

void db_update_entry(const User& user);
void db_update_entry(const Vehicle& v, bool del=false);
void db_update_entry(const ParkingApplication& app);
void db_update_entry(const ParkingPass& pass);

std::vector<User> db_match_entry(const User& filter, bool limitToOne=false);
std::vector<Vehicle> db_match_entry(const Vehicle& v, bool limitToOne=false);
std::vector<ParkingApplication> db_match_entry(const ParkingApplication& app, bool limitToOne=false);
std::vector<ParkingPass> db_match_entry(const ParkingPass& user, bool limitToOne=false);