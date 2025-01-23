/**
 * @file lobby_data.hpp
 */
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "isession.hpp"
#include "lobby_context.hpp"
#include "session_view.hpp"

namespace io_blair {
class LobbyData {
 public:
  /**
   * @brief Construct a new Lobby Data object.
   * 
   * @param code The lobby's join code.
   */
  explicit LobbyData(std::string code);

  /**
   * @brief Attempts to place \p session into the lobby.
   * Operation will succeed if the lobby isn't full or
   * at least one of the sessions already in the lobby
   * has expired.
   * 
   * @param session The session trying to join.
   * @return LobbyContext. The session was
   * successfully placed into the lobby.
   * @return nullopt. The session couldn't be placed
   * into the lobby, indicating it was full.
   */
  std::optional<LobbyContext> join(std::weak_ptr<ISession> session);

  /**
   * @brief Removes \p session from the lobby. If the session
   * isn't in the lobby, nothing occurs.
   * 
   * @param session The session leaving the lobby.
   */
  void leave(const std::weak_ptr<ISession>& session);

  /**
   * @brief The lobby's join code.
   */
  const std::string code_;

 private:
  std::optional<LobbyContext> join(std::weak_ptr<ISession> session, SessionView& a, SessionView& b);

  static bool leave(const std::weak_ptr<ISession>& session, SessionView& a, SessionView& b);

  // Represent the two possible sessions in the lobby.
  SessionView p1_;
  SessionView p2_;
};

}  // namespace io_blair
