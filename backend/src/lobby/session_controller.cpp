#include "session_controller.hpp"


namespace io_blair {
SessionController::SessionController(Player& self, Player& other, ILobbyController& controller)
    : self_(self), other_(other), controller_(controller) {}

void SessionController::set_character(Character character) {
  controller_.set_character(self_, other_, character);
}

}  // namespace io_blair
