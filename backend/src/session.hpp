#pragma once

#include <memory>
#include <vector>

#include "game.hpp"
#include "net_common.hpp"

namespace io_blair {
class LobbyManager;
class Lobby;
class Session : public std::enable_shared_from_this<Session> {
   public:
    enum class Error {
        // Session should close
        kFatal,
        // Session can continue
        kRecoverable
    };

    Session(net::io_context& ctx, tcp::socket socket, LobbyManager& manager);

    // Listen for Websocket upgrade request
    void run();

    // Queue message to send through Websocket
    void write(const std::shared_ptr<std::string>& str);

    LobbyManager& manager();

    std::shared_ptr<Lobby>& lobby();

   private:
    /*
        If error was recoverable, invoke log_err().
        Otherwise, close session.
    */
    Error fail(error_code ec, const char* what);

    // Print (ec) and (what) to stderr
    static void log_err(error_code ec, const char* what);

    // Handler that is invoked when a connection was accepted
    void on_accept(error_code ec);

    // Handler that is invoked when a message was read
    void on_read(error_code ec, std::size_t bytes);

    // Handler that is invoked when a message was written
    void on_write(error_code ec, std::size_t bytes);

    // Close session
    void close();

    tcp::socket socket_;
    // Websocket
    websocket::stream<tcp::socket&> ws_;
    // Incoming messages are written to this buffer
    beast::flat_static_buffer<500> buffer_;
    // Synchronizes writes
    net::strand<net::io_context::executor_type> write_strand_;
    // Queue of messages to be written
    std::vector<std::shared_ptr<const std::string>> queue_;
    // Parses and acts on incoming messages
    Game state_;
    LobbyManager& manager_;
    std::shared_ptr<Lobby> lobby_;
};

}  // namespace io_blair
