#include "session.hpp"

#include <iostream>
#include <memory>
#include <utility>

#include "event.hpp"
#include "handler.hpp"
#include "json.hpp"
#include "session_context.hpp"


namespace io_blair {
using std::shared_ptr;
using std::string;

Session::Session(net::io_context& ctx, tcp::socket&& socket)
    : ws_(std::move(socket)),
      read_strand_(net::make_strand(ctx)),
      write_strand_(net::make_strand(ctx)),
      handler_(nullptr) {
  ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));
}

std::shared_ptr<Session> Session::make(net::io_context& ctx, tcp::socket&& socket,
                                       LobbyManager& manager) {
  auto session = std::make_shared<Session>(ctx, std::move(socket));

  // The reason Session couldn't be properly initialized with just the c'tor
  // is because the handler we want to use requires a shared_ptr to the session
  // to construct.
  session->handler_ = std::make_unique<Game>(SessionContext{session, manager});

  return session;
}

void Session::run() {
  ws_.async_accept([self = shared_from_this()](error_code ec) {
    if (!ec) {
      self->async_read();
    }
  });
}

void Session::async_send(shared_ptr<const string> msg) {
  net::post(write_strand_,
            beast::bind_front_handler(&Session::on_send, shared_from_this(), std::move(msg)));
}

void Session::async_send(std::string msg) {
  async_send(std::make_shared<const string>(std::move(msg)));
}


void Session::async_handle(SessionEvent ev) {
  net::post(read_strand_, [self = shared_from_this(), ev] { (*self->handler_)(ev); });
}

bool Session::is_fatal(error_code ec) {
#ifndef NDEBUG
  std::cerr << "Websocket read err: " << ec << '\n';
#endif
  return ec == websocket::error::closed || ec == net::error::connection_aborted
         || ec == net::error::operation_aborted;
}

void Session::async_read() {
  ws_.async_read(buffer_, beast::bind_front_handler(&Session::on_read, shared_from_this()));
}

void Session::async_write() {
  ws_.async_write(
      net::buffer(*queue_.front()),
      net::bind_executor(write_strand_,
                         beast::bind_front_handler(&Session::on_write, shared_from_this())));
}

void Session::on_read(error_code ec, size_t) {
  if (ec) {
    if (!is_fatal(ec)) {
      async_read();
    } else {
      async_handle(SessionEvent::kCloseSession);
    }
    return;
  }

  net::post(read_strand_,
            [self = shared_from_this(), data = beast::buffers_to_string(buffer_.data())]() {
              json::decode(data, *self->handler_);
            });
  buffer_.consume(buffer_.size());

  async_read();
}

void Session::on_send(shared_ptr<const string> msg) {
  queue_.push_back(std::move(msg));

  if (queue_.size() > 1) {
    return;
  }
  async_write();
}

void Session::on_write(error_code, size_t) {
  queue_.erase(queue_.begin());

  if (queue_.empty()) {
    return;
  }
  async_write();
}

}  // namespace io_blair
