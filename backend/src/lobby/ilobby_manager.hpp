/**
 * @file ilobby_manager.hpp
 */
#pragma once

#include <optional>

#include "isession.hpp"
#include "lobby_context.hpp"

namespace io_blair {
/**
 * @brief An interface for a manager that allows sessions to create/join lobbies.
 */
class ILobbyManager {
 public:
  virtual ~ILobbyManager() = default;

  /**
   * @brief Creates a lobby with \p session in it.
   * 
   * @param session The session joining the lobby.
   * @return LobbyContext. Context on the lobby joined.
   */
  virtual LobbyContext create(std::weak_ptr<ISession> session) = 0;

  /**
   * @brief Attempts to place \p session in a preexisting lobby. If the lobby is full
   * or there isn't one with a matching code, std::nullopt will be returned.
   * 
   * @param session The session trying to join.
   * @param code The join code identifying the lobby to join.
   * @return std::optional<LobbyContext>. Context on the lobby joined
   * or std::nullopt if the lobby was full or couldn't be found.
   */
  virtual std::optional<LobbyContext> join(std::weak_ptr<ISession> session, std::string_view code)
      = 0;
};

}  // namespace io_blair
