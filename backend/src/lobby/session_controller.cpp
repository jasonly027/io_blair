#include "session_controller.hpp"

#include "maze.hpp"


namespace io_blair {
SessionController::SessionController(Player& self, Player& other, ILobbyController& controller)
    : self_(self), other_(other), controller_(controller) {}

void SessionController::set_character(Character character) {
  controller_.set_character(self_, other_, character);
}

void SessionController::move_character(coordinate coordinate) {
  controller_.move_character(self_, other_, coordinate);
}

void SessionController::check_win() {
  controller_.check_win();
}

void SessionController::new_game() {
  controller_.new_game();
}

}  // namespace io_blair
