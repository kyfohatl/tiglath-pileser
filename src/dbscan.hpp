#pragma once

#include <vector>
#include <memory>

#include <intercept.hpp>

namespace dbscan {
  namespace types {
    enum node_type {
      CORE,
      BORDER,
      NOISE,
      UNKNOWN
    };

    struct node {
      intercept::types::object object;
      std::vector<node&> neighbors;
      node_type type = node_type::UNKNOWN;
      std::shared_ptr<cluster> allocated_cluster = nullptr;
    };

    struct cluster {
      std::vector<intercept::types::object> objects;
      intercept::types::vector2 centroid;
    };
  };

  std::vector<std::shared_ptr<types::cluster>> dbscan(std::vector<intercept::types::object> objects, float epsilon = 100.f, uint32_t min_points = 10);

  std::shared_ptr<types::cluster> form_core_cluster(types::node node, std::shared_ptr<types::cluster> parent_cluster = nullptr);
};
