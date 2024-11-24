#pragma once

#include <cstdint>
#include <memory>
#include <string_view>
#include <thread>
#include <vector>

#include "lobby_manager.hpp"
#include "net_common.hpp"

namespace io_blair {

class Server : public std::enable_shared_from_this<Server> {
   public:
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

    // Handler that is invoked when a connection is accepted
    void on_accept(error_code ec);

    net::io_context ctx_;
    // The socker a new connection will have
    tcp::socket socket_;
    tcp::acceptor acceptor_;
    // Pool of threads that can accept work
    std::vector<std::thread> pool_;
    LobbyManager manager_;
};
}  // namespace io_blair
