/**
 * @file player.hpp
 */
#pragma once

#include "character.hpp"
#include "session_view.hpp"


namespace io_blair {
/**
 * @brief Represents a player in a lobby.
 */
struct Player {
  SessionView session;
  Character character = Character::unknown;

  /**
   * @brief Resets data members.
   */
  void reset();

  /**
   * @brief Determines if session hasn't expired.
   * 
   * @return true The session hasn't expired.
   * @return false The session expired.
   */
  bool exists() const;
};

}  // namespace io_blair
