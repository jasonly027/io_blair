/**
 * @file player.hpp
 */
#pragma once

#include <memory>
#include <string>

#include "character.hpp"
#include "event.hpp"
#include "isession.hpp"
#include "maze.hpp"
#include "session_view.hpp"


namespace io_blair {
/**
 * @brief Represents a player in a lobby.
 */
class Player {
 public:
  /**
   * @brief Convenience method that forwards to Player::session().
   * 
   * @param msg 
   */
  void send(std::shared_ptr<const std::string> msg);

  /**
   * @brief Convenience method that forwards to Player::session().
   * 
   * @param msg 
   */
  void send(std::string msg);

  /**
   * @brief Convenience method that forwards to Player::session()'s
   * async_handle.
   * 
   * @param msg 
   */
  void send(SessionEvent ev);

  /**
   * @brief Convenience method for underlying SessionView.
   * 
   * @param session
   * @return true 
   * @return false 
   */
  bool try_set(std::weak_ptr<ISession> session);

  /**
   * @brief Resets data members.
   * 
   * @param reset_session Determine whether to also reset the session.
   */
  void reset(bool reset_session);

  /**
   * @brief Determines if session hasn't expired.
   * 
   * @return true The session hasn't expired.
   * @return false The session expired.
   */
  bool exists() const;

  /**
   * @brief Gets the underlying session.
   * 
   * @return ISession& 
   */
  ISession& session();

  /**
   * @brief Convenience method for comparing with underlying SessionView.
   * 
   * @param lhs 
   * @param rhs 
   * @return true 
   * @return false 
   */
  friend bool operator==(const Player& lhs, const std::shared_ptr<ISession>& rhs);

  /**
   * @brief The character of the player.
   */
  Character character = Character::unknown;

  /**
   * @brief The position of the player in the maze.
   */
  coordinate position;

 private:
  SessionView session_;
};

}  // namespace io_blair
