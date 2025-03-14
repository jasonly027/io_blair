#include "lobby_controller.hpp"

#include <memory>
#include <mutex>
#include <optional>

#include "character.hpp"
#include "event.hpp"
#include "json.hpp"
#include "session_controller.hpp"


namespace io_blair {
using std::make_shared;
using std::make_unique;
using std::nullopt;
using std::optional;
using std::string;
using std::weak_ptr;
using guard    = std::lock_guard<std::recursive_mutex>;
namespace jout = json::out;

namespace {
constexpr coordinate kMazeStart = {1, 6};
constexpr coordinate kMazeEnd   = {6, 1};
}  // namespace

LobbyController::LobbyController(string code)
    : code_(std::move(code)), maze_(kMazeStart, kMazeEnd) {
  static_assert(Maze::in_range(kMazeStart));
  static_assert(Maze::in_range(kMazeEnd));
}

optional<LobbyContext> LobbyController::join(weak_ptr<ISession> session) {
  guard lock(mutex_);

  if (p1_.try_set(session)) {
    p1_.send(
        jout::lobby_join(code_, static_cast<int>(p1_.exists()) + static_cast<int>(p2_.exists())));
    p2_.send(jout::lobby_other_join());
    return LobbyContext{code_, p2_.session(), make_unique<SessionController>(p1_, p2_, *this)};
  }

  if (p2_.try_set(session)) {
    p2_.send(
        jout::lobby_join(code_, static_cast<int>(p1_.exists()) + static_cast<int>(p2_.exists())));
    p1_.send(jout::lobby_other_join());
    return LobbyContext{code_, p1_.session(), make_unique<SessionController>(p2_, p1_, *this)};
  }

  session.lock()->async_send(jout::lobby_join(nullopt, nullopt));
  return nullopt;
}

void LobbyController::leave(const weak_ptr<ISession>& session) {
  guard lock(mutex_);
  auto sess = session.lock();

  if (sess == p1_) {
    p1_.reset();
    p2_.send(jout::lobby_other_leave());
    return;
  }

  if (sess == p2_) {
    p2_.reset();
    p1_.send(jout::lobby_other_leave());
    return;
  }
}

bool LobbyController::empty() const {
  guard lock(mutex_);
  return !p1_.exists() && !p2_.exists();
}

void LobbyController::set_character(Player& self, Player& other, Character character) {
  guard lock(mutex_);

  // Self is already character
  if (character == self.character) {
    return;
  }
  // Other is already character
  if (character != Character::unknown && character == other.character) {
    return;
  }

  self.character = character;
  other.send(jout::character_confirm(character));

  // If either hasn't chosen a character, don't transition to game yet.
  if (self.character == Character::unknown || other.character == Character::unknown) {
    return;
  }

  start_game();
}

void LobbyController::start_game() {
  guard lock(mutex_);

  auto msg = make_shared<const string>(jout::transition_to_ingame());
  p1_.send(SessionEvent::kTransitionToInGame);
  p1_.send(msg);
  p2_.send(SessionEvent::kTransitionToInGame);
  p2_.send(std::move(msg));

  maze_.randomize();
  p1_.position = kMazeStart;
  p2_.position = kMazeStart;

  p1_.send(jout::ingame_maze(maze_, p1_.character));
  p2_.send(jout::ingame_maze(maze_, p2_.character));
}

}  // namespace io_blair
