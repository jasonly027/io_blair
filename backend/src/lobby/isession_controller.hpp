/**
 * @file isession_controller.hpp
 */
#pragma once

#include "character.hpp"
#include "maze.hpp"


namespace io_blair {
/**
 * @brief An interface for a session controller. This controller
 * wraps an ILobbyController so that the Player objects
 * don't have to be passed everytime.
 *
 * @see ILobbyController
 */
class ISessionController {
 public:
  virtual ~ISessionController() = default;

  /**
   * @brief Set the character of this session.
   * @param Character The character to set session as.
   */
  virtual void set_character(Character) = 0;

  /**
   * @brief Moves the character of this session.
   * @param coordinate The position to move the character to.
   */
  virtual void move_character(coordinate) = 0;

  /**
   * @brief Check if the game is complete.
   */
  virtual void check_win() = 0;

  /**
   * @brief Starts a new game.
   */
  virtual void new_game() = 0;
};

}  // namespace io_blair
