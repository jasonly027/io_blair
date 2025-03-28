#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <thread>

#include "server.hpp"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cout << "Usage: ./io_blair_server <port>\n";
    return 0;
  }

  constexpr const char* kAddress = "0.0.0.0";
  const auto port                = static_cast<uint16_t>(std::atoi(argv[1]));
  const auto threads             = std::thread::hardware_concurrency();

  std::make_shared<io_blair::Server>(kAddress, port, threads)->run();
}
