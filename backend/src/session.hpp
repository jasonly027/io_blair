#pragma once

#include <memory>
#include <vector>

#include "lobby_manager.hpp"
#include "net_common.hpp"
#include "game.hpp"

namespace io_blair {
class Game;
class Session : public std::enable_shared_from_this<Session> {
   public:
    enum class Error { kFatal, kRecoverable };

    Session(net::io_context& ctx, tcp::socket socket,
            LobbyManager& manager);

    void run();

    void write(const std::shared_ptr<std::string>& str);

    LobbyManager& manager();

   private:
    Error fail(error_code ec, const char* what);

    static void log_err(error_code ec, const char* what);

    void on_accept(error_code ec);

    void on_read(error_code ec, std::size_t bytes);

    void on_write(error_code ec, std::size_t bytes);

    void close();

    tcp::socket socket_;
    websocket::stream<tcp::socket&> ws_;
    beast::flat_static_buffer<500> buffer_;
    LobbyManager& manager_;
    net::strand<net::io_context::executor_type> write_strand_;
    std::vector<std::shared_ptr<const std::string>> queue_;
    Game state_;
};

}  // namespace io_blair
