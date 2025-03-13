#include "maze.hpp"
#include <random>

namespace io_blair::direction {
thread_local std::mt19937 mt{std::random_device{}()};
}
