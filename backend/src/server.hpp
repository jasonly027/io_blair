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
    Server(std::string_view address, uint16_t port);

    int run(int8_t threads = 1);

   private:
    static void log_err(error_code ec, const char* what);

    void on_accept(error_code ec);

    net::io_context ctx_;
    tcp::socket socket_;
    tcp::acceptor acceptor_;
    std::vector<std::thread> pool_;
    std::shared_ptr<LobbyManager> manager_;
};
}  // namespace io_blair
