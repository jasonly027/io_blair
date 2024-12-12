#pragma once

#include <memory>
#include <string>
#include <vector>

#include "game.hpp"
#include "lobby.hpp"
#include "net_common.hpp"

namespace io_blair {

class ISession {
   public:
    virtual ~ISession() = default;
    virtual void run() = 0;
    virtual void write(std::string msg) = 0;
    virtual bool join_new_lobby() = 0;
    virtual bool join_lobby(const std::string& code) = 0;
    virtual void leave_lobby() = 0;
};

class WebSocketSession : public ISession,
                         public std::enable_shared_from_this<WebSocketSession> {
   public:
    template <std::size_t N>
    using buffer = beast::flat_static_buffer<N>;
    using strand = net::strand<net::io_context::executor_type>;
    using msg_queue = std::vector<std::string>;
    using websock = websocket::stream<tcp::socket&>;

    enum class Error {
        // Session should close
        kFatal,
        // Session can continue
        kRecoverable
    };

    WebSocketSession(net::io_context& ctx, tcp::socket socket,
                     LobbyManager& manager);

    // Listen for Websocket upgrade request
    void run() override;

    // Enqueue message to write through Websocket
    void write(std::string msg) override;

    bool join_new_lobby() override;

    bool join_lobby(const std::string& code) override;

    void leave_lobby() override;

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
    websock ws_;
    // Incoming messages are written to this buffer
    buffer<500> buffer_;
    // Synchronizes writes
    strand write_strand_;
    // Queue of messages to be written
    msg_queue queue_;
    // Parses and acts on incoming messages
    Game state_;
    LobbyManager& manager_;
    std::shared_ptr<Lobby> lobby_;
};

}  // namespace io_blair
