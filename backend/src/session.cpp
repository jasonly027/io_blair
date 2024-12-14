#include "session.hpp"

#include <cstddef>
#include <iostream>

#include "lobby.hpp"

using std::cout, std::cerr, std::string, std::string_view, std::size_t,
    std::shared_ptr;

namespace io_blair {

WebSocketSession::WebSocketSession(net::io_context& ctx, tcp::socket socket,
                                   LobbyManager& manager)
    : socket_(std::move(socket)),
      ws_(socket_),
      write_strand_(net::make_strand(ctx)),
      state_(*this, manager) {
    ws_.set_option(
        websocket::stream_base::timeout::suggested(beast::role_type::server));
}

void WebSocketSession::run() {
    // Expects a Websocket upgrade request
    ws_.async_accept(
        [self = shared_from_this()](error_code ec) { self->on_accept(ec); });
}

void WebSocketSession::write(string msg) {
    net::post(write_strand_, [msg = std::move(msg), self = shared_from_this()] {
        // Add to queue
        self->queue_.push_back(msg);

        /*
            If there there are already messages in the queue,
            the logic of on_write() will already know it needs to
            keep queueing writes.

            Otherwise, this needs to start the write chain.
        */
        if (self->queue_.size() <= 1) self->prepare_for_next_write();
    });
}

shared_ptr<ISession> WebSocketSession::get_shared() {
    return shared_from_this();
}

auto WebSocketSession::fail(error_code ec, const char* what) -> Error {
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

void WebSocketSession::log_err(error_code ec, const char* what) {
    cerr << what << ": " << ec.message() << '\n';
}

void WebSocketSession::on_accept(error_code ec) {
    if (ec && fail(ec, "ws on_accept") == Error::kFatal) return;
    prepare_for_next_read();
}

void WebSocketSession::prepare_for_next_read() {
    ws_.async_read(buffer_,
                   [self = shared_from_this()](error_code ec, size_t bytes) {
                       self->on_read(ec, bytes);
                   });
}

void WebSocketSession::on_read(error_code ec, size_t) {
    if (ec && fail(ec, "ws on_read") == Error::kFatal) return;

    state_.parse(beast::buffers_to_string(buffer_.data()));
    buffer_.consume(buffer_.size());

    prepare_for_next_read();
}

void WebSocketSession::prepare_for_next_write() {
    ws_.async_write(
        net::buffer(queue_.front()),
        net::bind_executor(write_strand_, [self = shared_from_this()](
                                              error_code ec, size_t bytes) {
            self->on_write(ec, bytes);
        }));
}

void WebSocketSession::on_write(error_code ec, size_t bytes [[maybe_unused]]) {
    if (ec && fail(ec, "ws on_write") == Error::kFatal) return;

    // Finished writing msg, remove it from the queue
    queue_.erase(queue_.begin());

    // Continue writing if queue isn't empty
    if (!queue_.empty()) prepare_for_next_write();
}

void WebSocketSession::close() {
    state_.leave();

    // Send close message through Websocket stream
    ws_.async_close(websocket::close_code::normal,
                    [self = shared_from_this()](error_code ec) {
                        if (ec != net::error::operation_aborted)
                            return log_err(ec, "ws async_close");
                    });
}
}  // namespace io_blair
