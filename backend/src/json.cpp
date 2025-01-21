#include "json.hpp"

#include <functional>

#include "ihandler.hpp"


namespace io_blair::json {
using std::optional;
using std::string;
using std::string_view;

void decode(const string& data, IHandler& handler) {
  if (auto res = rfl::json::read<json::in::AllJsonTypes>(data); res) {
    (*res).visit([&](const auto& decoded) { std::ref(handler)(decoded); });
  }
}

namespace {
string encode(const auto& obj) {
  using Processors = rfl::
      Processors<rfl::AddStructName<"type">, rfl::SnakeCaseToCamelCase, rfl::UnderlyingEnums>;
  return rfl::json::write<Processors>(obj);
}

}  // namespace

namespace out {
string lobby_join(const optional<string_view>& code) {
  return encode(lobbyJoin{.success = code.has_value(), .code = code.value_or("")});
}

string lobby_other_join() {
  return encode(lobbyOtherJoin{});
}

string lobby_other_leave() {
  return encode(lobbyOtherLeave{});
}
}  // namespace out

}  // namespace io_blair::json
