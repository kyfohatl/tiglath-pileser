#include <cmath>
#include <intercept.hpp>
#include <string>

#include "dbscan.hpp"

#define EPSILON 2500

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

  auto enemy_clusters = dbscan::dbscan(units, EPSILON, 4);

  int j = 0;
  for (auto cluster : enemy_clusters) {
    auto marker = sqf::create_marker("cluster:" + std::to_string(j++), cluster->centroid);
    sqf::set_marker_type(marker, "hd_dot");
  }

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

// This function is exported and is called by the host each frame.
void __cdecl intercept::on_frame() {
  // send it to the binary SQF sideChat command
  intercept::sqf::side_chat(sqf::player(), "onFrame");
  get_enemy_positions(sqf::get_side(sqf::player()));
}

// This function is exported and is called by the host at the end of mission initialization.
void intercept::post_init() {
  sqf::add_action(sqf::player(), "Load plugin", code("\"intercept\" callExtension (\"load_extension:tiglath-pileser\")"));
  sqf::add_action(sqf::player(), "Unload plugin", code("\"intercept\" callExtension (\"unload_extension:tiglath-pileser\")"));
  auto position = intercept::sqf::get_pos(intercept::sqf::player());
  auto marker = sqf::create_marker("referenceMarker", position);
  sqf::set_marker_shape(marker, "ELLIPSE");
  sqf::set_marker_size(marker, types::vector2(std::sqrt(EPSILON), std::sqrt(EPSILON)));
}