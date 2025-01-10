#pragma once

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

using name      = const char*;
using value_str = const char*;

// NOLINTBEGIN(readability-identifier-naming)

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

} SharedState;

constexpr struct {
  const struct {
    value_str create = "create";
    value_str join   = "join";
  } type;

  // Should exist if type is join
  // The join code of the lobby
  const struct {
    name _ = "code";
  } code;

} Prelobby;

constexpr struct {
  const struct {
    value_str hover   = "hover";
    value_str confirm = "confirm";
  } type;

  // Should exist if type is hover
  // Which character self hovered
  const struct {
    name _ = "hover";

    const CharacterImpl io    = static_cast<CharacterImpl>(Character::kIO);
    const CharacterImpl blair = static_cast<CharacterImpl>(Character::kBlair);
  } hover;

  // Should exist if type is confirm
  // Which character self confirmed
  const struct {
    name _ = "confirm";

    const CharacterImpl unset = static_cast<CharacterImpl>(Character::kUnset);
    const CharacterImpl io    = static_cast<CharacterImpl>(Character::kIO);
    const CharacterImpl blair = static_cast<CharacterImpl>(Character::kBlair);
  } confirm;

} CharacterSelect;

constexpr struct {
  const struct {
    value_str move = "move";
  } type;

  // Should exist if type is move
  // The row self is trying to move to
  const struct {
    name _ = "row";

    const int64_t value = 0;  // [0, Maze::kRows)
  } row;

  // Should exist if type is move
  // The col self is trying to move to
  const struct {
    name _ = "col";

    const int64_t value = 0;  // [0, Maze::kCols)
  } col;

} InGame;

constexpr struct {
  const struct {
    value_str restart = "restart";
  } type;
} GameFinished;

// NOLINTEND(readability-identifier-naming)

}  // namespace io_blair::request
