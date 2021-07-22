#include <intercept.hpp>
#include <string>

#include "dbscan.hpp"

using namespace intercept;

int intercept::api_version() { //This is required for the plugin to work.
  return INTERCEPT_SDK_API_VERSION;
}

void intercept::register_interfaces() {
}

void intercept::pre_start() {
}

void intercept::pre_init() {
  sqf::system_chat("The Intercept template plugin is running!");
}

// void display_units_on_map() {
//   std::vector<types::object> all_units = sqf::all_units();
//   for (auto unit : all_units) {
//     auto pos_matrix = ((game_data_object*) unit.data.getRef())->get_position_matrix();
//     auto marker = sqf::create_marker(sqf::str(unit), pos_matrix._position);
//     sqf::set_marker_type(marker, "hd_dot");
//   }
// }

std::vector<types::object> get_known_units(types::side side) {
  return sqf::units(side);
}

void get_enemy_positions(types::side side) {
  auto units = get_known_units(side);

  auto enemy_clusters = dbscan::dbscan(units);

  int i = 0;
  for (auto cluster : enemy_clusters) {
    auto marker = sqf::create_marker(std::to_string(i++), cluster.get()->centroid);
    sqf::set_marker_type(marker, "hd_dot");
  }
}

// This function is exported and is called by the host at the end of mission initialization.
void intercept::post_init() {
  // display_units_on_map();
  get_known_units(sqf::get_side(sqf::player()));
}