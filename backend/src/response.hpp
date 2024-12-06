#pragma once

#include <format>
#include <string>
#include <utility>

namespace io_blair {

namespace fields {

constexpr struct {
    const char* type = "lobby_type";
    const char* type_create = "create";
    const char* type_join = "join";
    const char* join_code = "code";

} kPrelobby;

}  // namespace fields

namespace response {

template <typename T>
[[nodiscard]] inline auto create_lobby(T&& code) {
    return std::format(R"({{ "code" : "{}" }})", std::forward<T>(code));
}

inline std::string join(bool success) {
    return std::format(R"({{ "join" : "{}" }})", success ? "success" : "fail");
}

}  // namespace response

}  // namespace io_blair
