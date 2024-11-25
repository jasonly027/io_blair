#include "session.hpp"

#include <iostream>
#include <string>

#include "lobby.hpp"
#include "lobby_manager.hpp"

using std::cout, std::cerr, std::shared_ptr;

namespace io_blair {
Session::Session(net::io_context& ctx, tcp::socket socket,
                 LobbyManager& manager)
    : socket_(std::move(socket)),
      ws_(socket_),
      write_strand_(net::make_strand(ctx)),
      state_(*this),
      manager_(manager) {
    ws_.set_option(
        websocket::stream_base::timeout::suggested(beast::role_type::server));
}

void Session::run() {
    // Expects a Websocket upgrade request
    ws_.async_accept(
        [self = shared_from_this()](error_code ec) { self->on_accept(ec); });
}

void Session::write(const std::shared_ptr<std::string>& str) {
    net::post(write_strand_, [str, self = shared_from_this()] {
        // Add to queue
        self->queue_.push_back(str);

        /*
            If there there are already messages in the queue,
            the logic of on_write() will already know it needs to
            keep queueing async_write().

            Otherwise, this needs to start the async_write() chain.
        */
        if (self->queue_.size() > 1) return;
        self->ws_.async_write(
            net::buffer(*self->queue_.front()),
            net::bind_executor(self->write_strand_,
                               [self = self->shared_from_this()](
                                   error_code ec, std::size_t bytes) {
                                   self->on_write(ec, bytes);
                               }));
    });
}

LobbyManager& Session::manager() { return manager_; }

shared_ptr<Lobby>& Session::lobby() { return lobby_; }

Session::Error Session::fail(error_code ec, const char* what) {
    // If (ec) is one of the following errors, the session must close
    if (ec == websocket::condition::handshake_failed ||
        ec == net::error::operation_aborted ||
        ec == net::error::connection_aborted ||
        ec == net::error::connection_reset || ec == websocket::error::closed) {
        cout << "Closing CONN\n";
        close();
        return Error::kFatal;
    }
    log_err(ec, what);
    return Error::kRecoverable;
}

void Session::log_err(error_code ec, const char* what) {
    cerr << what << ": " << ec.message() << '\n';
}

void Session::on_accept(error_code ec) {
    if (ec && fail(ec, "ws on_accept") == Error::kFatal) return;

    // If no errors, we can now start listening for messages
    ws_.async_read(
        buffer_, [self = shared_from_this()](error_code ec, std::size_t bytes) {
            self->on_read(ec, bytes);
        });
}

void Session::on_read(error_code ec, std::size_t) {
    if (ec && fail(ec, "ws on_read") == Error::kFatal) return;

    // Let state_ parse incoming message
    state_.parse(beast::buffers_to_string(buffer_.data()));
    buffer_.consume(buffer_.size());

    // Wait for a new incoming message
    ws_.async_read(
        buffer_, [self = shared_from_this()](error_code ec, std::size_t bytes) {
            self->on_read(ec, bytes);
        });
}

void Session::on_write(error_code ec, std::size_t bytes [[maybe_unused]]) {
    if (ec && fail(ec, "ws on_write") == Error::kFatal) return;

    // We can now erase it from the queue
    queue_.erase(queue_.begin());

    // Continue writing if queue isn't empty
    if (!queue_.empty()) {
        ws_.async_write(
            net::buffer(*queue_.front()),
            net::bind_executor(
                write_strand_,
                [self = shared_from_this()](error_code ec, std::size_t bytes) {
                    self->on_write(ec, bytes);
                }));
    }
}

void Session::close() {
    // Send close message through Websocket stream
    ws_.async_close(websocket::close_code::normal,
                    [self = shared_from_this()](error_code ec) {
                        if (ec != net::error::operation_aborted)
                            return log_err(ec, "ws async_close");
                    });
}
}  // namespace io_blair
