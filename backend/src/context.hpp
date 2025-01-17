#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "lobby.hpp"
#include "session.hpp"

namespace io_blair {

/**
 * @brief Context related to a Session.
 */
struct SessionContext {
  void send(std::shared_ptr<const std::string> msg);

  void send(std::string msg);

  const std::shared_ptr<ISession> session;
  ILobbyManager& lobby_manager;
};

/**
 * @brief Context related to a lobby.
 */
class LobbyContext {
 public:
  std::string_view code;

 private:
};

}  // namespace io_blair
