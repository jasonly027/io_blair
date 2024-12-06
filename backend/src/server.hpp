#pragma once

#include <cstdint>
#include <memory>
#include <string_view>
#include <thread>
#include <vector>

#include "net_common.hpp"
#include "lobby.hpp"

namespace io_blair {

class Server : public std::enable_shared_from_this<Server> {
   public:
    using thread_pool = std::vector<std::thread>;

    // Create server at (address) listening on (port)
    Server(std::string_view address, uint16_t port);

    /*
        Start listening for connection requests
        (threads) is number of threads to create
        that can accept async work
    */
    int run(int8_t threads = 1);

   private:
    // Prints (ec) and (what) to stderr
    static void log_err(error_code ec, const char* what);

    // Setup acceptor to listen for connections
    void prepare_acceptor(std::string_view address, uint16_t port);

    // Enqueue that we're ready for a new connection
    void prepare_for_next_connection();

    // Enqueue handler that should be invoked when server terminates
    void prepare_for_termination();

    // Start threads that will take on async work
    void start_threads(int8_t threads);

    // Handler that is invoked when a connection is accepted
    void on_accept(error_code ec);

    net::io_context ctx_;
    // The socker a new connection will have
    tcp::socket socket_;
    tcp::acceptor acceptor_;
    net::signal_set signals_;
    // Pool of threads that can accept work
    thread_pool pool_;
    LobbyManager manager_;
};
}  // namespace io_blair
