#include "lobby_controller.hpp"

#include <memory>
#include <mutex>

#include "json.hpp"
#include "session_controller.hpp"


namespace io_blair {
using std::make_unique;
using std::nullopt;
using std::optional;
using std::string;
using std::weak_ptr;
using guard    = std::lock_guard<std::mutex>;
namespace jout = json::out;

LobbyController::LobbyController(string code)
    : code_(std::move(code)) {}

optional<LobbyContext> LobbyController::join(weak_ptr<ISession> session) {
  guard lock(mutex_);

  if (p1_.session.try_set(session)) {
    p2_.session.async_send(jout::lobby_other_join());
    return LobbyContext{code_, p2_.session, make_unique<SessionController>(p1_, p2_, *this)};
  }

  if (p2_.session.try_set(std::move(session))) {
    p1_.session.async_send(jout::lobby_other_join());
    return LobbyContext{code_, p1_.session, make_unique<SessionController>(p2_, p1_, *this)};
  }

  return nullopt;
}

void LobbyController::leave(const weak_ptr<ISession>& session) {
  guard lock(mutex_);

  auto sess = session.lock();
  if (sess == p1_.session) {
    p1_.session.reset();
    p2_.session.async_send(jout::lobby_other_leave());
  } else if (sess == p2_.session) {
    p2_.session.reset();
    p1_.session.async_send(jout::lobby_other_leave());
  }
}

void LobbyController::set_character(Player& self, Player& other, Character character) {}
}  // namespace io_blair
