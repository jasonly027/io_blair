#include "maze.hpp"

#include <gtest/gtest.h>

#include <bitset>

using std::bitset;

namespace io_blair {

TEST(CellTest, ValuesUnsetByDefault) {
    Cell cell;

    EXPECT_FALSE(cell.path);
    EXPECT_FALSE(cell.io_path);
    EXPECT_FALSE(cell.blair_path);
    EXPECT_FALSE(cell.coin);
}

TEST(CellTest, SetAllPaths) {
    Cell cell;

    cell.set_all_paths(true);
    EXPECT_TRUE(cell.path);
    EXPECT_TRUE(cell.io_path);
    EXPECT_TRUE(cell.blair_path);
    EXPECT_TRUE(cell.coin);

    cell.set_all_paths(false);
    EXPECT_FALSE(cell.path);
    EXPECT_FALSE(cell.io_path);
    EXPECT_FALSE(cell.blair_path);
    EXPECT_FALSE(cell.coin);
}

TEST(CellTest, SerializeIO) {
    using bits = bitset<2>;

    Cell cell;

    cell.io_path = true;
    cell.coin = true;
    ASSERT_EQ(bits("11"), bits(cell.serialize_io()));

    cell.io_path = true;
    cell.coin = false;
    ASSERT_EQ(bits("01"), bits(cell.serialize_io()));

    cell.io_path = false;
    cell.coin = true;
    ASSERT_EQ(bits("10"), bits(cell.serialize_io()));

    cell.io_path = false;
    cell.coin = false;
    ASSERT_EQ(bits("00"), bits(cell.serialize_io()));
}

TEST(CellTest, SerializeBlair) {
    using bits = bitset<2>;

    Cell cell;

    cell.blair_path = true;
    cell.coin = true;
    ASSERT_EQ(bits("11"), bits(cell.serialize_blair()));

    cell.blair_path = true;
    cell.coin = false;
    ASSERT_EQ(bits("01"), bits(cell.serialize_blair()));

    cell.blair_path = false;
    cell.coin = true;
    ASSERT_EQ(bits("10"), bits(cell.serialize_blair()));

    cell.blair_path = false;
    cell.coin = false;
    ASSERT_EQ(bits("00"), bits(cell.serialize_blair()));
}

}  // namespace io_blair
