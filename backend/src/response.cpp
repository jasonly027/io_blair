#include "response.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <rfl/json.hpp>
#include <utility>

#include "character.hpp"
#include "maze.hpp"
#include "rfl/json/write.hpp"

using std::string, std::string_view, std::optional, std::array;
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

struct OtherJoin {
    string type = "otherJoin";
};

string other_join() { return json::write(OtherJoin{}); }

struct OtherLeave {
    string type = "otherLeave";
};

string other_leave() { return json::write(OtherLeave{}); }

struct Confirm {
    string type = "confirm";
    CharacterImpl confirm;
};

string confirm(Character character) {
    return json::write(
        Confirm{.confirm = static_cast<CharacterImpl>(character)});
}

struct Maze {
    string type = "maze";
    io_blair::Maze::position start;
    io_blair::Maze::position end;
    array<array<int8_t, io_blair::Maze::kCols>, io_blair::Maze::kRows> maze;
};

string maze(
    io_blair::Maze::position start, io_blair::Maze::position end,
    array<array<int8_t, io_blair::Maze::kCols>, io_blair::Maze::kRows> maze) {
    return json::write(Maze{.start = std::move(start),
                            .end = std::move(end),
                            .maze = std::move(maze)});
}

}  // namespace io_blair::response
