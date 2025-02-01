#include "player.hpp"

#include "character.hpp"


namespace io_blair {
void Player::reset() {
  session.reset();
  character = Character::unknown;
}

}  // namespace io_blair
