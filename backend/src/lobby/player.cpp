#include "player.hpp"

#include "character.hpp"


namespace io_blair {
void Player::reset() {
  session.reset();
  character = Character::unknown;
}

bool Player::exists() const {
  return !session.expired();
}

}  // namespace io_blair
