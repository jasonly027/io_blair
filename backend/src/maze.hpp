#pragma once

#include <array>
#include <cstdint>
#include <utility>

namespace io_blair {

struct Cell {
   public:
    bool path = false;
    bool io_path = false;
    bool blair_path = false;
    bool coin = false;

    void set_all_paths(bool value);

    // Will serialize with only io_path and coin
    int8_t serialize_io() const;
    // Will serialize with only blair_path and coin
    int8_t serialize_blair() const;
};

class Maze {
   public:
    using position = std::pair<int8_t, int8_t>;

    static Maze generate_maze();

    static constexpr int8_t kRows = 16;
    static constexpr int8_t kCols = 16;
    static constexpr int16_t kTotalDim = kRows * kCols;

    const std::array<Cell, kCols>& operator[](int8_t row) const;

    // Where to start in the maze
    position start() const;

    bool is_start(int8_t row, int8_t col) const;

    // Where to end in the maze
    position end() const;

    bool is_end(int8_t row, int8_t col) const;

    // Will serialize with only io_path and coin per cell
    std::array<std::array<int8_t, kCols>, kRows> serialize_io() const;
    // Will serialize with only blair_path and coin per cell
    std::array<std::array<int8_t, kCols>, kRows> serialize_blair() const;

   private:
    std::array<Cell, kCols>& operator[](int8_t row);

    void set_start(int8_t row, int8_t col);

    void set_end(int8_t row, int8_t col);

    position start_;
    position end_;
    std::array<std::array<Cell, kCols>, kRows> matrix_;
};

}  // namespace io_blair
