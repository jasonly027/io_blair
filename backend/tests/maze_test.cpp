#include "maze.hpp"

#include <gtest/gtest.h>

#include <array>
#include <climits>
#include <cstdint>

#include "character.hpp"


namespace io_blair::testing {
using std::array;
namespace dir = direction;

TEST(CellShould, HaveNoPathsSetByDeafult) {
  Cell cell;

  EXPECT_FALSE(cell.any());
}

TEST(CellShould, HaveAnyWhenAnyDirectionIsSet) {
  Cell c1;
  Cell c2;
  Cell c3;
  Cell c4;

  c1.set(dir::Io::kUp, true);
  c2.set(dir::Io::kRight, true);
  c3.set(dir::Io::kDown, true);
  c4.set(dir::Io::kLeft, true);

  EXPECT_TRUE(c1.any());
  EXPECT_TRUE(c2.any());
  EXPECT_TRUE(c3.any());
  EXPECT_TRUE(c4.any());
}

TEST(CellShould, ResetFlagsOnClear) {
  Cell cell;

  cell.set(dir::Io::kUp, true);
  cell.set(dir::Io::kRight, true);
  cell.set(dir::Io::kDown, true);
  cell.set(dir::Io::kLeft, true);
  cell.clear();

  EXPECT_FALSE(cell.any());
}

TEST(CellShould, StoreCoinChanges) {
  Cell cell;
  EXPECT_FALSE(cell.coin());

  cell.set_coin(true);
  EXPECT_TRUE(cell.coin());
}

TEST(CellShould, StoreIoDirectionChangesExclusively) {
  Cell cell;

  cell.set(dir::Io::kUp, true);
  cell.set(dir::Io::kRight, true);
  cell.set(dir::Io::kDown, true);
  cell.set(dir::Io::kLeft, true);

  EXPECT_TRUE(cell[dir::Io::kUp]);
  EXPECT_TRUE(cell[dir::Io::kRight]);
  EXPECT_TRUE(cell[dir::Io::kDown]);
  EXPECT_TRUE(cell[dir::Io::kLeft]);

  EXPECT_FALSE(cell[dir::Blair::kUp]);
  EXPECT_FALSE(cell[dir::Blair::kRight]);
  EXPECT_FALSE(cell[dir::Blair::kDown]);
  EXPECT_FALSE(cell[dir::Blair::kLeft]);
}

TEST(CellShould, StoreBlairDirectionChangesExclusively) {
  Cell cell;

  cell.set(dir::Blair::kUp, true);
  cell.set(dir::Blair::kRight, true);
  cell.set(dir::Blair::kDown, true);
  cell.set(dir::Blair::kLeft, true);

  EXPECT_TRUE(cell[dir::Blair::kUp]);
  EXPECT_TRUE(cell[dir::Blair::kRight]);
  EXPECT_TRUE(cell[dir::Blair::kDown]);
  EXPECT_TRUE(cell[dir::Blair::kLeft]);

  EXPECT_FALSE(cell[dir::Io::kUp]);
  EXPECT_FALSE(cell[dir::Io::kRight]);
  EXPECT_FALSE(cell[dir::Io::kDown]);
  EXPECT_FALSE(cell[dir::Io::kLeft]);
}

TEST(CellShould, BeTrueForBothAccessWhenFullySet) {
  Cell cell;

  cell.set(dir::Both::kUp, true);
  cell.set(dir::Both::kRight, true);
  cell.set(dir::Io::kDown, true);
  cell.set(dir::Blair::kLeft, true);

  EXPECT_TRUE(cell[dir::Both::kUp]);
  EXPECT_TRUE(cell[dir::Both::kRight]);
  EXPECT_FALSE(cell[dir::Both::kDown]);
  EXPECT_FALSE(cell[dir::Both::kLeft]);

  EXPECT_TRUE(cell[dir::Io::kUp]);
  EXPECT_TRUE(cell[dir::Io::kRight]);
  EXPECT_TRUE(cell[dir::Io::kDown]);
  EXPECT_FALSE(cell[dir::Io::kLeft]);

  EXPECT_TRUE(cell[dir::Blair::kUp]);
  EXPECT_TRUE(cell[dir::Blair::kRight]);
  EXPECT_FALSE(cell[dir::Blair::kDown]);
  EXPECT_TRUE(cell[dir::Blair::kLeft]);
}

TEST(CellShould, BeTrueOnEitherAccessForAnySet) {
  Cell cell;

  cell.set(dir::Both::kUp, true);
  cell.set(dir::Both::kRight, true);
  cell.set(dir::Io::kDown, true);
  cell.set(dir::Blair::kLeft, true);

  EXPECT_TRUE(cell[dir::Either::kUp]);
  EXPECT_TRUE(cell[dir::Either::kRight]);
  EXPECT_TRUE(cell[dir::Either::kDown]);
  EXPECT_TRUE(cell[dir::Either::kLeft]);
}

TEST(CellShould, SerializeToZeroForUnknown) {
  Cell cell;

  cell.set(direction::Io::kUp, true);

  EXPECT_EQ(cell.serialize_for(Character::unknown), 0);
}

TEST(CellShould, SerializeToIo) {
  Cell cell;

  cell.set(direction::Both::kUp, true);
  cell.set(direction::Both::kRight, true);
  cell.set(direction::Io::kDown, true);
  cell.set(direction::Blair::kLeft, true);
  cell.set_coin(true);

  EXPECT_EQ(cell.serialize_for(Character::Io), 0b1'0111'0011);
}

TEST(CellShould, SerializeToBlair) {
  Cell cell;

  cell.set(direction::Both::kUp, true);
  cell.set(direction::Both::kRight, true);
  cell.set(direction::Io::kDown, true);
  cell.set(direction::Blair::kLeft, true);
  cell.set_coin(true);

  EXPECT_EQ(cell.serialize_for(Character::Blair), 0b1'1011'0011);
}

TEST(MazeShould, InRange) {
  using M = Maze<3, 2>;

  // x under boundary
  EXPECT_FALSE(M::in_range(-1, 0));
  // x over boundary
  EXPECT_FALSE(M::in_range(2, 0));
  // y under boundary
  EXPECT_FALSE(M::in_range(0, -1));
  // y over boundary
  EXPECT_FALSE(M::in_range(0, 3));
  // both oob
  EXPECT_FALSE(M::in_range(2, 3));

  // both at lower boundary
  EXPECT_TRUE(M::in_range(0, 0));
  // x at upper boundary
  EXPECT_TRUE(M::in_range(1, 0));
  // y at upper boundary
  EXPECT_TRUE(M::in_range(0, 2));
  // both at upper boundary
  EXPECT_TRUE(M::in_range(1, 2));
}

TEST(MazeShould, BeUnsetByDefault) {
  Maze<1, 2> maze({0, 0}, {0, 0});

  EXPECT_FALSE(maze.at(0, 0).any());
  EXPECT_FALSE(maze.at(0, 1).any());
}

class MazeLeftRightShouldF : public ::testing::Test {
 protected:
  Maze<1, 2> maze_{
      {0, 0},
      {1, 0},
      []() constexpr -> array<array<Cell, 2>, 1> {
        Cell c1;
        c1.set(dir::Io::kRight, true);
        Cell c2;
        c2.set(dir::Io::kLeft, true);

        return {
            {c1, c2}};
       }
      ()
  };
};

TEST_F(MazeLeftRightShouldF, BeTraversableFromBothDirections) {
  EXPECT_TRUE(maze_.traversable(maze_.start(), maze_.end()));
  EXPECT_TRUE(maze_.traversable(maze_.end(), maze_.start()));
}

TEST_F(MazeLeftRightShouldF, NotBeTraversableWhenStartIsOob) {
  EXPECT_FALSE(maze_.traversable({INT_MIN, INT_MIN}, maze_.end()));
  EXPECT_FALSE(maze_.traversable({INT_MAX, INT_MAX}, maze_.end()));
}

TEST_F(MazeLeftRightShouldF, NotBeTraversableWhenEndIsOob) {
  EXPECT_FALSE(maze_.traversable(maze_.start(), {INT_MIN, INT_MIN}));
  EXPECT_FALSE(maze_.traversable(maze_.start(), {INT_MAX, INT_MAX}));
}

TEST_F(MazeLeftRightShouldF, NotBeTraversableWhenBothAreOob) {
  EXPECT_FALSE(maze_.traversable({INT_MIN, INT_MIN}, {INT_MAX, INT_MAX}));
  EXPECT_FALSE(maze_.traversable({INT_MAX, INT_MAX}, {INT_MIN, INT_MIN}));
}

TEST_F(MazeLeftRightShouldF, BeSerialized) {
  using matrix = array<array<int16_t, 2>, 1>;

  EXPECT_EQ(maze_.serialize_for(Character::Io),
            (matrix{
                {0b0'0010'0000, 0b00'1000'0000}
  }));

  EXPECT_EQ(maze_.serialize_for(Character::Blair),
            (matrix{
                {0b0'0000'0000, 0b00'0000'0000}
  }));

  EXPECT_EQ(maze_.serialize_for(Character::unknown),
            (matrix{
                {0, 0}
  }));
}

}  // namespace io_blair::testing
