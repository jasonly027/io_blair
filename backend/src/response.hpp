#pragma once

#include <format>
#include <memory>
#include <string>
#include <utility>

namespace io_blair {

namespace fields {

constexpr struct {
    const char* type = "lobby_type";
    const char* type_create = "create";
    const char* type_join = "join";
} kPrelobby;

}  // namespace fields

namespace response {

template <typename T>
[[nodiscard]] inline auto make(T&& response) {
    return std::make_shared<std::string>(std::forward<T>(response));
}

template <typename T>
[[nodiscard]] inline auto create_lobby(T&& code) {
    return make(std::format(R"({{ "code" : "{}" }})", std::forward<T>(code)));
}

}  // namespace response

}  // namespace io_blair
