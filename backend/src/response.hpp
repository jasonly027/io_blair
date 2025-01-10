#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>

#include "character.hpp"
#include "maze.hpp"

namespace io_blair::response {

std::string join(std::optional<std::string> code = std::nullopt);

std::string msg(std::string msg);

std::string hover(Character character);

std::string other_join();

std::string other_leave();

std::string confirm(Character character);

std::string maze(Maze::position start, Maze::position end,
                 std::array<std::array<int16_t, Maze::kCols>, Maze::kRows> maze);

std::string move_self(Maze::position pos);

std::string move_other(Maze::position pos);

std::string win();

std::string restart();

}  // namespace io_blair::response
