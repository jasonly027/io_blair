#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "server.hpp"

int main(int argc, char** argv) {
  // if (argc != 3) {
  //     std::cerr << "Usage: io_blair_server <address> <port>\n"
  //               << "Example: io_blair_server 0.0.0.0 8080\n";
  //     std::exit(EXIT_FAILURE);
  // }

  // auto* address = argv[1];
  // auto port = static_cast<uint16_t>(std::atoi(argv[2]));
  constexpr const char* kAddress = "0.0.0.0";
  constexpr uint16_t kPort       = 8'080;
  constexpr uint8_t kThreads     = 4;

  std::make_shared<io_blair::Server>(kAddress, kPort, kThreads)->run();
}
