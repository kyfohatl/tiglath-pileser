#include "dbscan.hpp"

#include <iostream>
#include <fstream>

namespace dbscan {
  std::vector<std::shared_ptr<types::cluster>> dbscan(std::vector<intercept::types::object> objects, float epsilon, uint32_t min_points) {
    auto all_nodes = std::vector<std::shared_ptr<types::node>>();
    auto core_nodes = std::vector<std::shared_ptr<types::node>>();

    std::ofstream log;
    log.open ("C:\\Program Files (x86)\\Steam\\steamapps\\common\\Arma 3\\@tiglath-pileser\\intercept\\log.txt", std::ios::trunc | std::ios::out);

    log << "In dbscan" << std::endl;

    for (auto object : objects) {
      auto node = std::make_shared<types::node>();
      node->object = object;
      all_nodes.push_back(node);
    }

    // Find all neighbors (within epsilon distance) of each node and label CORE points
    for (auto node : all_nodes) {
      auto object_position = intercept::sqf::get_pos(node->object);
      log << "node round 1: " << intercept::sqf::str(node->object) << "pos: " << intercept::sqf::str(object_position) << std::endl;

      for (auto other_node : all_nodes) {
        auto other_node_position = intercept::sqf::get_pos(other_node->object);
        auto distance = object_position.distance_2d_squared(other_node_position);

        if (distance <= epsilon) {
          node->neighbors.push_back(other_node);
        }
      }

      // By the official DBSCAN algorithm, the point itself should be counted as the neighbor total
      auto neighbors_size = node->neighbors.size() + 1;

      if (neighbors_size >= min_points) {
        node->type = types::node_type::CORE;
        core_nodes.push_back(node);
      }
    }

    for (auto node : all_nodes) {
      log << "node round 2: " << intercept::sqf::str(node->object) << ", neighors size: " << node->neighbors.size() << std::endl;

      if (node->type != types::node_type::CORE) {
        for (auto other_node : node->neighbors) {
          if (other_node->type == types::node_type::CORE) {
            node->type = types::node_type::BORDER;
            break;
          }
        }

        if (node->type != types::node_type::BORDER) {
          node->type = types::node_type::NOISE;
        }
      }
    }

    auto clusters = std::vector<std::shared_ptr<types::cluster>>();

    // Core points within epsilon distance of each other must be grouped into the same cluster
    for (auto node : core_nodes) {
      if (node->allocated_cluster != nullptr) {
        continue;
      }

      auto cluster = form_core_cluster(node, nullptr);
      clusters.push_back(cluster);
    }

    // Border points must then be allocated to a neighboring cluster
    // Currently border points are just allocated to the first neighboring cluster
    for (auto node : all_nodes) {
      if (node->type == types::node_type::BORDER) {
        for (auto neighbor : node->neighbors) {
          if (neighbor->type == types::node_type::CORE) {
            node->allocated_cluster = neighbor->allocated_cluster;
            node->allocated_cluster->objects.push_back(node->object);

            auto object_position = intercept::sqf::get_pos(node->object);
            node->allocated_cluster->centroid += intercept::types::vector2(object_position);

            break;
          }
        }
      }
    }

    // Finally, we must calculate the centroid for each cluster
    for (auto cluster : clusters) {
      cluster->centroid /= static_cast<float>(cluster->objects.size());
    }

    log.close();

    return clusters;
  }


  // Forms a cluster using the given core point and all connected neighboring and neighbors of neighboring core points
  // and returns the result
  std::shared_ptr<types::cluster> form_core_cluster(std::shared_ptr<types::node> node, std::shared_ptr<types::cluster> parent_cluster) {
    // Check if node has already been visited
    if (node->allocated_cluster != nullptr) {
      return node->allocated_cluster;
    }

    // If no cluster has been given, create one
    auto cluster = parent_cluster;
    auto object_position = intercept::types::vector2(intercept::sqf::get_pos(node->object));
    if (cluster == nullptr) {
      cluster = std::make_shared<types::cluster>();
      cluster->centroid = object_position;
    } else {
      cluster->centroid += object_position;
    }

    node->allocated_cluster = cluster;
    cluster->objects.push_back(node->object);

    // All neighboring core points must also be allocated to the same cluster
    for (auto neighbor : node->neighbors) {
      if (neighbor->type == types::node_type::CORE) {
        form_core_cluster(neighbor, cluster);
      }
    }

    return cluster;
  }
};