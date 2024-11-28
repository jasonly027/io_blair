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
    template <std::size_t N>
    using static_buffer = beast::flat_static_buffer<N>;
    using strand = net::strand<net::io_context::executor_type>;
    using message_queue = std::vector<std::shared_ptr<const std::string>>;

    enum class Error {
        // Session should close
        kFatal,
        // Session can continue
        kRecoverable
    };

    Session(net::io_context& ctx, tcp::socket socket);

    // Listen for Websocket upgrade request
    void run();

    // Queue message to send through Websocket
    void write(const std::shared_ptr<std::string>& str);

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

    // Enqueue that we're ready to read the next msg
    void prepare_for_next_read();

    // Handler that is invoked when a message was read
    void on_read(error_code ec, std::size_t bytes);

    // Enqueue that we're ready to write the next msg
    void prepare_for_next_write();

    // Handler that is invoked when a message was written
    void on_write(error_code ec, std::size_t bytes);

    // Close session
    void close();

    tcp::socket socket_;
    // Websocket
    websocket::stream<tcp::socket&> ws_;
    // Incoming messages are written to this buffer
    static_buffer<500> buffer_;
    // Synchronizes writes
    strand write_strand_;
    // Queue of messages to be written
    message_queue queue_;
    // Parses and acts on incoming messages
    Game state_;
    std::shared_ptr<Lobby> lobby_;
};

}  // namespace io_blair
