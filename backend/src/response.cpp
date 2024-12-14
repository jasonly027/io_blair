#include "response.hpp"

#include <rfl/json.hpp>
#include <type_traits>
#include <utility>

using std::string, std::string_view, std::optional, std::underlying_type_t;
namespace json = rfl::json;

namespace io_blair {

namespace response {

struct Join {
    string type = "join";
    bool success;
    optional<string> code;
};

string join(bool success, optional<string> code) {
    assert(success == code.has_value() &&
           "Either success alongside a code or fail by itself");
    return json::write(Join{.success = success, .code = std::move(code)});
}

struct Chat {
    string type = "chat";
    string msg;
};

string chat(string msg) { return json::write(Chat{.msg = std::move(msg)}); }

struct Hover {
    string type = "hover";
    underlying_type_t<Character> hover;
};

string hover(Character character) {
    return json::write(
        Hover{.hover = static_cast<underlying_type_t<Character>>(character)});
}

}  // namespace response
}  // namespace io_blair
