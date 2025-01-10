#include "maze.hpp"

#include <cassert>
#include <random>
#include <stack>
#include <tuple>
#include <vector>

#include "character.hpp"

namespace io_blair {

using std::random_device, std::mt19937, std::uniform_int_distribution, std::array, std::vector,
    std::stack, std::tuple, std::optional;
using position = Maze::position;

namespace ranges = std::ranges;

namespace {
thread_local mt19937 mt{random_device{}()};

// Get Directions in a uniformly random order
array<Direction, 4> get_random_dirs() {
  using Dir = Direction;
  array<Dir, 4> res{Dir::kUp, Dir::kRight, Dir::kDown, Dir::kLeft};
  ranges::shuffle(res, mt);
  return res;
}

// 10% chance to return IO
// 10% chance to return Blair
// 80% chance to return Unset
Character get_random_char() {
  thread_local uniform_int_distribution dist(1, 10);
  int rand = dist(mt);

  if (rand == 1) {
    return Character::kIO;
  }

  if (rand == 2) {
    return Character::kBlair;
  }

  return Character::kUnset;
}

// 10% chance to return true
bool get_random_coin() {
  thread_local uniform_int_distribution dist(1, 10);
  return dist(mt) == 1;
}

}  // namespace

Maze Maze::generate_maze() {
  Maze maze;

  /*
    A tuple of:
    - position of "this" cell
    - idx of which dir to access in the array (the 3rd tuple element)
    - an array of all the dirs, randomly shuffled
  */
  using pos_idx_arr = tuple<position, int, array<Direction, 4>>;

  // Iterative version of Recursive Backtracking for Maze gen
  stack<pos_idx_arr, vector<pos_idx_arr>> stack;
  stack.emplace(position{0, 0}, 0, get_random_dirs());

  while (!stack.empty()) {
    auto [pos, idx, arr] = stack.top();
    stack.pop();

    optional<position> neighbor = get_neighbor(pos, arr[idx]);
    // If valid, unmodified neighbor, create a path between them
    if (neighbor.has_value() && !maze[*neighbor].any_path()) {
      switch (get_random_char()) {
        // Allow both to see newly added path
        case Character::kUnset:
          maze.cell(pos).set_path_both(arr[idx], true);
          maze.cell(*neighbor).set_path_both(get_opposite(arr[idx]), true);
          break;
        // Allow only IO to see newly added path
        case Character::kIO:
          maze.cell(pos).set_path_io(arr[idx], true);
          maze.cell(*neighbor).set_path_io(get_opposite(arr[idx]), true);
          break;
        // Allow only Blair to see newly added path
        case Character::kBlair:
          maze.cell(pos).set_path_blair(arr[idx], true);
          maze.cell(*neighbor).set_path_blair(get_opposite(arr[idx]), true);
          break;
      }

      // Push this cell for later if there are still
      // dirs in the array to try
      if (idx + 1 < arr.size()) {
        stack.emplace(pos, idx + 1, arr);
      }
      stack.emplace(*neighbor, 0, get_random_dirs());
    }
    // The neighbor was invalid, so we check the next dir
    // in our randomly-generated sequence
    else if (idx + 1 < arr.size()) {
      stack.emplace(pos, idx + 1, arr);
    }

    // The roll to see if this cell should have a coin should
    // only happen once, so this only evaluates when this cell
    // is permanently leaving the stack
    if (idx + 1 >= arr.size()) {
      maze.cell(pos).set_coin(get_random_coin());
    }
  }

  return maze;
}

}  // namespace io_blair
