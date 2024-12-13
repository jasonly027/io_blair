#pragma once

#include <optional>
#include <string>

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
using value = const char*;

constexpr struct {
    const struct {
        name _ = "type";

        // Possible in all states that aren't Prelobby
        value chat = "chat";

        // Possible in all states that aren't Prelobby
        value leave = "leave";
    } type;

    // Should exist if type is chat
    // The msg that should be forwarded to other player
    const struct {
        name _ = "msg";
    } msg;

} SharedState;  // NOLINT(readability-identifier-naming)

constexpr struct {
    const struct {
        value create = "create";
        value join = "join";
    } type;

    // Should exist if type is join
    // The join code of the lobby
    const struct {
        name _ = "code";
    } code;

} Prelobby;  // NOLINT(readability-identifier-naming)

constexpr struct {
    const struct {
        value hover = "hover";
        value confirm = "confirm";
    } type;

    // Should exist if type is hover
    // Which character self hovered
    const struct {
        name _ = "hover";

        name io = "io";
        name blair = "blair";
    } hover;

    // Should exist if type is confirm
    // Which character self confirmed
    const struct {
        name _ = "confirm";

        value io = "io";
        value blair = "blair";
    } confirm;

} CharacterSelect;  // NOLINT(readability-identifier-naming)

}  // namespace request

namespace response {

std::string join(bool success, std::optional<std::string> code = std::nullopt);

std::string chat(std::string msg);

}  // namespace response

}  // namespace io_blair
