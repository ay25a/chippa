#include "elements.hpp"
#include "terminal/cli.hpp"


void ui_student() {
  cli_clear();
  cli_header("Welocme " << gCurrentUser.fullname << '!');

  switch (cli_menu({"Student Profile", "Registered Vehicles", "Parking Passes", "Applications", "Exit"})) {
    case 0:
      ui_profile(gCurrentUser);
      break;
    case 1:
      ui_vehicles(gCurrentUser);
      break;
    case 2:
      ui_passes(gCurrentUser);
      break;
    case 3:
      ui_applications(gCurrentUser);
      break;
    case 4:
      exit(0);
  }
}

void ui_vehicles(const User &user){
  FOR_LOOP_BEGIN()
  cli_clear();
  cli_header("Vehicles");

 
  auto vhs = db_find<Vehicle>({0, gCurrentUser.id});
  cli_field("Vehicles Registered", vhs.size());

  for(const auto& vh: vhs)
    cli_field(vh.plate, vh.model);

  cli_separator(10);
  switch (cli_menu({"Add", "Delete", "Back"})) {
    case 0:
      ui_add_vehicle();
      break;
    case 1:
      ui_delete_vehicle(vhs);
      break;
    case 2:
      return;
    }
  FOR_LOOP_END()
}

void ui_add_vehicle() {
  cli_clear();
  cli_header("Add a new Vehicle");

  std::string plate = cli_input_valid<std::string>("License Plate: ", [](const std::string& in, std::string& out){
    if(in.size() == 7 && std::isdigit(in[3]) && std::isdigit(in[4]) && std::isdigit(in[5]) && std::isdigit(in[6])){
      out = in;
      return "";
    }

    return "Please enter a valid license plate Number!";
  });

  std::hash<std::string> hasher;
  int id = static_cast<int>(hasher(plate)) + gCurrentUser.id;
  if(db_find_by_id<Vehicle>(id, nullptr)){
    cli_error("Vehicle already Exists!");
    cli_confirm();
    return;
  }

  std::string model = cli_input_valid<std::string>("Model (company name.): ", [](const std::string& in, std::string& out){
    if(in.size() < 2 || in.size() > 8)
      return "Please Enter a valid car model!";
    out = in;
    return "";
  });

  
  Vehicle vh{id, gCurrentUser.id};
  std::copy(plate.begin(), plate.end(), &vh.plate[0]);
  std::copy(model.begin(), model.end(), &vh.model[0]);

  if(!db_add_record(vh)){
    cli_error("Failed to add a new Vehicle (Unknown Reason)");
    cli_confirm();
  }
}

void ui_delete_vehicle(const std::vector<Vehicle>& owned){
  cli_clear();
  std::vector<std::string> plates(owned.size());
  for(size_t i = 0; i < plates.size(); ++i)
    plates[i] = owned[i].plate;

  auto index = cli_menu(plates);
  if(!cli_boolean("Confirm deletion of " + plates[index] + "? "))
    return;

  if(!db_delete_record<Vehicle>(owned[index].id)){
    cli_error("Failed to delete the Vehicle (Unknown Error)");
    cli_confirm();
  }
}



void ui_applications(const User& user){
  FOR_LOOP_BEGIN()
  cli_clear();
  cli_header("My Parking Passes");

  auto passes = db_find<ParkingPass>({0, user.id});
  if(passes.empty()) {
    std::cout << "No Passes found for " << user.fullname << "\n";
    cli_confirm();
    return;
  }

  const ParkingPass* active = nullptr;
  for(const auto& p : passes){
    if(p.status == ePassStatus::Active)
      active = &p;
  }

  if(active != nullptr){
    cli_table({"ID", "Issue Date", "Expiry Date","Duration", "Status"}, {{
      std::to_string(active->id), std::to_string(active->issueDate),
      std::to_string(active->issueDate), PASS_DURATION_STRING(active->duration),
      PASS_STATUS_STRING(active->status)
    }});
  } 
  else 
    cli_subheader("No Active Passes!");
  
  
  cli_separator(10);
  switch(cli_menu({"History", "Back"})){
    case 0:{
      cli_clear();
      std::vector<std::vector<std::string>> values;
      for(const auto& p: passes){
        if(p.status == ePassStatus::Active) 
          continue;
        values.push_back({
          std::to_string(p.id), std::to_string(p.issueDate), 
          PASS_DURATION_STRING(p.duration), PASS_STATUS_STRING(p.status)});
      }

      cli_table({"ID", "Issue Date", "Duration", "Status"}, values);
      cli_confirm();
      break;
    }
    case 1: 
      return;
  }
  
  FOR_LOOP_END()
}