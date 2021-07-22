#include "dbscan.hpp"

namespace dbscan {
  std::vector<std::shared_ptr<types::cluster>> get_clusters(std::vector<intercept::types::object> objects, float epsilon, int min_points) {
    auto all_nodes = std::vector<types::node>();
    auto core_nodes = std::vector<types::node&>();

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
        for (auto other_node : node.neighbors) {
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

    for (auto node : all_nodes) {
      if (node.type == types::node_type::CORE || node.type == types::node_type::BORDER) {
        if (node.allocated_cluster != nullptr) {
          continue;
        }

        auto cluster = form_core_cluster(node, nullptr);

        cluster.get()->centroid /= cluster.get()->objects.size();

        clusters.push_back(cluster);
      }
    }

    return clusters;
  }

  std::shared_ptr<types::cluster> form_core_cluster(types::node node, std::shared_ptr<types::cluster> parent_cluster) {
    if (node.allocated_cluster != nullptr) {
      return node.allocated_cluster;
    }

    auto cluster = parent_cluster;
    if (cluster == nullptr) {
      cluster = std::make_shared<types::cluster>();
    }

    node.allocated_cluster = cluster;

    cluster.get()->objects.push_back(node.object);

    auto object_position = ((game_data_object*) node.object.data.getRef())->get_position_matrix()._position;
    cluster.get()->centroid += intercept::types::vector2(object_position);
  }
};