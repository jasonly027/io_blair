#include "player.hpp"

#include <utility>

#include "character.hpp"
#include "isession.hpp"


namespace io_blair {
void Player::send(std::shared_ptr<const std::string> msg) {
  session_.async_send(std::move(msg));
}

void Player::send(std::string msg) {
  session_.async_send(std::move(msg));
}

void Player::send(SessionEvent ev) {
  session_.async_handle(ev);
}

bool Player::try_set(std::weak_ptr<ISession> session) {
  return session_.try_set(std::move(session));
}

void Player::reset() {
  session_.reset();
  character = Character::unknown;
  position  = {0, 0};
}

bool Player::exists() const {
  return !session_.expired();
}

ISession& Player::session() {
  return session_;
}

bool operator==(const Player& lhs, const std::shared_ptr<ISession>& rhs) {
  return lhs.session_ == rhs;
}

}  // namespace io_blair
