#pragma once

#include <array>
#include <bitset>
#include <concepts>
#include <cstdint>
#include <cstdlib>
#include <utility>

#include "character.hpp"


namespace io_blair {
namespace direction {

using UnderlyingDirectionT = int;

enum class General { kUp, kRight, kDown, kLeft };
using enum General;

enum class Io : UnderlyingDirectionT { kUp = 0, kRight = 1, kDown = 2, kLeft = 3 };

enum class Blair : UnderlyingDirectionT { kUp = 4, kRight = 5, kDown = 6, kLeft = 7 };

template <typename T>
concept Direction = std::same_as<T, Io> || std::same_as<T, Blair>;

enum class Both : UnderlyingDirectionT { kUp, kRight, kDown, kLeft };


constexpr UnderlyingDirectionT to_underlying(Io dir) {
  return static_cast<UnderlyingDirectionT>(dir);
}

constexpr UnderlyingDirectionT to_underlying(Blair dir) {
  return static_cast<UnderlyingDirectionT>(dir);
}

constexpr std::pair<Io, Blair> both(Both dir) {
  switch (dir) {
    case Both::kUp:    return {Io::kUp, Blair::kUp};
    case Both::kRight: return {Io::kRight, Blair::kRight};
    case Both::kDown:  return {Io::kDown, Blair::kDown};
    case Both::kLeft:  return {Io::kLeft, Blair::kLeft};
  }
}
}  // namespace direction

/**
 * @brief A coordinate position within a Maze. 
 * Cardinal positions may or may not allow access to
 * a neighboring cell.
 */
class Cell {
 public:
  constexpr explicit Cell(std::bitset<9> bits = {})
      : bits_(std::move(bits)) {}

  constexpr bool operator[](direction::Direction auto dir) const {
    return bits_[direction::to_underlying(dir)];
  }

  constexpr bool operator[](direction::Both dir) const {
    const auto [io, blair] = direction::both(dir);
    return (*this)[io] || (*this)[blair];
  }

  constexpr bool any() const {
    using Both      = direction::Both;
    const auto self = *this;
    return self[Both::kUp] || self[Both::kRight] || self[Both::kDown] || self[Both::kLeft];
  }

  constexpr bool coin() const {
    return bits_[kCoinIdx];
  }

  constexpr void set(direction::Direction auto dir, bool value) {
    bits_[direction::to_underlying(dir)] = value;
  }

  constexpr void set(direction::Both dir, bool value) {
    const auto [io, blair] = direction::both(dir);
    this->set(io, value);
    this->set(blair, value);
  }

  constexpr void set_coin(bool value) {
    bits_[kCoinIdx] = value;
  }

  constexpr void clear() {
    bits_.reset();
  }

  constexpr int16_t serialize_for(Character character) const {
    switch (character) {
      case Character::unknown: return 0;
      case Character::Io:      return serialize_impl<direction::Io>();
      case Character::Blair:   return serialize_impl<direction::Blair>();
    }
  }

 private:
  static constexpr size_t kCoinIdx = 8;

  template <direction::Direction Character>
  constexpr int16_t serialize_impl() const {
    const auto self = *this;
    using Both      = direction::Both;

    std::bitset<9> res;

    res[0] = self[Both::kUp];
    res[1] = self[Both::kRight];
    res[2] = self[Both::kDown];
    res[3] = self[Both::kLeft];
    res[4] = self[Character::kUp];
    res[5] = self[Character::kRight];
    res[6] = self[Character::kDown];
    res[7] = self[Character::kLeft];
    res[8] = coin();

    return static_cast<int16_t>(res.to_ulong());
  }

  std::bitset<9> bits_;
};

using coordinate = std::pair<int, int>;

/**
 * @brief A maze made up of Cell's which a start and end point.
 */
template <int Rows, int Cols>
class Maze {
 public:
  template <typename T>
  using matrix = std::array<std::array<T, Cols>, Rows>;

  constexpr Maze(matrix<Cell> matrix, coordinate start, coordinate end)
      : start(start), end(end), matrix_(matrix) {}

  constexpr const Cell& at(int row, int col) const {
    return matrix_[row][col];
  }

  constexpr bool traversable(coordinate a, coordinate b) const {
    auto [x1, y1] = a;
    if (x1 < 0 || x1 >= Rows || y1 < 0 || y1 >= Cols) return false;

    auto [x2, y2] = b;

    if (x1 == x2) {
      // a
      // ↓
      // b
      if (y1 + 1 == y2) return at(y1, x1)[direction::Both::kDown];
      // a
      // ↑
      // b
      if (y1 - 1 == y2) return at(y1, x1)[direction::Both::kUp];
    }

    if (y1 == y2) {
      // a → b
      if (x1 + 1 == x2) return at(y1, x2)[direction::Both::kRight];
      // b ← a
      if (x1 - 1 == x2) return at(y1, x2)[direction::Both::kLeft];
    }

    return false;
  }

  constexpr matrix<int16_t> serialize_for(Character character) const {
    matrix<int16_t> res;
    for (int row = 0; row < Rows; ++row) {
      for (int col = 0; col < Cols; ++col) {
        res[row][col] = at(row, col).serialize_for(character);
      }
    }

    return res;
  }

  const coordinate start;
  const coordinate end;

 private:
  matrix<Cell> matrix_;
};
}  // namespace io_blair
