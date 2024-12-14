#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include "character.hpp"

namespace io_blair {

namespace request {

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
        value_str chat = "chat";

        // Possible in all states that aren't Prelobby
        value_str leave = "leave";
    } type;

    // Should exist if type is chat
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

        using CharImpl = std::underlying_type_t<Character>;

        const uint8_t io =
            static_cast<CharImpl>(Character::kIO);
        const uint8_t blair =
            static_cast<CharImpl>(Character::kBlair);
    } hover;

    // Should exist if type is confirm
    // Which character self confirmed
    const struct {
        name _ = "confirm";

        using CharImpl = std::underlying_type_t<Character>;

        const uint8_t io =
            static_cast<CharImpl>(Character::kIO);
        const uint8_t blair =
            static_cast<CharImpl>(Character::kBlair);
    } confirm;

} CharacterSelect;  // NOLINT(readability-identifier-naming)

}  // namespace request

namespace response {

std::string join(bool success, std::optional<std::string> code = std::nullopt);

std::string chat(std::string msg);

std::string hover(Character character);

}  // namespace response

}  // namespace io_blair
