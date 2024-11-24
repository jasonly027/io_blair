#pragma once

#include <memory>
#include <string>
namespace io_blair {
namespace response {

namespace fields {

constexpr struct {
    const char* type = "lobby_type";
    const char* type_create = "create";
    const char* type_join = "join";
} kPrelobby;

}  // namespace fields

inline auto make(const char* const response) {
    return std::make_shared<std::string>(response);
}

}  // namespace response
}  // namespace io_blair
