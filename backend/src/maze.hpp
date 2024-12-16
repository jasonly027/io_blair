#pragma once

#include <array>
#include <bitset>
#include <cstdint>
#include <utility>

namespace io_blair {

class Cell {
   public:
    bool path() const;
    void set_path(bool value);

    // If IO can see path
    bool io_path() const;
    void set_io_path(bool value);

    // If Blair can see path
    bool blair_path() const;
    void set_blair_path(bool value);

    bool coin() const;
    void set_coin(bool value);

    // Will serialize as if Blair can't see any paths
    int8_t serialize_io() const;
    // Will serialize as if IO can't see any paths
    int8_t serialize_blair() const;

    static constexpr int8_t kFeatures = 4;

   private:
    // {coin, blair_path, io_path, path}
    std::bitset<kFeatures> data_;
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

    // Will serialize as if Blair can't see any paths
    std::array<int8_t, kTotalDim> serialize_io() const;
    // Will serialize as if IO can't see any paths
    std::array<int8_t, kTotalDim> serialize_blair() const;

   private:
    std::array<Cell, kCols>& operator[](int8_t row);

    void set_start(int8_t row, int8_t col);

    void set_end(int8_t row, int8_t col);

    position start_;
    position end_;
    std::array<std::array<Cell, kCols>, kRows> matrix_;
};

}  // namespace io_blair
