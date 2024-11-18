
#include "session.hpp"

#include <iostream>

#include "boost/asio/error.hpp"
#include "boost/beast/websocket/error.hpp"
#include "net_common.hpp"

namespace io_blair {
Session::Session(tcp::socket socket,
                 const std::shared_ptr<LobbyManager>& manager)
    : socket_(std::move(socket)), ws_(socket_), manager_(manager) {}

void Session::run() {
    ws_.async_accept(
        [self = shared_from_this()](error_code ec) { self->on_accept(ec); });
}

void Session::fail(error_code ec, const char* what) {
    if (ec == net::error::operation_aborted || ec == websocket::error::closed)
        return;
    std::cerr << what << ": " << ec.what() << '\n';
}

void Session::on_accept(error_code ec) {
    if (ec) {
        if (ec != websocket::condition::handshake_failed)
            fail(ec, "ws on_accept");
        return close();
    }

    ws_.async_read(
        buffer_, [self = shared_from_this()](error_code ec, std::size_t bytes) {
            self->on_read(ec, bytes);
        });
}

void Session::on_read(error_code ec, std::size_t bytes) {
    if (ec) {
        fail(ec, "ws on_read");
    } else {
    }

    ws_.async_read(
        buffer_, [self = shared_from_this()](error_code ec, std::size_t bytes) {
            self->on_read(ec, bytes);
        });
}

void Session::close() {
    ws_.async_close(websocket::close_code::normal,
                    [self = shared_from_this()](error_code ec) {
                        if (ec) return fail(ec, "ws async_close");
                    });
}
}  // namespace io_blair
