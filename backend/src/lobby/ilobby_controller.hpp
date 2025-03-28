/**
 * @file ilobby_controller.hpp
 */
#pragma once

#include <memory>
#include <optional>

#include "character.hpp"
#include "isession.hpp"
#include "lobby_context.hpp"
#include "maze.hpp"
#include "player.hpp"


namespace io_blair {
/**
 * @brief An interface for a lobby controller that allows sessions to join
 * and leave. Player's are modified through the controller.
 */
class ILobbyController {
 public:
  virtual ~ILobbyController() = default;

  /**
   * @brief Set \p self 's character. Does not set \p self if \p character
   * is already chosen by \p other.
   * 
   * @param self 
   * @param other 
   * @param character The character to set \p self as. 
   */
  virtual void set_character(Player& self, Player& other, Character character) = 0;

  /**
   * @brief Moves self's character. Will reset the \p self 's position if
   * the traversal is not permitted by the maze.
   * 
   * @param self 
   * @param other 
   * @param coordinate The coordinate to move to.
   */
  virtual void move_character(Player& self, Player& other, coordinate coordinate) = 0;

  /**
   * @brief Check if the game is complete.
   */
  virtual void check_win() = 0;

  /**
   * @brief Starts a new game.
   */
  virtual void new_game() = 0;

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
  virtual std::optional<LobbyContext> join(std::weak_ptr<ISession> session) = 0;

  /**
   * @brief Removes \p session from the lobby. If \p session
   * isn't in the lobby, nothing occurs.
   * 
   * @param session The session leaving the lobby.
   */
  virtual void leave(const std::weak_ptr<ISession>& session) = 0;

  /**
   * @brief Determines whether the lobby has no sessions in it.
   * 
   * @return true The lobby is empty.
   * @return false The lobby isn't empty.
   */
  virtual bool empty() const = 0;
};

}  // namespace io_blair
