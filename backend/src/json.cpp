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
string lobby_join(const optional<string_view>& code, optional<int> player_count,
                  optional<Character> other_confirm) {
  return encode(lobbyJoin{.success       = code.has_value(),
                          .code          = code.value_or(kEmptyStr),
                          .player_count  = player_count.value_or(0),
                          .other_confirm = other_confirm.value_or(Character::unknown)});
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

string ingame_maze(LobbyController::Maze maze, Character self, Character other) {
  const auto [startX, startY] = maze.start();
  const auto [endX, endY]     = maze.end();
  return encode(inGameMaze{
      .maze  = maze.serialize_for(self),
      .start = {startX, startY},
      .end   = {endX,   endY  },
      .cell  = maze.at(maze.start()).serialize_for(other)
  });
}

string character_move(coordinate coordinate, int16_t cell) {
  const auto [x, y] = coordinate;
  return encode(characterMove{
      .coordinate = {x, y},
        .cell = cell, .reset = false
  });
}

string character_reset() {
  return encode(characterMove{.coordinate = {}, .cell = 0, .reset = true});
}

string character_other_move(Direction direction, bool reset) {
  return encode(characterOtherMove{.direction = direction, .reset = reset});
}

string coin_taken(coordinate coordinate) {
  const auto [x, y] = coordinate;
  return encode(coinTaken{
      .coordinate = {x, y}
  });
}

string transition_to_gamedone() {
  return encode(transitionToGameDone{});
}

}  // namespace out

}  // namespace io_blair::json
