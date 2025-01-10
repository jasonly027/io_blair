#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "game.hpp"
#include "lobby.hpp"
#include "net_common.hpp"

namespace io_blair {

class ISession {
 public:
  virtual ~ISession() = default;

  enum class State : int8_t { kPrelobby, kWaitingForLobby, kInLobby };

  virtual void run()                  = 0;
  virtual void write(std::string msg) = 0;

  virtual State state() const         = 0;
  virtual void set_state(State state) = 0;

  virtual void try_lobby_update(std::string data)       = 0;
  virtual void set_lobby(std::shared_ptr<ILobby> lobby) = 0;

  virtual std::shared_ptr<ISession> get_shared()             = 0;
  virtual std::shared_ptr<const ISession> get_shared() const = 0;
};

class WebSocketSession : public ISession, public std::enable_shared_from_this<WebSocketSession> {
 public:
  enum class Error {
    // Session should close
    kFatal,
    // Session can continue
    kRecoverable
  };

  WebSocketSession(net::io_context& ctx, tcp::socket socket, LobbyManager& manager);

  // Listen for Websocket upgrade request
  void run() override;

  // Enqueue msg to client
  void write(std::string msg) override;

  State state() const override;
  void set_state(State state) override;

  void try_lobby_update(std::string data) override;
  void set_lobby(std::shared_ptr<ILobby> lobby) override;

  std::shared_ptr<ISession> get_shared() override;

  std::shared_ptr<const ISession> get_shared() const override;

 private:
  using strand = net::strand<net::io_context::executor_type>;

  // If error was recoverable, invoke log_err().
  // Otherwise, close session.
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

  void try_lobby_leave();

  // Return value may change on repeated calls.
  // STORE THE RETURN VALUE in a local variable before using it.
  std::shared_ptr<ILobby> lobby_unsafe();

  tcp::socket socket_;
  // Websocket
  websocket::stream<tcp::socket&> ws_;
  // Incoming messages are written to this buffer
  beast::flat_static_buffer<500> buffer_;
  // Synchronizes reads
  strand read_strand_;
  // Synchronizes writes
  strand write_strand_;
  // Queue of messages to be written
  std::vector<std::string> queue_;
  std::atomic<State> state_;
  // Parses and acts on incoming messages
  Game game_;
  // Mutex for lobby_unsafe_
  std::mutex lobby_mutex_;
  // DO NOT use this directly. Use the setter/getter accordingly
  std::shared_ptr<ILobby> lobby_unsafe_;
};

}  // namespace io_blair
