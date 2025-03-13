#include "maze.hpp"
#include <gtest/gtest.h>
#include <iostream>

namespace io_blair::testing {
TEST(RandomShould, Callable) {
  auto dirs = direction::random_dirs();
  for (auto d : dirs) {
    std::cout << static_cast<int>(d);
  }
  Maze<4, 4>::random();
  FAIL();
}
}
