#include "response.hpp"

#include <cassert>
#include <rfl/json.hpp>
#include <utility>

#include "character.hpp"
#include "rfl/json/write.hpp"

using std::string, std::string_view, std::optional;
namespace json = rfl::json;

namespace io_blair::response {

struct Join {
    string type = "join";
    optional<string> code;
};

string join(optional<string> code) {
    return json::write(Join{.code = std::move(code)});
}

struct Msg {
    string type = "msg";
    string msg;
};

string msg(string msg) { return json::write(Msg{.msg = std::move(msg)}); }

struct Hover {
    string type = "hover";
    CharacterImpl hover;
};

string hover(Character character) {
    return json::write(Hover{.hover = static_cast<CharacterImpl>(character)});
}

struct ConfirmOther {
    string type = "confirm";
    CharacterImpl character;
};

string confirm(Character character) {
    return json::write(
        ConfirmOther{.character = static_cast<CharacterImpl>(character)});
}

}  // namespace io_blair::response
