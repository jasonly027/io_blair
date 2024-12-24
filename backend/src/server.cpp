#include "server.hpp"

#include <cassert>
#include <csignal>
#include <cstdlib>
#include <iostream>

#include "lobby.hpp"
#include "net_common.hpp"
#include "session.hpp"

namespace io_blair {
Server::Server(std::string_view address, uint16_t port)
    : socket_(ctx_),
      acceptor_(ctx_),
      signals_(ctx_, SIGINT, SIGTERM),
      manager_(ctx_) {
    prepare_acceptor(address, port);
    prepare_for_termination();
}

int Server::run(int8_t threads) {
    if (threads <= 0) {
        std::cerr << "Thread count should be greater than 0\n";
        return EXIT_FAILURE;
    }

    prepare_for_next_connection();
    start_threads(threads);

    return EXIT_SUCCESS;
}

void Server::log_err(error_code ec, const char* const what) {
    if (ec == net::error::operation_aborted) return;
    std::cerr << what << ": " << ec.what() << '\n';
}

void Server::prepare_acceptor(std::string_view address, uint16_t port) {
    tcp::endpoint endpoint(net::ip::make_address(address), port);
    error_code ec;

    acceptor_.open(endpoint.protocol(), ec);
    if (ec) {
        log_err(ec, "acceptor open");
        return;
    }

    acceptor_.set_option(tcp::acceptor::reuse_address(true), ec);
    if (ec) {
        log_err(ec, "acceptor set_option reuse_address");
        return;
    }

    acceptor_.bind(endpoint, ec);
    if (ec) {
        log_err(ec, "acceptor bind");
        return;
    }

    acceptor_.listen(net::socket_base::max_listen_connections, ec);
    if (ec) {
        log_err(ec, "acceptor listen");
        return;
    }
}

void Server::prepare_for_next_connection() {
    acceptor_.async_accept(socket_, [self = shared_from_this()](error_code ec) {
        self->on_accept(ec);
    });
}

void Server::start_threads(int8_t threads) {
    if (threads > 1) {
        for (int8_t i = 0; i < threads - 1; ++i) {
            pool_.emplace_back([self = shared_from_this()] {
                error_code ec;
                self->ctx_.run(ec);
                if (ec) self->log_err(ec, "ctx_ run");
            });
        }
    }
    // This thread itself will relinquish control and take async work too
    error_code ec;
    ctx_.run(ec);
    if (ec) log_err(ec, "ctx_ run");
}

void Server::prepare_for_termination() {
    signals_.async_wait([this](error_code, int) {
        ctx_.stop();

        if (acceptor_.is_open()) acceptor_.close();

        for (auto& thread : pool_) {
            if (thread.joinable()) thread.join();
        }
    });
}

void Server::on_accept(error_code ec) {
    if (ec) return log_err(ec, "on_accept handler");

    std::cout << "Server accepted\n";

    // When this handler is invoked, socket_ will contain a TCP connection.
    // We construct a new Session instance with the socket.
    std::make_shared<WebSocketSession>(ctx_, std::move(socket_), manager_)
        ->run();

    // Because we moved socket_ on the previous line,
    // socket_ is reset to a state ready to contain a new TCP connection.
    prepare_for_next_connection();
}

}  // namespace io_blair
