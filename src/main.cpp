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

std::vector<types::object> get_known_units(types::side side) {
  return sqf::units(side);
}

void get_enemy_positions(types::side side) {
  auto units = get_known_units(side);

  auto enemy_clusters = dbscan::dbscan(units);

  auto marker_colors = std::vector<std::string>({"ColorBlack", "ColorRed", "ColorBrown", "ColorBlue", "ColorGreen", "ColorWhite", "ColorOrange"});

  for (int i = 0; i < enemy_clusters.size(); i++) {
    auto marker_color = marker_colors[i];
    auto cur_cluster = enemy_clusters[i];

    for (auto unit : cur_cluster.get()->objects) {
      auto marker = sqf::create_marker(sqf::str(unit), sqf::get_pos(unit));
      sqf::set_marker_type(marker, "hd_dot");
      sqf::set_marker_color(marker, marker_color);
    }
  }

  int i = 0;
  for (auto cluster : enemy_clusters) {
    auto marker = sqf::create_marker(std::to_string(i++), cluster.get()->centroid);
    sqf::set_marker_type(marker, "hd_dot");
  }
}

// This function is exported and is called by the host at the end of mission initialization.
void intercept::post_init() {
  get_known_units(sqf::get_side(sqf::player()));
}