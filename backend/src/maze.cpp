#include "maze.hpp"

using std::array;

namespace io_blair {

void Cell::set_all_paths(bool value) {
    path = value;
    io_path = value;
    blair_path = value;
    coin = value;
}

int8_t Cell::serialize_io() const {
    int8_t bits = 0;
    bits |= static_cast<int>(io_path);
    bits |= static_cast<int>(coin) << 1;
    return bits;
}

int8_t Cell::serialize_blair() const {
    int8_t bits = 0;
    bits |= static_cast<int>(blair_path);
    bits |= static_cast<int>(coin) << 1;
    return bits;
}

Maze Maze::generate_maze() {
    Maze maze;

    return maze;
}

auto Maze::operator[](int8_t row) -> array<Cell, kCols>& {
    return matrix_[row];
}

auto Maze::operator[](int8_t row) const -> const array<Cell, kCols>& {
    return matrix_[row];
}

auto Maze::start() const -> position { return start_; }

void Maze::set_start(int8_t row, int8_t col) {
    start_.first = row;
    start_.second = col;
}

bool Maze::is_start(int8_t row, int8_t col) const {
    return start_.first == row && start_.second == col;
}

auto Maze::end() const -> position { return end_; }

void Maze::set_end(int8_t row, int8_t col) {
    end_.first = row;
    end_.second = col;
}

bool Maze::is_end(int8_t row, int8_t col) const {
    return end_.first == row && end_.second == col;
}

auto Maze::serialize_io() const -> array<array<int8_t, kCols>, kRows> {
    array<array<int8_t, kCols>, kRows> res;
    for (size_t row = 0; row < matrix_.size(); ++row) {
        for (size_t col = 0; col < matrix_[row].size(); ++col) {
            res[row][col] = matrix_[row][col].serialize_io();
        }
    }

    return res;
}

auto Maze::serialize_blair() const -> array<array<int8_t, kCols>, kRows> {
    array<array<int8_t, kCols>, kRows> res;
    for (size_t row = 0; row < matrix_.size(); ++row) {
        for (size_t col = 0; col < matrix_[row].size(); ++col) {
            res[row][col] = matrix_[row][col].serialize_io();
        }
    }

    return res;
}

}  // namespace io_blair
