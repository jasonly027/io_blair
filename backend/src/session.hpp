#pragma once

#include <memory>

#include "boost/beast/core/flat_static_buffer.hpp"
#include "lobby_manager.hpp"
#include "net_common.hpp"

namespace io_blair {
class Session : public std::enable_shared_from_this<Session> {
   public:
    Session(tcp::socket socket, const std::shared_ptr<LobbyManager>& manager);

    void run();

   private:
    static void fail(error_code ec, const char* what);

    void on_accept(error_code ec);

    void on_read(error_code ec, std::size_t bytes);

    void close();

    tcp::socket socket_;
    websocket::stream<tcp::socket&> ws_;
    beast::flat_static_buffer<500> buffer_;
    std::shared_ptr<LobbyManager> manager_;
};

}  // namespace io_blair
