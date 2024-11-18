#include "server.hpp"

#include <cassert>
#include <csignal>
#include <cstdlib>
#include <iostream>

#include "boost/asio/error.hpp"
#include "session.hpp"

namespace io_blair {
Server::Server(std::string_view address, uint16_t port)
    : socket_(ctx_), acceptor_(ctx_), manager_(new LobbyManager()) {
    tcp::endpoint endpoint(net::ip::make_address(address), port);
    error_code ec;

    acceptor_.open(endpoint.protocol(), ec);
    if (ec) {
        fail(ec, "acceptor open");
        return;
    }

    acceptor_.set_option(tcp::acceptor::reuse_address(true), ec);
    if (ec) {
        fail(ec, "acceptor set_option reuse_address");
        return;
    }

    acceptor_.bind(endpoint, ec);
    if (ec) {
        fail(ec, "acceptor bind");
        return;
    }

    acceptor_.listen(net::socket_base::max_listen_connections, ec);
    if (ec) {
        fail(ec, "acceptor listen");
        return;
    }
}

int Server::run(int8_t threads) {
    assert(threads > 0 && "threads must be greater than 0");

    acceptor_.async_accept(socket_, [self = shared_from_this()](error_code ec) {
        self->on_accept(ec);
    });

    net::signal_set signals(ctx_, SIGINT, SIGTERM);
    signals.async_wait([this](error_code, int) { ctx_.stop(); });

    if (threads > 1) {
        for (int8_t i = 0; i < threads - 1; ++i) {
            pool_.emplace_back(
                [self = shared_from_this()] { self->ctx_.run(); });
        }
    }
    ctx_.run();

    return EXIT_SUCCESS;
}

void Server::fail(error_code ec, const char* const what) {
    if (ec == net::error::operation_aborted) return;
    std::cerr << what << ": " << ec.what() << '\n';
}

void Server::on_accept(error_code ec) {
    if (ec) return fail(ec, "on_accept handler");

    std::cout << "Server accepted\n";

    std::make_shared<Session>(std::move(socket_), manager_)->run();

    acceptor_.async_accept(socket_, [self = shared_from_this()](error_code ec) {
        self->on_accept(ec);
    });
}

}  // namespace io_blair
