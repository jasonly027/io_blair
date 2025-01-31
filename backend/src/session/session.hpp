/**
 * @file session.hpp
 */
#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/system.hpp>
#include <memory>
#include <vector>

#include "event.hpp"
#include "ihandler.hpp"
#include "isession.hpp"
#include "lobby_manager.hpp"

namespace io_blair {
namespace net       = boost::asio;
using tcp           = net::ip::tcp;
using error_code    = boost::system::error_code;
namespace beast     = boost::beast;
namespace websocket = beast::websocket;

/**
 * @brief Session communicates with the client.
 */
class Session : public ISession, public std::enable_shared_from_this<Session> {
 public:
  /**
   * @brief Creates a new Session object. This function should be used
   * instead of the constructor to have a properly initialized Session.
   * 
   * @param ctx The context used for async operations.
   * @param socket The socket containing the client connection.
   * @param manager The lobby manager.
   * @return std::shared_ptr<Session> 
   */
  static std::shared_ptr<Session> make(net::io_context& ctx, tcp::socket&& socket,
                                       LobbyManager& manager);

  /**
   * @brief Construct a new Session object.
   *
   * @warning Do not directly instantiate a Session with this constructor.
   * Session must be created through Session::make.
   * 
   * @param ctx The context used for async operations.
   * @param socket The socket containing the client connection.
   */
  Session(net::io_context& ctx, tcp::socket&& socket);

  /**
   * @brief Starts the session and immediately returns. Operations are done
   * on the io_context thread(s).
   */
  void run();

  void async_send(std::shared_ptr<const std::string> msg) override;

  void async_send(std::string msg) override;

  void async_handle(SessionEvent) override;

 private:
  // Checks if the error code is fatal, meaning the session should terminate.
  static bool is_fatal(error_code);

  // Declare intent to read from client and immediately return.
  void async_read();

  // Declare intent to write to client and immediately return.
  void async_write();

  // The handler that is called when the client sends data.
  void on_read(error_code, size_t bytes);

  // The handler that is called when send is initiated.
  void on_send(std::shared_ptr<const std::string> msg);

  // The handler that is called after data has been written to the client.
  void on_write(error_code ec, size_t bytes);

  // Holds the client connection.
  websocket::stream<beast::tcp_stream> ws_;

  // Used to store incoming client data.
  beast::flat_static_buffer<512> buffer_;

  using strand = net::strand<net::io_context::executor_type>;
  // Synchronizes reads from the client.
  strand read_strand_;
  // Synchronizes writes to the client.
  strand write_strand_;

  // Stores messages to be sent to the client.
  std::vector<std::shared_ptr<const std::string>> queue_;

  // Handles incoming client data.
  std::unique_ptr<IHandler> handler_;
};

}  // namespace io_blair
