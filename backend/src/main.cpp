#include <cstdint>
#include <memory>
#include <thread>

#include "server.hpp"

int main() {
  constexpr const char* kAddress = "0.0.0.0";
  constexpr uint16_t kPort       = 8'080;
  const auto threads             = std::thread::hardware_concurrency();

  std::make_shared<io_blair::Server>(kAddress, kPort, threads)->run();
}
