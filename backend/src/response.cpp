#include "response.hpp"

#include <rfl/json.hpp>
#include <utility>

#include "character.hpp"
#include "maze.hpp"

namespace io_blair::response {

using std::string, std::string_view, std::optional, std::array;

namespace json = rfl::json;

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

string msg(string msg) {
  return json::write(Msg{.msg = std::move(msg)});
}

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

string other_join() {
  return json::write(OtherJoin{});
}

struct OtherLeave {
  string type = "otherLeave";
};

string other_leave() {
  return json::write(OtherLeave{});
}

struct Confirm {
  string type = "confirm";
  CharacterImpl confirm;
};

string confirm(Character character) {
  return json::write(Confirm{.confirm = static_cast<CharacterImpl>(character)});
}

struct RespMaze {
  string type = "maze";
  Maze::position start;
  Maze::position end;
  array<array<int16_t, Maze::kCols>, Maze::kRows> maze;
};

string maze(Maze::position start, Maze::position end,
            array<array<int16_t, Maze::kCols>, Maze::kRows> maze) {
  return json::write(
      RespMaze{.start = std::move(start), .end = std::move(end), .maze = std::move(maze)});
}

struct Move {
  string type = "move";
  string who;
  Maze::position pos;
};

string move_self(Maze::position pos) {
  return json::write(Move{.who = "self", .pos = std::move(pos)});
}

string move_other(Maze::position pos) {
  return json::write(Move{.who = "other", .pos = std::move(pos)});
}

struct Win {
  string type = "win";
};

string win() {
  return json::write(Win{});
}

struct Restart {
  string type = "restart";
};

string restart() {
  return json::write(Restart{});
}

}  // namespace io_blair::response
