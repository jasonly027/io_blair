#include "maze.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <bitset>
#include <limits>
#include <optional>


namespace io_blair {

using std::bitset, std::nullopt, std::numeric_limits;

TEST(CellTest, SetPathIO) {
  Cell cell;

  EXPECT_FALSE(cell.up());
  cell.set_path_io(Direction::kUp, true);
  EXPECT_TRUE(cell.up());

  EXPECT_FALSE(cell.right());
  cell.set_path_io(Direction::kRight, true);
  EXPECT_TRUE(cell.right());

  EXPECT_FALSE(cell.down());
  cell.set_path_io(Direction::kDown, true);
  EXPECT_TRUE(cell.down());

  EXPECT_FALSE(cell.left());
  cell.set_path_io(Direction::kLeft, true);
  EXPECT_TRUE(cell.left());
}

TEST(CellTest, SetPathBlair) {
  Cell cell;

  EXPECT_FALSE(cell.up());
  cell.set_path_blair(Direction::kUp, true);
  EXPECT_TRUE(cell.up());

  EXPECT_FALSE(cell.right());
  cell.set_path_blair(Direction::kRight, true);
  EXPECT_TRUE(cell.right());

  EXPECT_FALSE(cell.down());
  cell.set_path_blair(Direction::kDown, true);
  EXPECT_TRUE(cell.down());

  EXPECT_FALSE(cell.left());
  cell.set_path_blair(Direction::kLeft, true);
  EXPECT_TRUE(cell.left());
}

TEST(CellTest, SetPathBoth) {
  Cell cell;

  EXPECT_FALSE(cell.up());
  cell.set_path_both(Direction::kUp, true);
  EXPECT_TRUE(cell.up());

  EXPECT_FALSE(cell.right());
  cell.set_path_both(Direction::kRight, true);
  EXPECT_TRUE(cell.right());

  EXPECT_FALSE(cell.down());
  cell.set_path_both(Direction::kDown, true);
  EXPECT_TRUE(cell.down());

  EXPECT_FALSE(cell.left());
  cell.set_path_both(Direction::kLeft, true);
  EXPECT_TRUE(cell.left());
}

TEST(CellTest, Directions) {
  Cell cell;

  // A direction should be true if either
  // IO or Blair can see the path

  cell.set_path_io(Direction::kUp, true);
  cell.set_path_blair(Direction::kUp, false);
  EXPECT_TRUE(cell.up());

  cell.set_path_io(Direction::kUp, false);
  cell.set_path_blair(Direction::kUp, true);
  EXPECT_TRUE(cell.up());

  cell.set_path_io(Direction::kRight, true);
  cell.set_path_blair(Direction::kRight, false);
  EXPECT_TRUE(cell.right());

  cell.set_path_io(Direction::kRight, false);
  cell.set_path_blair(Direction::kRight, true);
  EXPECT_TRUE(cell.right());

  cell.set_path_io(Direction::kDown, true);
  cell.set_path_blair(Direction::kDown, false);
  EXPECT_TRUE(cell.down());

  cell.set_path_io(Direction::kDown, false);
  cell.set_path_blair(Direction::kDown, true);
  EXPECT_TRUE(cell.down());

  cell.set_path_io(Direction::kLeft, true);
  cell.set_path_blair(Direction::kLeft, false);
  EXPECT_TRUE(cell.left());

  cell.set_path_io(Direction::kLeft, false);
  cell.set_path_blair(Direction::kLeft, true);
  EXPECT_TRUE(cell.left());
}

TEST(CellTest, AnyPath) {
  Cell cell;

  EXPECT_FALSE(cell.any_path());

  cell.set_path_both(Direction::kUp, true);
  EXPECT_TRUE(cell.any_path());
  cell.set_path_both(Direction::kUp, false);

  cell.set_path_both(Direction::kRight, true);
  EXPECT_TRUE(cell.any_path());
  cell.set_path_both(Direction::kRight, false);

  cell.set_path_both(Direction::kDown, true);
  EXPECT_TRUE(cell.any_path());
  cell.set_path_both(Direction::kDown, false);

  cell.set_path_both(Direction::kLeft, true);
  EXPECT_TRUE(cell.any_path());
  cell.set_path_both(Direction::kLeft, false);
}

TEST(CellTest, ClearPaths) {
  Cell cell;

  EXPECT_FALSE(cell.up());
  EXPECT_FALSE(cell.right());
  EXPECT_FALSE(cell.down());
  EXPECT_FALSE(cell.left());

  cell.set_path_both(Direction::kUp, true);
  cell.set_path_both(Direction::kRight, true);
  cell.set_path_both(Direction::kDown, true);
  cell.set_path_both(Direction::kLeft, true);

  EXPECT_TRUE(cell.up());
  EXPECT_TRUE(cell.right());
  EXPECT_TRUE(cell.down());
  EXPECT_TRUE(cell.left());

  cell.clear_paths_both();

  EXPECT_FALSE(cell.up());
  EXPECT_FALSE(cell.right());
  EXPECT_FALSE(cell.down());
  EXPECT_FALSE(cell.left());
}

TEST(CellTest, Coin) {
  Cell cell;

  EXPECT_FALSE(cell.coin());

  cell.set_coin(true);

  EXPECT_TRUE(cell.coin());
}

TEST(CellTest, SerializeIO) {
  using bits = bitset<9>;

  Cell cell;

  cell.set_path_both(Direction::kUp, true);
  cell.set_path_both(Direction::kRight, true);
  cell.set_path_io(Direction::kDown, true);
  cell.set_path_blair(Direction::kLeft, true);
  cell.set_coin(true);

  ASSERT_EQ(bits("101000011"), bits(cell.serialize_io()));
}

TEST(CellTest, SerializeBlair) {
  using bits = bitset<9>;

  Cell cell;

  cell.set_path_both(Direction::kUp, true);
  cell.set_path_both(Direction::kRight, true);
  cell.set_path_io(Direction::kDown, true);
  cell.set_path_blair(Direction::kLeft, true);
  cell.set_coin(true);

  ASSERT_EQ(bits("110000011"), bits(cell.serialize_blair()));
}

TEST(MazeTest, GetNeighbor) {
  using position = Maze::position;

  position pos{0, 0};
  EXPECT_EQ(Maze::get_neighbor(pos, Direction::kUp), nullopt);
  EXPECT_EQ(Maze::get_neighbor(pos, Direction::kRight), (position{0, 1}));
  EXPECT_EQ(Maze::get_neighbor(pos, Direction::kDown), (position{1, 0}));
  EXPECT_EQ(Maze::get_neighbor(pos, Direction::kLeft), nullopt);

  pos = {1, 1};
  EXPECT_EQ(Maze::get_neighbor(pos, Direction::kUp), (position{0, 1}));
  EXPECT_EQ(Maze::get_neighbor(pos, Direction::kRight), (position{1, 2}));
  EXPECT_EQ(Maze::get_neighbor(pos, Direction::kDown), (position{2, 1}));
  EXPECT_EQ(Maze::get_neighbor(pos, Direction::kLeft), (position{1, 0}));
}

TEST(MazeTest, AreNeighbors) {
  using position = Maze::position;

  position center = {0, 0};

  position up    = {-1, 0};
  position right = {0, 1};
  position down  = {1, 0};
  position left  = {0, -1};

  EXPECT_TRUE(Maze::are_neighbors(center, up));
  EXPECT_TRUE(Maze::are_neighbors(center, right));
  EXPECT_TRUE(Maze::are_neighbors(center, down));
  EXPECT_TRUE(Maze::are_neighbors(center, left));

  position top_right    = {-1, 1};
  position bottom_right = {1, 1};
  position bottom_left  = {1, -1};
  position top_left     = {-1, -1};

  EXPECT_FALSE(Maze::are_neighbors(center, top_right));
  EXPECT_FALSE(Maze::are_neighbors(center, bottom_right));
  EXPECT_FALSE(Maze::are_neighbors(center, bottom_left));
  EXPECT_FALSE(Maze::are_neighbors(center, top_left));

  position far = {std::numeric_limits<int>::max(), std::numeric_limits<int>::max()};

  EXPECT_FALSE(Maze::are_neighbors(center, far));
}

}  // namespace io_blair
