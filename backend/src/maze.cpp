#include "maze.hpp"

#include <random>


namespace io_blair::direction {
using std::array;
using std::mt19937;
using std::random_device;
using std::uniform_int_distribution;
namespace ranges = std::ranges;


namespace {
thread_local mt19937 mt{random_device{}()};
}

array<General, 4> random_dirs() {
  array<General, 4> res{kUp, kRight, kDown, kLeft};
  ranges::shuffle(res, mt);
  return res;
}

Character random_char() {
  thread_local uniform_int_distribution dist(1, 10);
  switch (dist(mt)) {
    case 1:  return Character::Io;
    case 2:  return Character::Blair;
    default: return Character::unknown;
  }
}

bool random_coin() {
  thread_local uniform_int_distribution dist(1, 10);
  return dist(mt) == 1;
}

}  // namespace io_blair::direction
