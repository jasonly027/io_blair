#include "lobby_controller.hpp"

#include <memory>
#include <mutex>
#include <optional>

#include "character.hpp"
#include "event.hpp"
#include "json.hpp"
#include "maze.hpp"
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
constexpr coordinate kMazeStart = {1, 4};
constexpr coordinate kMazeEnd   = {4, 1};

// Gets the direction start needs to move to reach end.
// Returns nullopt if coordinates aren't cardinal direction neighbors.
constexpr optional<jout::Direction> to_dir(coordinate start, coordinate end) {
  auto [x1, y1] = start;
  auto [x2, y2] = end;

  if (x1 == x2) {
    // a
    // ↓
    // b
    if (y1 + 1 == y2) return json::out::Direction::down;
    // b
    // ↑
    // a
    if (y1 - 1 == y2) return json::out::Direction::up;
  }

  if (y1 == y2) {
    // a → b
    if (x1 + 1 == x2) return json::out::Direction::right;
    // b ← a
    if (x1 - 1 == x2) return json::out::Direction::left;
  }

  return nullopt;
}
}  // namespace

LobbyController::LobbyController(string code)
    : code_(std::move(code)), maze_(kMazeStart, kMazeEnd) {
  static_assert(Maze::in_range(kMazeStart));
  static_assert(Maze::in_range(kMazeEnd));
}

optional<LobbyContext> LobbyController::join(weak_ptr<ISession> session) {
  guard lock(mutex_);

  if (p1_.try_set(session)) {
    const int player_count = static_cast<int>(p1_.exists()) + static_cast<int>(p2_.exists());
    p1_.send(jout::lobby_join(code_, player_count, p2_.character));
    p2_.send(jout::lobby_other_join());

    return LobbyContext{code_, p2_.session(), make_unique<SessionController>(p1_, p2_, *this)};
  }

  if (p2_.try_set(session)) {
    const int player_count = static_cast<int>(p1_.exists()) + static_cast<int>(p2_.exists());
    p2_.send(jout::lobby_join(code_, player_count, p1_.character));
    p1_.send(jout::lobby_other_join());

    return LobbyContext{code_, p1_.session(), make_unique<SessionController>(p2_, p1_, *this)};
  }

  return nullopt;
}

void LobbyController::leave(const weak_ptr<ISession>& session) {
  guard lock(mutex_);
  auto sess = session.lock();

  if (sess == p1_) {
    p1_.reset(true);

    p2_.reset(false);
    p2_.send(jout::lobby_other_leave());
    p2_.send(SessionEvent::kTransitionToCharacterSelect);
    return;
  }

  if (sess == p2_) {
    p2_.reset(true);

    p1_.reset(false);
    p1_.send(jout::lobby_other_leave());
    p1_.send(SessionEvent::kTransitionToCharacterSelect);
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

  new_game();
}

void LobbyController::move_character(Player& self, Player& other, coordinate coordinate) {
  guard lock(mutex_);

  bool traversable = maze_.traversable(self.position, coordinate);

  self.send(traversable ? jout::character_move(coordinate,
                                               maze_.at(coordinate).serialize_for(other.character))
                        : jout::character_reset());
  other.send(jout::character_other_move(*to_dir(self.position, coordinate), !traversable));

  self.position = traversable ? coordinate : kMazeStart;

  if (traversable && maze_.at(coordinate).coin()) {
    maze_.take_coin(coordinate);
    broadcast(make_shared<const string>(jout::coin_taken(coordinate)));
  }

  check_win();
}

void LobbyController::check_win() {
  guard lock(mutex_);

  if (p1_.position != maze_.end() || p2_.position != maze_.end() || maze_.any_coin()) {
    return;
  }
  broadcast(make_shared<const string>(jout::transition_to_gamedone()));
  broadcast(SessionEvent::kTransitionToGameDone);
}

void LobbyController::new_game() {
  guard lock(mutex_);

  broadcast(make_shared<const string>(jout::transition_to_ingame()));
  broadcast(SessionEvent::kTransitionToInGame);

  maze_.randomize();
  p1_.position = kMazeStart;
  p2_.position = kMazeStart;

  p1_.send(jout::ingame_maze(maze_, p1_.character, p2_.character));
  p2_.send(jout::ingame_maze(maze_, p2_.character, p1_.character));
}


void LobbyController::broadcast(std::shared_ptr<const std::string> msg) {
  guard lock(mutex_);

  p1_.send(msg);
  p2_.send(std::move(msg));
}

void LobbyController::broadcast(SessionEvent ev) {
  guard lock(mutex_);

  p1_.send(ev);
  p2_.send(ev);
}

}  // namespace io_blair
