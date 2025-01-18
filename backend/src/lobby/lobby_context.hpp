/**
 * @file lobby_context.hpp
 */
#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <string_view>

#include "isession.hpp"

namespace io_blair {
/**
 * @brief Context related to a lobby.
 */
class LobbyContext {
 public:
  explicit LobbyContext(std::string_view code, std::weak_ptr<ISession>& other,
                        std::recursive_mutex& mutex);

  /**
   * @brief Sends a message to the other session in the lobby.
   * If the other session isn't available, nothing occurs.
   * 
   * @param msg The message to send.
   */
  void send_other(std::shared_ptr<const std::string> msg);

  /**
   * @brief Convenience method for send_other() that wraps \p msg
   * in a shared_ptr automatically.
   * 
   * @param msg The message to send.
   */
  void send_other(std::string msg);

  /**
   * @brief The lobby join code.
   */
  const std::string_view code;

 private:
  // Methods should acquire this mutex as ILobbyManager may also acquire it to modify other_.
  std::recursive_mutex& mutex_;

  // Reference to the other session. May be updated by the ILobbyManager on joins/leaves.
  std::weak_ptr<ISession>& other_;
};

}  // namespace io_blair
