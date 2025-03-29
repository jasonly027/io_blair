#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <thread>

#include "server.hpp"

int main() {
  const char* port_str = std::getenv("SERVER_PORT");
  if (port_str == nullptr) {
    std::cerr << "Set SERVER_PORT env to run server.\n";
    return 0;
  }

  constexpr const char* kAddress = "0.0.0.0";
  const auto port                = static_cast<uint16_t>(std::atoi(port_str));
  const auto threads             = std::thread::hardware_concurrency();

  std::make_shared<io_blair::Server>(kAddress, port, threads)->run();
}
