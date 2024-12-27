#pragma once

#include <cstdint>

#include "character.hpp"

namespace io_blair::request {

/*

The following structs reflect how incoming JSON
should be structured.

Each top-level struct is the expected JSON structure
at that game state, e.g., the PreLobby struct is how
the JSON should be structured when the Game is at
the state State::kPreLobby.

Each member struct within a top-level struct explains
a name-value pair. <member struct>._ is the "name"
of that pair. Every other field is a possible "value".

Top-level struct SharedState contains pairs that may be
seen in more than one state

Expected JSON described by Prelobby:

{
    "type": "create"
}

or

{
    "type": "join"
    "code": "<join_code here>"
}

*/

using name = const char*;
using value_str = const char*;

constexpr struct {
  const struct {
    name _ = "type";

    // Possible in all states that aren't Prelobby
    value_str msg = "msg";

    // Possible in all states that aren't Prelobby
    value_str leave = "leave";
  } type;

  // Should exist if type is msg
  // The msg that should be forwarded to other player
  const struct {
    name _ = "msg";
  } msg;

} SharedState;  // NOLINT(readability-identifier-naming)

constexpr struct {
  const struct {
    value_str create = "create";
    value_str join = "join";
  } type;

  // Should exist if type is join
  // The join code of the lobby
  const struct {
    name _ = "code";
  } code;

} Prelobby;  // NOLINT(readability-identifier-naming)

constexpr struct {
  const struct {
    value_str hover = "hover";
    value_str confirm = "confirm";
  } type;

  // Should exist if type is hover
  // Which character self hovered
  const struct {
    name _ = "hover";

    const int8_t io = static_cast<CharacterImpl>(Character::kIO);
    const int8_t blair = static_cast<CharacterImpl>(Character::kBlair);
  } hover;

  // Should exist if type is confirm
  // Which character self confirmed
  const struct {
    name _ = "confirm";

    const int8_t unset = static_cast<CharacterImpl>(Character::kUnset);
    const int8_t io = static_cast<CharacterImpl>(Character::kIO);
    const int8_t blair = static_cast<CharacterImpl>(Character::kBlair);
  } confirm;

} CharacterSelect;  // NOLINT(readability-identifier-naming)

constexpr struct {
  const struct {
    value_str move = "move";
  } type;

  const struct {
    name _ = "move";

    value_str up = "up";
    value_str right = "right";
    value_str down = "down";
    value_str left = "left";
  } move;

} InGame;  // NOLINT(readability-identifier-naming)

}  // namespace io_blair::request
