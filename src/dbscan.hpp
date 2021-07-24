#pragma once

#include <vector>
#include <memory>
#include <functional>

#include <intercept.hpp>

namespace dbscan {
  namespace types {
    enum class node_type : uint8_t {
      CORE,
      BORDER,
      NOISE,
      UNKNOWN
    };

    struct cluster {
      std::vector<intercept::types::object> objects;
      intercept::types::vector2 centroid;
    };

    struct node {
      intercept::types::object object;
      std::vector<std::reference_wrapper<node>> neighbors;
      node_type type = node_type::UNKNOWN;
      std::shared_ptr<cluster> allocated_cluster = nullptr;
    };
  };

  std::vector<std::shared_ptr<types::cluster>> dbscan(std::vector<intercept::types::object> objects, float epsilon = 2500.f, uint32_t min_points = 4);

  std::shared_ptr<types::cluster> form_core_cluster(types::node node, std::shared_ptr<types::cluster> parent_cluster = nullptr);
};
