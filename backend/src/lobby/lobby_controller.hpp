/**
 * @file lobby_controller.hpp
 */
#pragma once

#include <memory>
#include <mutex>
#include <optional>
#include <string>

#include "character.hpp"
#include "ilobby_controller.hpp"
#include "isession.hpp"
#include "lobby_context.hpp"
#include "player.hpp"


namespace io_blair {
/**
 * @brief A thread-safe ILobbyController.
 */
class LobbyController : public ILobbyController {
 public:
  /**
   * @brief Construct a new Lobby Controller object.
   * 
   * @param code The lobby's join code.
   */
  explicit LobbyController(std::string code);

  /**
   * @brief Tries to place \p session into the lobby. The session
   * will be sent a json::out::lobbyJoin object with the result. The other
   * session in the lobby is sent a json::out::lobbyOtherJoin if joining was
   * successful.
   * 
   * @param session The session to try placing.
   * @return std::optional<LobbyContext> The context of the lobby of
   * placing was successful, otherwise nullopt.
   */
  std::optional<LobbyContext> join(std::weak_ptr<ISession> session) override;

  /**
   * @brief Removes \p session from the lobby. Sends json::out::lobbyOtherLeave to
   * the other session in the lobby. Does nothing if session isn't
   * in this lobby.
   * 
   * @param session The session to remove.
   */
  void leave(const std::weak_ptr<ISession>& session) override;

  /**
   * @brief Determines if the lobby has any existing players.
   * 
   * @return true The lobby has no existing players.
   * @return false The lobby has some existing players.
   */
  bool empty() const override;

  /**
   * @brief Sets the character of \p self. Setting will not happen
   * if \p self is already \p character or \p other is already \p character
   * (unless \p character is unknown as both self and other can be unknown
   * at the same time). The other player is sent a json::out::characterConfirm.
   * If both players are no longer unknown after setting, SessionEvent::kTransitionToInGame
   * and json::out::transitionToInGame are sent to both players.
   * 
   * @param self 
   * @param other 
   * @param character 
   */
  void set_character(Player& self, Player& other, Character character) override;

  /**
   * @brief The lobby's join code.
   */
  const std::string code_;

 private:
  mutable std::mutex mutex_;

  Player p1_;
  Player p2_;
};

}  // namespace io_blair
