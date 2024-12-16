#include "maze.hpp"

#include <gtest/gtest.h>

#include <bitset>

using std::bitset;

namespace io_blair {

TEST(CellTest, ValuesUnsetByDefault) {
    Cell cell;

    EXPECT_FALSE(cell.path());
    EXPECT_FALSE(cell.io_path());
    EXPECT_FALSE(cell.blair_path());
    EXPECT_FALSE(cell.coin());
}

TEST(CellTest, SerializeIO) {
    using bits = bitset<Cell::kFeatures>;

    Cell cell;

    cell.set_path(true);
    cell.set_io_path(true);
    cell.set_coin(true);

    ASSERT_EQ(bits("1011"), bits(cell.serialize_io()));

    cell.set_blair_path(true);
    ASSERT_EQ(bits("1011"), bits(cell.serialize_io()))
        << "The third bit, blair_path, should always be off";
}

TEST(CellTest, SerializeBlair) {
    using bits = bitset<Cell::kFeatures>;

    Cell cell;

    cell.set_path(true);
    cell.set_io_path(true);
    cell.set_blair_path(true);
    cell.set_coin(true);

    ASSERT_EQ(bits("1101"), bits(cell.serialize_blair()));

    cell.set_blair_path(true);
    ASSERT_EQ(bits("1101"), bits(cell.serialize_blair()))
        << "The second bit, io_path, should always be off";
}

}  // namespace io_blair
