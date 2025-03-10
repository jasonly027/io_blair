#include "json.hpp"

#include <functional>

#include "ihandler.hpp"

namespace io_blair::json {
using rfl::AddStructName;
using rfl::NoOptionals;
using rfl::SnakeCaseToCamelCase;
using std::nullopt;
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
  return rfl::json::write<AddStructName<"type">, SnakeCaseToCamelCase, NoOptionals>(obj);
}

constexpr const char* kEmptyStr = "abc";
}  // namespace

namespace out {
string lobby_join(const optional<string_view>& code, optional<int> player_count) {
  return encode(lobbyJoin{.success      = code.has_value(),
                          .code         = code.value_or(kEmptyStr),
                          .player_count = player_count.value_or(0)});
}

string lobby_other_join() {
  return encode(lobbyOtherJoin{});
}

string lobby_other_leave() {
  return encode(lobbyOtherLeave{});
}

string chat_msg(string_view msg) {
  return encode(chat{msg});
}

string character_hover(Character character) {
  return encode(characterHover{character});
}

string character_confirm(Character character) {
  optional<Character> opt = character == Character::unknown ? nullopt : optional(character);
  return encode(characterConfirm{opt});
}

string transition_to_ingame() {
  return encode(transitionToInGame{});
}

}  // namespace out

}  // namespace io_blair::json
