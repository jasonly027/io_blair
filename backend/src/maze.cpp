#include "maze.hpp"

#include <array>
#include <cstdint>

using std::array;

namespace io_blair {

bool Cell::path() const { return data_[0]; }
void Cell::set_path(bool value) { data_[0] = value; }

bool Cell::io_path() const { return data_[1]; }
void Cell::set_io_path(bool value) { data_[1] = value; }

bool Cell::blair_path() const { return data_[2]; }
void Cell::set_blair_path(bool value) { data_[2] = value; }

bool Cell::coin() const { return data_[3]; }
void Cell::set_coin(bool value) { data_[3] = value; }

int8_t Cell::serialize_io() const {
    Cell copy = *this;
    copy.set_blair_path(false);
    return copy.data_.to_ulong();
}

int8_t Cell::serialize_blair() const {
    Cell copy = *this;
    copy.set_io_path(false);
    return copy.data_.to_ulong();
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

auto Maze::serialize_io() const -> array<int8_t, kTotalDim> {
    array<int8_t, kTotalDim> res;
    for (size_t row = 0; row < matrix_.size(); ++row) {
        for (size_t col = 0; col < matrix_[row].size(); ++col) {
            res[row * kCols + col] = matrix_[row][col].serialize_io();
        }
    }

    return res;
}

auto Maze::serialize_blair() const -> array<int8_t, kTotalDim> {
    array<int8_t, kTotalDim> res;
    for (size_t row = 0; row < matrix_.size(); ++row) {
        for (size_t col = 0; col < matrix_[row].size(); ++col) {
            res[row * kCols + col] = matrix_[row][col].serialize_blair();
        }
    }

    return res;
}

}  // namespace io_blair
