#include "dbscan.hpp"

namespace dbscan {
  std::vector<std::shared_ptr<types::cluster>> dbscan(std::vector<intercept::types::object> objects, float epsilon, uint32_t min_points) {
    auto all_nodes = std::vector<types::node>();
    auto core_nodes = std::vector<std::reference_wrapper<types::node>>();

    for (auto object : objects) {
      all_nodes.push_back({ object });
    }

    // Find all neighbors (within epsilon distance) of each node and label CORE points
    for (auto node : all_nodes) {
      auto object_position = ((game_data_object*) node.object.data.getRef())->get_position_matrix()._position;

      for (auto other_node : all_nodes) {
        auto other_node_position = ((game_data_object*) other_node.object.data.getRef())->get_position_matrix()._position;
        auto distance = object_position.distance_2d_squared(other_node_position);

        if (distance <= epsilon) {
          node.neighbors.push_back(other_node);
        }
      }

      // By the official DBSCAN algorithm, the point itself should be counted as the neighbor total
      auto neighbors_size = node.neighbors.size() + 1;

      if (neighbors_size >= min_points) {
        node.type = types::node_type::CORE;
        core_nodes.push_back(node);
      }
    }

    for (auto node : all_nodes) {
      if (node.type != types::node_type::CORE) {
        for (types::node other_node : node.neighbors) {
          if (other_node.type == types::node_type::CORE) {
            node.type = types::node_type::BORDER;
            break;
          }
        }

        if (node.type != types::node_type::BORDER) {
          node.type = types::node_type::NOISE;
        }
      }
    }

    auto clusters = std::vector<std::shared_ptr<types::cluster>>();

    // Core points within epsilon distance of each other must be grouped into the same cluster
    for (types::node node : core_nodes) {
      if (node.allocated_cluster != nullptr) {
        continue;
      }

      auto cluster = form_core_cluster(node, nullptr);
      clusters.push_back(cluster);
    }

    // Border points must then be allocated to a neighboring cluster
    // Currently border points are just allocated to the first neighboring cluster
    for (auto node : all_nodes) {
      if (node.type == types::node_type::BORDER) {
        for (types::node neighbor : node.neighbors) {
          if (neighbor.type == types::node_type::CORE) {
            node.allocated_cluster = neighbor.allocated_cluster;
            node.allocated_cluster.get()->objects.push_back(node.object);

            auto object_position = ((game_data_object*) node.object.data.getRef())->get_position_matrix()._position;
            node.allocated_cluster.get()->centroid += intercept::types::vector2(object_position);

            break;
          }
        }
      }
    }

    // Finally, we must calculate the centroid for each cluster
    for (auto cluster : clusters) {
      cluster.get()->centroid /= static_cast<float>(cluster.get()->objects.size());
    }

    return clusters;
  }


  // Forms a cluster using the given core point and all connected neighboring and neighbors of neighboring core points
  // and returns the result
  std::shared_ptr<types::cluster> form_core_cluster(types::node node, std::shared_ptr<types::cluster> parent_cluster) {
    // Check if node has already been visited
    if (node.allocated_cluster != nullptr) {
      return node.allocated_cluster;
    }

    // If no cluster has been given, create one
    auto cluster = parent_cluster;
    auto object_position = intercept::types::vector2(((game_data_object*) node.object.data.getRef())->get_position_matrix()._position);
    if (cluster == nullptr) {
      cluster = std::make_shared<types::cluster>();
      cluster.get()->centroid = object_position;
    } else {
      cluster.get()->centroid += object_position;
    }

    node.allocated_cluster = cluster;
    cluster.get()->objects.push_back(node.object);

    // All neighboring core points must also be allocated to the same cluster
    for (types::node neighbor : node.neighbors) {
      if (neighbor.type == types::node_type::CORE) {
        form_core_cluster(neighbor, cluster);
      }
    }

    return cluster;
  }
};