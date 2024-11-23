
#include "session.hpp"

#include <iostream>
#include <memory>
#include <string>

#include "boost/asio/bind_executor.hpp"
#include "boost/asio/io_context.hpp"
#include "boost/asio/post.hpp"
#include "net_common.hpp"

using std::cout, std::cerr;

namespace io_blair {
Session::Session(net::io_context& ctx, tcp::socket socket,
                 const std::shared_ptr<LobbyManager>& manager)
    : socket_(std::move(socket)),
      ws_(socket_),
      manager_(manager),
      write_strand_(net::make_strand(ctx)),
      state_(manager_) {
    ws_.set_option(
        websocket::stream_base::timeout::suggested(beast::role_type::server));
}

void Session::run() {
    ws_.async_accept(
        [self = shared_from_this()](error_code ec) { self->on_accept(ec); });
}

Session::Error Session::fail(error_code ec, const char* what) {
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

    ws_.async_read(
        buffer_, [self = shared_from_this()](error_code ec, std::size_t bytes) {
            self->on_read(ec, bytes);
        });
}

void Session::on_read(error_code ec, std::size_t bytes) {
    if (ec && fail(ec, "ws on_read") == Error::kFatal) return;

    const beast::string_view sv(net::buffer_cast<const char*>(buffer_.data()),
                                bytes);
    if (auto reply = state_.parse(sv)) {
        write(*reply);
    }

    ws_.async_read(
        buffer_, [self = shared_from_this()](error_code ec, std::size_t bytes) {
            self->on_read(ec, bytes);
        });
}

void Session::write(const std::shared_ptr<std::string>& str) {
    net::post(write_strand_, [str, self = shared_from_this()] {
        self->queue_.push_back(str);

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

void Session::on_write(error_code ec, std::size_t bytes [[maybe_unused]]) {
    if (ec && fail(ec, "ws on_write") == Error::kFatal) return;

    queue_.erase(queue_.begin());

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
    ws_.async_close(websocket::close_code::normal,
                    [self = shared_from_this()](error_code ec) {
                        if (ec != net::error::operation_aborted)
                            return log_err(ec, "ws async_close");
                    });
}
}  // namespace io_blair
