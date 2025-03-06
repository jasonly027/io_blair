#include "string_hash.hpp"

namespace io_blair {

std::size_t StringHash::operator()(const char* str) const {
  return hash_type{}(str);
}

std::size_t StringHash::operator()(std::string_view str) const {
  return hash_type{}(str);
}

std::size_t StringHash::operator()(const std::string& str) const {
  return hash_type{}(str);
}

}  // namespace io_blair
