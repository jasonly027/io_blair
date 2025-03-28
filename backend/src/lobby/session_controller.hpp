/**
 * @file session_controller.hpp
 */
#pragma once

#include "character.hpp"
#include "ilobby_controller.hpp"
#include "isession_controller.hpp"
#include "maze.hpp"
#include "player.hpp"


namespace io_blair {
class SessionController : public ISessionController {
 public:
  /**
   * @brief Construct a new Session Controller object.
   * 
   * @param self 
   * @param other 
   * @param controller The controller this wraps.
   */
  SessionController(Player& self, Player& other, ILobbyController&);

  void set_character(Character) override;

  void move_character(coordinate) override;

  void check_win() override;

  void new_game() override;

 private:
  // The self and other passed to the underlying controller.
  Player& self_;
  Player& other_;

  // The underlying controller.
  ILobbyController& controller_;
};
}  // namespace io_blair
