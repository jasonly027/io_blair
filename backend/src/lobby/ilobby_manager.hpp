/**
 * @file ilobby_manager.hpp
 */
#pragma once

#include <optional>

#include "isession.hpp"
#include "lobby_context.hpp"

namespace io_blair {
/**
 * @brief An interface for a lobby manager that allows sessions to create/join lobbies.
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
   * @brief Attempts to place \p session in a preexisting lobby.
   * 
   * @param session The session trying to join.
   * @param code The join code identifying the lobby to join.
   * @return std::optional<LobbyContext>. Context on the lobby joined
   * or std::nullopt if the lobby was full or couldn't be found.
   */
  virtual std::optional<LobbyContext> join(std::weak_ptr<ISession> session, std::string_view code)
      = 0;

  /**
   * @brief Removes \p session from a preexisting lobby. If the session
   * isn't in the lobby or the join code isn't associated with a lobby, nothing occurs.
   * 
   * @param session The session leaving the lobby.
   * @param code The join code identifying the lobby to leave.
   */
  virtual void leave(const std::weak_ptr<ISession>& session, std::string_view code) = 0;
};

}  // namespace io_blair
