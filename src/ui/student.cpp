#include "elements.hpp"
#include "terminal/cli.hpp"


static void ui_add_vehicle();
static void ui_delete_vehicle(const std::vector<Vehicle>& owned);
static void ui_vehicles(const User &user);

void ui_student() {
  cli_clear();
  cli_header("Welocme " << gCurrentUser.fullname << '!');

  switch (cli_menu({"Student Profile", "Registered Vehicles", "Exit"})) {
    case 0:
      ui_profile(gCurrentUser);
      break;
    case 1:
      ui_vehicles(gCurrentUser);
      break;
    case 2:
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

  std::string plate = cli_input_valid<std::string>("License Plate: ", [](std::string_view in, std::string& out){
    if(in.size() == 7 && std::isdigit(in[3]) && std::isdigit(in[4]) && std::isdigit(in[5]) && std::isdigit(in[6])){
      out = in;
      return "";
    }

    return "Please enter a valid license plate Number!";
  });

  std::hash<std::string> hasher;
  int id = static_cast<int>(hasher(plate)) + gCurrentUser.id;
  auto index = db_find_by_id<Vehicle>(id, nullptr);
  if(index != ENTRY_NOT_FOUND){
    cli_error("Vehicle already Exists!");
    cli_confirm();
    return;
  }

  std::string model = cli_input_valid<std::string>("Model (company name.): ", [](std::string_view in, std::string& out){
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
/*








void ui_vehicle_details(const Vehicle& vh){
  cli_header(vh.LicensePlate);
  cli_field("Owner", vh.Owner);
  cli_field("Model", vh.Model);
}

void ui_edit_vehicle(const Vehicle& vh){}

void ui_user_applications(const User& user){}

void ui_view_user_pass(const User& user){}


*/