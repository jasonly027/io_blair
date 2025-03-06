/**
 * @file string_hash.hpp
 */
#pragma once

#include <functional>
#include <string>
#include <string_view>

namespace io_blair {

/**
 * @brief Transparent hashing for heterogeneous lookup for unordered containers.
 */
struct StringHash {
  using hash_type      = std::hash<std::string_view>;
  using is_transparent = void;

  std::size_t operator()(const char* str) const;
  std::size_t operator()(std::string_view str) const;
  std::size_t operator()(const std::string& str) const;
};

}  // namespace io_blair
