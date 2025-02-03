#include "server.hpp"

#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "session.hpp"

namespace io_blair {

using std::cerr;
using std::string_view;

namespace ip = net::ip;

Server::Server(string_view address, uint16_t port, uint8_t threads)
    : acceptor_(ctx_), exit_signals_(ctx_, SIGINT, SIGTERM), threads_(threads) {
  prepare_acceptor(address, port);
  prepare_exit();
}

void Server::run() {
  acceptor_.async_accept(ctx_, beast::bind_front_handler(&Server::on_accept, shared_from_this()));

  if (threads_ > 1) {
    for (uint8_t i = 0; i < threads_ - 1; ++i) {
      pool_.emplace_back([self = shared_from_this()] { self->ctx_.run(); });
    }
  }
  ctx_.run();
}

void Server::log_fatal(error_code ec, const char* what) {
  cerr << what << ": " << ec << '\n';
  exit(EXIT_FAILURE);
}

void Server::prepare_acceptor(std::string_view address, uint16_t port) {
  tcp::endpoint endpoint(ip::make_address(address), port);
  error_code ec;

  acceptor_.open(endpoint.protocol(), ec);
  if (ec) {
    log_fatal(ec, "Failed to open acceptor");
  }

  acceptor_.set_option(tcp::acceptor::reuse_address(true), ec);
  if (ec) {
    log_fatal(ec, "Failed to set option to reuse address");
  }

  acceptor_.bind(endpoint, ec);
  if (ec) {
    log_fatal(ec, "Failed to bind to endpoint");
  }

  acceptor_.listen(net::socket_base::max_listen_connections, ec);
  if (ec) {
    log_fatal(ec, "Failed to set acceptor to listen state");
  }
}

void Server::prepare_exit() {
  exit_signals_.async_wait([this](error_code, int) {
    ctx_.stop();

    if (acceptor_.is_open()) {
      acceptor_.close();
    }

    for (auto& thread : pool_) {
      if (thread.joinable()) {
        thread.join();
      }
    }
  });
}

void Server::on_accept(error_code ec, tcp::socket socket) {
  if (!ec) {
#ifndef NDEBUG
    std::cerr << "Connection join\n";
#endif
    Session::make(ctx_, std::move(socket), manager_)->run();
  }
  acceptor_.async_accept(ctx_, beast::bind_front_handler(&Server::on_accept, shared_from_this()));
}
}  // namespace io_blair
