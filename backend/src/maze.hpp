#pragma once

#include <array>
#include <bitset>
#include <cstdint>
#include <optional>
#include <type_traits>
#include <utility>

namespace io_blair {

enum class Direction : int8_t { kUp = 0, kRight = 1, kDown = 2, kLeft = 3 };

constexpr Direction get_opposite(Direction dir) {
  switch (dir) {
    case Direction::kUp:    return Direction::kDown;
    case Direction::kRight: return Direction::kLeft;
    case Direction::kDown:  return Direction::kUp;
    case Direction::kLeft:  return Direction::kRight;
  }
}

class Cell {
 public:
  // IO or Blair can see a path up
  constexpr bool up() const {
    return bits_[kOffsetToIO + kOffsetToUp] || bits_[kOffsetToBlair + kOffsetToUp];
  }

  // IO or Blair can see a path right
  constexpr bool right() const {
    return bits_[kOffsetToIO + kOffsetToRight] || bits_[kOffsetToBlair + kOffsetToRight];
  }

  // IO or Blair can see a path down
  constexpr bool down() const {
    return bits_[kOffsetToIO + kOffsetToDown] || bits_[kOffsetToBlair + kOffsetToDown];
  }

  // IO or Blair can see a path left
  constexpr bool left() const {
    return bits_[kOffsetToIO + kOffsetToLeft] || bits_[kOffsetToBlair + kOffsetToLeft];
  }

  // True if any of up, right, down
  // or left are true
  constexpr bool any_path() const {
    return up() || right() || down() || left();
  }

  constexpr void set_path_both(Direction dir, bool value) {
    set_path_io(dir, value);
    set_path_blair(dir, value);
  }

  constexpr void set_path_io(Direction dir, bool value) {
    bits_[kOffsetToIO + static_cast<std::underlying_type_t<Direction>>(dir)] = value;
  }

  constexpr void set_path_blair(Direction dir, bool value) {
    bits_[kOffsetToBlair + static_cast<std::underlying_type_t<Direction>>(dir)] = value;
  }

  constexpr void clear_paths_both() {
    clear_paths_io();
    clear_paths_blair();
  }

  constexpr void clear_paths_io() {
    for (int offset = 0; offset < kDirections; ++offset) {
      bits_[offset + kOffsetToIO] = false;
    }
  }

  constexpr void clear_paths_blair() {
    for (int offset = 0; offset < kDirections; ++offset) {
      bits_[offset + kOffsetToBlair] = false;
    }
  }

  constexpr bool coin() const {
    return bits_[kOffsetToCoin];
  }

  constexpr void set_coin(bool value) {
    bits_[kOffsetToCoin] = value;
  }

  // First 4 bits are what both can see
  // Second 4 bits are what IO exclusively sees
  // 9th bit is coin
  constexpr int16_t serialize_io() const {
    return serialize_impl(kOffsetToIO, kOffsetToBlair);
  }

  // First 4 bits are what both can see
  // Second 4 bits are what Blair exclusively sees
  // 9th bit is coin
  constexpr int16_t serialize_blair() const {
    return serialize_impl(kOffsetToBlair, kOffsetToIO);
  }

 private:
  constexpr int16_t serialize_impl(int offset_to_target_char, int offset_to_nontarget_char) const {
    std::bitset<9> res;

    for (int offset = 0; offset < kDirections; ++offset) {
      // Paths both can see
      res[offset] = bits_[offset + kOffsetToIO] && bits_[offset + kOffsetToBlair];
      // Paths only Blair can see
      res[offset + kOffsetToExclusivePath]
          = bits_[offset + offset_to_target_char] && !bits_[offset + offset_to_nontarget_char];
    }
    res[kOffsetToCoin] = bits_[kOffsetToCoin];

    return static_cast<int16_t>(res.to_ulong());
  }

  // clang-format off
    /*
        Paths are represented by 4 bits:
        up, right, down, left

        An ON bit means theres a passage in that direction
        to a cell, in other words, there is no wall obstructing
        traversal. It is implied that that destination cell
        has a passage back to the original cell, in other words,
        the destination cell's bit for the opposite direction is
        guaranteed ON.

        Example:
        For a cell of 0001 (Only up bit is ON),
        The cell up of this cell should
        AT LEAST be X1XX (Down bit is ON, X means unknown)
    */
  // clang-format on

  // First 4 bits are paths IO sees
  // Second 4 bits are paths Blair sees
  // 9th bit is coin
  std::bitset<9> bits_;

  static constexpr int kOffsetToUp = 0;
  static constexpr int kOffsetToRight = 1;
  static constexpr int kOffsetToDown = 2;
  static constexpr int kOffsetToLeft = 3;
  static constexpr int kDirections = 4;

  static constexpr int kOffsetToIO = 0;
  static constexpr int kOffsetToBlair = 4;
  // Used for the second 4 bits of serialization
  static constexpr int kOffsetToExclusivePath = 4;

  static constexpr int kOffsetToCoin = 8;
};

class Maze {
 public:
  using position = std::pair<int, int>;

  // Generate a random maze
  static Maze generate_maze();

  static constexpr int kRows = 16;
  static constexpr int kCols = 16;
  static constexpr int kTotalDim = kRows * kCols;

  // Get the position in direction (dir) from (pos)
  // Returns nullopt if it would be out of bounds from the maze
  static constexpr std::optional<position> get_neighbor(position pos, Direction dir) {
    auto& [row, col] = pos;
    switch (dir) {
      case Direction::kUp:    --row; break;
      case Direction::kRight: ++col; break;
      case Direction::kDown:  ++row; break;
      case Direction::kLeft:  --col; break;
    }

    if (row >= 0 && row < kRows && col >= 0 && col < kCols) {
      return pos;
    }
    return std::nullopt;
  }

  constexpr const std::array<Cell, kCols>& operator[](size_t row) const {
    return matrix_[row];
  }

  constexpr const Cell& operator[](position pos) const {
    return matrix_[pos.first][pos.second];
  }

  // Where to start in the maze
  constexpr position start() const {
    return start_;
  }

  constexpr bool is_start(position pos) const {
    return pos == start_;
  }

  // Where to end in the maze
  constexpr position end() const {
    return end_;
  }

  constexpr bool is_end(position pos) const {
    return pos == end_;
  }

  // Will serialize with only io_path and coin per cell
  constexpr std::array<std::array<int16_t, kCols>, kRows> serialize_io() const {
    std::array<std::array<int16_t, kCols>, kRows> res;
    for (int row = 0; row < kRows; ++row) {
      for (int col = 0; col < kCols; ++col) {
        res[row][col] = matrix_[row][col].serialize_io();
      }
    }
    return res;
  }

  // Will serialize with only blair_path and coin per cell
  constexpr std::array<std::array<int16_t, kCols>, kRows> serialize_blair() const {
    std::array<std::array<int16_t, kCols>, kRows> res;
    for (int row = 0; row < kRows; ++row) {
      for (int col = 0; col < kCols; ++col) {
        res[row][col] = matrix_[row][col].serialize_blair();
      }
    }
    return res;
  }

 private:
  constexpr std::array<Cell, kCols>& operator[](size_t row) {
    return matrix_[row];
  }

  constexpr Cell& operator[](position pos) {
    return matrix_[pos.first][pos.second];
  }

  constexpr void set_start(position pos) {
    start_ = pos;
  }

  constexpr void set_end(position pos) {
    end_ = pos;
  }

  position start_;
  position end_;
  std::array<std::array<Cell, kCols>, kRows> matrix_;
};

}  // namespace io_blair
