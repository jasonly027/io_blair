/**
 * @file maze.hpp
 */
#pragma once

#include <algorithm>
#include <array>
#include <bitset>
#include <cstdint>
#include <cstdlib>
#include <stack>
#include <type_traits>
#include <utility>

#include "character.hpp"


namespace io_blair {
/**
 * @brief A pair of ints. The first is x and the second is y.
 */
using coordinate = std::pair<int, int>;

/**
 * @brief A tuple of ints. The first is x and the second is y.
 */
using coordinate_arr = std::array<int, 2>;

namespace direction {
template <typename T>
concept CardinalDirectionEnum = std::is_enum_v<T> && requires {
  { T::kUp } -> std::convertible_to<T>;
  { T::kRight } -> std::convertible_to<T>;
  { T::kDown } -> std::convertible_to<T>;
  { T::kLeft } -> std::convertible_to<T>;
};

/**
 * @brief The underlying type of the directional enums.
 */
using UnderlyingDirectionT = int;

/**
 * @brief General enumeration of cardinal directions
 */
enum class General : UnderlyingDirectionT { kUp, kRight, kDown, kLeft };
using enum General;

/**
 * @brief Cardinal directions in relation to io_blair::Character::Io. Underlying value
 * has significance with io_blair::Cell's data access and serialization.
 */
enum class Io : UnderlyingDirectionT { kUp = 0, kRight = 1, kDown = 2, kLeft = 3 };

/**
 * @brief Cardinal directions in relation to io_blair::Character::Blair. Underlying
 * value has signifiance with io_blair::Cell's data access and serialization.
 */
enum class Blair : UnderlyingDirectionT { kUp = 4, kRight = 5, kDown = 6, kLeft = 7 };

/**
 * @brief Constrains \p T to be either io_blair::direction::Io or io_blair::direction::Blair.
 */
template <typename T>
concept CharacterDirectionEnum = std::same_as<T, Io> || std::same_as<T, Blair>;

/**
 * @brief Casts \p dir to its underlying type.
 * 
 * @param dir
 * @return UnderlyingDirectionT 
 */
constexpr UnderlyingDirectionT to_underlying(CharacterDirectionEnum auto dir) {
  return static_cast<UnderlyingDirectionT>(dir);
}

/**
 * @brief Cardinal directions in relation to both
 * io_blair::Character::Io and io_blair::Character::Blair.
 */
enum class Both : UnderlyingDirectionT { kUp, kRight, kDown, kLeft };

/**
 * @brief Cardinal directions in relation to either
 * io_blair::Character::Io and io_blair::Character::Blair.
 */
enum class Either : UnderlyingDirectionT { kUp, kRight, kDown, kLeft };

/**
 * @brief Converts a cardinal direction enum type to another type.
 * 
 * @tparam OutDir 
 * @tparam InDir 
 * @param dir 
 * @return OutDir 
 */
template <CardinalDirectionEnum OutDir, CardinalDirectionEnum InDir>
constexpr OutDir to(InDir dir) {
  switch (dir) {
    case InDir::kUp:    return OutDir::kUp;
    case InDir::kRight: return OutDir::kRight;
    case InDir::kDown:  return OutDir::kDown;
    case InDir::kLeft:  return OutDir::kLeft;
  }
}

/**
 * @brief Converts a cardinal direction to its opposite cardinal direction.
 * 
 * @tparam Direction 
 * @param dir 
 * @return Direction 
 */
template <CardinalDirectionEnum Direction>
constexpr Direction opposite(Direction dir) {
  switch (dir) {
    case Direction::kUp:    return Direction::kDown;
    case Direction::kRight: return Direction::kLeft;
    case Direction::kDown:  return Direction::kUp;
    case Direction::kLeft:  return Direction::kRight;
  }
}

/**
 * @brief Creates a shuffled array of every enum in direction::General.
 * 
 * @return std::array<General, 4> 
 */
std::array<General, 4> random_dirs();

/**
 * @brief Gets a random Character. Both io_blair::Character::Io and
 * io_blair::Character::Blair each have a 10% chance while
 * io_blair::Character::unknown has a 80% chance.
 * 
 * @return Character 
 */
Character random_char();

/**
 * @brief Has a 10% chance to return true, otherwise false.
 * 
 * @return true 
 * @return false 
 */
bool random_coin();

/**
 * @brief Calculates a new coordinate by translating \p start
 * towards \p dir by 1.
 * 
 * @tparam Direction 
 * @param start The starting coordinate.
 * @param dir The direction to translate towards.
 * @return coordinate 
 */
template <CardinalDirectionEnum Direction>
constexpr coordinate translate(coordinate start, Direction dir) {
  const auto [x, y] = start;
  switch (dir) {
    case Direction::kUp:    return {x, y - 1};
    case Direction::kRight: return {x + 1, y};
    case Direction::kDown:  return {x, y + 1};
    case Direction::kLeft:  return {x - 1, y};
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
  /**
   * @brief Constructs a new Cell object.
   * 
   * @param bits Underlying bits data for cell.
   */
  constexpr explicit Cell(std::bitset<9> bits = {})
      : bits_(std::move(bits)) {}

  /**
   * @brief Determines whether this character can see a path
   * to the neighbor towards \p dir.
   * 
   * @param dir The direction to try seeing.
   * @return true This character can see a path.
   * @return false This character cannot see a path.
   */
  constexpr bool operator[](direction::CharacterDirectionEnum auto dir) const {
    return bits_[direction::to_underlying(dir)];
  }

  /**
   * @brief Determines whether both character can see a path
   * to the neighbor towards \p dir.
   * 
   * @param dir The direction to try seeing.
   * @return true This character can see a path.
   * @return false This character cannot see a path.
   */
  constexpr bool operator[](direction::Both dir) const {
    return (*this)[direction::to<direction::Io>(dir)]
           && (*this)[direction::to<direction::Blair>(dir)];
  }

  /**
   * @brief Determines whether either character can see a path
   * to the neighbor towards \p dir.
   * 
   * @param dir The direction to try seeing.
   * @return true This character can see a path.
   * @return false This character cannot see a path.
   */
  constexpr bool operator[](direction::Either dir) const {
    return (*this)[direction::to<direction::Io>(dir)]
           || (*this)[direction::to<direction::Blair>(dir)];
  }

  /**
   * @brief Determines whether this cell can access any
   * neighboring cells.
   * 
   * @return true A neighbor can be accessed.
   * @return false A neighbor cannot be accessed.
   */
  constexpr bool any() const {
    using Either    = direction::Either;
    const auto self = *this;
    return self[Either::kUp] || self[Either::kRight] || self[Either::kDown] || self[Either::kLeft];
  }

  /**
   * @brief Determines whether there is a coin in this cell.
   * 
   * @return true There is a coin.
   * @return false There isn't a coin.
   */
  constexpr bool coin() const {
    return bits_[kCoinIdx];
  }

  /**
   * @brief Changes whether the character represented by \p dir
   * can see the path in that direction.
   * 
   * @param dir The direction and represented character.
   * @param value Whether the path towards \p dir can be seen or not.
   */
  constexpr void set(direction::CharacterDirectionEnum auto dir, bool value) {
    bits_[direction::to_underlying(dir)] = value;
  }

  /**
   * @brief Changes how both characters see the path in the direction
   * of \p dir.
   * 
   * @param dir The direction.
   * @param value Whether the path towards \p dir can be seen or not.
   */
  constexpr void set(direction::Both dir, bool value) {
    this->set(direction::to<direction::Io>(dir), value);
    this->set(direction::to<direction::Blair>(dir), value);
  }

  /**
   * @brief Changes whether there is a coin in this cell.
   * 
   * @param value Whether there is a coin or not.
   */
  constexpr void set_coin(bool value) {
    bits_[kCoinIdx] = value;
  }

  /**
   * @brief Removes any seeable paths by either character.
   */
  constexpr void clear() {
    bits_.reset();
  }

  /**
   * @brief Serialize this cell with respect to \p character into bit flags.
   *
   * The first four bits are what both characters can see
   * up, right, down, and left, respectively.
   *
   * The next four bits are what \p character can see
   * up, right, down, and left, respectively. This does not imply
   * whether the other character can or cannot see it.
   *
   * The remaining bit is whether this cell has a coin or not.
   * 
   * @param character The character to serialize for.
   * @return int16_t 
   */
  constexpr int16_t serialize_for(Character character) const {
    switch (character) {
      case Character::unknown: return 0;
      case Character::Io:      return serialize_impl<direction::Io, direction::Blair>();
      case Character::Blair:   return serialize_impl<direction::Blair, direction::Io>();
    }
  }

 private:
  // The index of the coin flag in bits_
  static constexpr size_t kCoinIdx = 8;

  // See serialize_for
  template <direction::CharacterDirectionEnum Character, direction::CharacterDirectionEnum Other>
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

  // Flags for representing what either character can see in directions + coin flag.
  std::bitset<9> bits_;
};

/**
 * @brief A maze made up of Cell's which a start and end point.
 */
template <int Rows, int Cols>
class Maze {
 public:
  template <typename T>
  using matrix = std::array<std::array<T, Cols>, Rows>;

  /**
   * @brief Gets the number of rows in the maze.
   * 
   * @return int 
   */
  static constexpr int rows() {
    return Rows;
  }

  /**
   * @brief Gets the number of columns in the maze.
   * 
   * @return int 
   */
  static constexpr int cols() {
    return Cols;
  }

  /**
   * @brief Determines whether or not \p coordinate is within
   * the bounds of the maze.
   * 
   * @param coordinate The coordinate to check.
   * @return true Within bounds.
   * @return false Not within bounds.
   */
  static constexpr bool in_range(coordinate coordinate) {
    auto [x, y] = coordinate;
    return in_range(x, y);
  }

  /**
   * @brief Determines whether or not the coordinate is within
   * the bounds of the maze.
   * 
   * @param x The x coordinate (column).
   * @param y The y coordinate (row).
   * @return true Within bounds.
   * @return false Not within bounds.
   */
  static constexpr bool in_range(int x, int y) {
    return x >= 0 && x < Cols && y >= 0 && y < Rows;
  }

  /**
   * @brief Generates a random maze. \p start and \p end are
   * not checked to see if they are within bounds of the maze.
   *
   * @see Maze::randomize
   * 
   * @param start The start of the maze.
   * @param end The end of the maze.
   * @return Maze<Rows, Cols> 
   */
  static Maze<Rows, Cols> random(coordinate start, coordinate end) {
    return Maze<Rows, Cols>(std::move(start), std::move(end)).randomize();
  }

  /**
   * @brief Constructs a new Maze object.
   *
   * @param start The start of the maze.
   * @param end The end of the maze.
   * @param matrix The underlying matrix of the maze.
   */
  constexpr Maze(coordinate start, coordinate end,
                 matrix<Cell> matrix = std::array<std::array<Cell, Cols>, Rows>())
      : start_(std::move(start)), end_(std::move(end)), matrix_(std::move(matrix)) {}

  /**
   * @brief Gets the start of the maze.
   * 
   * @return const coordinate& 
   */
  constexpr const coordinate& start() const {
    return start_;
  }

  /**
   * @brief Gets the end of the maze.
   * 
   * @return const coordinate& 
   */
  constexpr const coordinate& end() const {
    return end_;
  }

  /**
   * @brief Determines if there are any coins left.
   * 
   * @return true There are coins left.
   * @return false There are no coins left.
   */
  constexpr bool any_coin() const {
    return std::ranges::any_of(matrix_, [](const auto& row) {
      return std::ranges::any_of(row, [](const Cell& cell) { return cell.coin(); });
    });
  }

  /**
   * @brief Accesses a cell in the maze.
   * 
   * @warning Undefined behavior if out of bounds access.
   * 
   * @param coordinate Identifies which cell in the maze to access.
   * @return const Cell& 
   */
  constexpr const Cell& at(coordinate coordinate) const {
    auto [x, y] = coordinate;
    return at(y, x);
  }

  /**
   * @brief Accesses a cell in the maze.
   *
   * @warning Undefined behavior if out of bounds access.
   * 
   * @param row The row of the cell.
   * @param col The column of the cell.
   * @return const Cell& 
   */
  constexpr const Cell& at(int row, int col) const {
    return matrix_[row][col];
  }

  /**
   * @brief Determines if a path from \p a to \p b exists.
   * 
   * @param a The start.
   * @param b The destination.
   * @return true There is a path.
   * @return false There is not a path.
   */
  constexpr bool traversable(coordinate a, coordinate b) const {
    auto [x1, y1] = a;
    if (!in_range(x1, y1)) return false;

    auto [x2, y2] = b;

    if (x1 == x2) {
      // a
      // ↓
      // b
      if (y1 + 1 == y2) return at(y1, x1)[direction::Either::kDown];
      // b
      // ↑
      // a
      if (y1 - 1 == y2) return at(y1, x1)[direction::Either::kUp];
    }

    if (y1 == y2) {
      // a → b
      if (x1 + 1 == x2) return at(y1, x1)[direction::Either::kRight];
      // b ← a
      if (x1 - 1 == x2) return at(y1, x1)[direction::Either::kLeft];
    }

    return false;
  }

  /**
   * @brief Randomizes the paths in the maze using recursive backtracking.
   *
   * @see https://weblog.jamisbuck.org/2010/12/27/maze-generation-recursive-backtracking
   */
  void randomize() {
    clear();

    namespace dir = direction;
    using dir::to;

    struct TraversalHistory {
      coordinate coord;                  // Identifies a cell in the maze for traversing
      int current_dir;                   // The next idx to use when accessing dirs
      std::array<dir::General, 4> dirs;  // Shuffled cardinal directions sequence
    };

    std::stack<TraversalHistory> stack;
    stack.emplace(coordinate{0, 0}, 0, dir::random_dirs());

    // Loops until every maze cell has been visited
    while (!stack.empty()) {
      auto [coord, i, dirs] = stack.top();
      auto current_dir      = dirs[i];
      stack.pop();

      // Continue to return this cell to the stack until we've tried every direction from it
      if (i + 1 < dirs.size()) {
        stack.emplace(coord, i + 1, dirs);
      }

      // Identify and determine if neighboring cell is in bounds of the maze
      // and has never been visited
      auto neighbor = dir::translate(coord, current_dir);
      if (!in_range(neighbor) || at(neighbor).any()) {
        continue;
      }

      // Create access from current cell to neighbor.
      // Use randomizer to determine who can see the path
      switch (dir::random_char()) {
        case Character::unknown: {
          bridge(coord, to<dir::Both>(current_dir), true);
        } break;
        case Character::Io: {
          bridge(coord, to<dir::Io>(current_dir), true);
        } break;
        case Character::Blair: {
          bridge(coord, to<dir::Blair>(current_dir), true);
        } break;
      }

      // Use randomizer to determine if cell should have coin
      at_mutable(neighbor).set_coin(dir::random_coin());

      // Continue traversal from neighbor's cell
      stack.emplace(neighbor, 0, dir::random_dirs());
    }
  }

  /**
   * @brief Removes the coin at \p coordinate. Does nothing
   * if coordinate is out of range or there is no coin in that
   * cell.
   * 
   * @param coordinate The coordinate to remove a coin from.
   */
  constexpr void take_coin(coordinate coordinate) {
    if (!in_range(coordinate)) return;
    at_mutable(coordinate).set_coin(false);
  }

  /**
   * @brief Serializes the maze with respect to \p character in bit flags.
   * 
   * @see Cell::serialize_for
   *
   * @param character The character to serialize for.
   * @return matrix<int16_t> 
   */
  constexpr matrix<int16_t> serialize_for(Character character) const {
    matrix<int16_t> res;
    for (int row = 0; row < Rows; ++row) {
      for (int col = 0; col < Cols; ++col) {
        res[row][col] = at(row, col).serialize_for(character);
      }
    }

    return res;
  }

  constexpr void clear() {
    for (int row = 0; row < Rows; ++row) {
      for (int col = 0; col < Cols; ++col) {
        at_mutable(row, col).clear();
      }
    }
  }

 private:
  // Undefined behavior if coordinate is out of bounds.
  constexpr Cell& at_mutable(coordinate coordinate) {
    auto [x, y] = coordinate;
    return at_mutable(y, x);
  }

  // Undefined behavior if row, col are out of bounds.
  constexpr Cell& at_mutable(int row, int col) {
    return matrix_[row][col];
  }

  // Changes the path visibility from the cell at coord to
  // its neighbor cell (if any). Updates visiblity from the perspective of both
  // sides for consistency, i.e. For cells A and B, if A can move to B,
  // B can move to A.
  constexpr void bridge(coordinate coord, auto dir, bool value) {
    at_mutable(coord).set(dir, value);

    if (auto neighbor = direction::translate(coord, dir); in_range(neighbor)) {
      at_mutable(neighbor).set(direction::opposite(dir), value);
    }
  }

  coordinate start_;
  coordinate end_;

  // Underlying matrix for the maze.
  matrix<Cell> matrix_;
};
}  // namespace io_blair
