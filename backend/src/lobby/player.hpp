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
  Character character;

  /**
   * @brief Resets data members.
   */
  void reset();
};

}  // namespace io_blair
