/**
 * @file server.hpp
 */
#pragma once

#include <boost/asio.hpp>
#include <boost/system.hpp>
#include <cstdint>
#include <memory>
#include <string_view>
#include <thread>
#include <vector>

#include "lobby_manager.hpp"

namespace io_blair {

namespace net    = boost::asio;
using tcp        = net::ip::tcp;
using error_code = boost::system::error_code;

/**
 * @brief Server listens for connections and starts a Session for each.
 */
class Server : public std::enable_shared_from_this<Server> {
 public:
  /**
   * @brief Construct a new Server object.
   *
   * @warning Do not directly instantiate Server. This object 
   * must be created as a std::shared_ptr<Server>.
   * 
   * @param address The address to listen on.
   * @param port The port to listen on.
   * @param threads The number of threads the server can use for processing.
   */
  Server(std::string_view address, uint16_t port, uint8_t threads);

  /**
   * @brief Starts the server. 
   *
   * @note Blocks until server termination.
   */
  void run();

 private:
  // Prints ec and what and exits program.
  static void log_fatal(error_code, const char* what);

  // Sets up acceptor to listen for connections.
  void prepare_acceptor(std::string_view address, uint16_t port);

  // Sets up cleanup operations on server termination.
  void prepare_exit();

  // The handler that is called when a connection is accepted.
  void on_accept(error_code, tcp::socket);

  // All async work done by the server and sessions use this io_context.
  net::io_context ctx_;

  // Listens and accepts client connections.
  tcp::acceptor acceptor_;

  // Used to schedule post server termination cleanup.
  net::signal_set exit_signals_;

  // The number of threads the server will utilize.
  uint8_t threads_;

  // The pool of threads the server will use to perform async tasks.
  std::vector<std::jthread> pool_;

  // Each client session is given a reference to this manager to create/join lobbies.
  LobbyManager manager_;
};
}  // namespace io_blair
