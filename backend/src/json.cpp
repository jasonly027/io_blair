#include "json.hpp"

#include <functional>

#include "ihandler.hpp"


namespace io_blair::json {
void decode(const std::string& data, IHandler& handler) {
  if (auto res = rfl::json::read<json::in::AllJsonTypes>(data); res) {
    (*res).visit([&](const auto& decoded) { std::ref(handler)(decoded); });
  }
}

namespace {
template <typename T>
std::string encode(T obj) {
  using Processors = rfl::
      Processors<rfl::AddStructName<"type">, rfl::SnakeCaseToCamelCase, rfl::UnderlyingEnums>;
  return rfl::json::write<Processors>(obj);
}

}  // namespace

namespace out {
std::string lobby_join(const std::optional<std::string_view>& code) {
  return encode(lobbyJoin{.success = code.has_value(), .code = code.value_or("")});
}

}  // namespace out

}  // namespace io_blair::json
