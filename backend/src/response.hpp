#pragma once

#include <optional>
#include <string>

namespace io_blair {

namespace request {

constexpr struct {
    const char* type = "lobbyType";
    const char* type_create = "create";
    const char* type_join = "join";
    const char* join_code = "code";

} kPrelobby;

}  // namespace request

namespace response {

std::string join(bool success, std::optional<std::string> code = std::nullopt);

}  // namespace response

}  // namespace io_blair
