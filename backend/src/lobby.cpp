#include "lobby.hpp"

#include <cstdint>

#include "character.hpp"
#include "game.hpp"
#include "maze.hpp"
#include "request.hpp"
#include "response.hpp"
#include "session.hpp"


namespace io_blair {

using std::shared_ptr, std::string, std::pair, std::optional, std::nullopt;
using lock     = std::lock_guard<std::mutex>;
using document = simdjson::ondemand::document;

namespace resp = response;
namespace req  = request;

void Player::msg(string msg) {
  if (connected()) return session_->write(std::move(msg));
}

void Player::set_session(shared_ptr<ISession> session) {
  session_ = std::move(session);
}

bool Player::connected() const {
  return session_ != nullptr;
}

void Player::reset() {
  character = Character::kUnset;
  session_->set_lobby(nullptr);
  session_->set_state(ISession::State::kPrelobby);
  session_.reset();
}

bool operator==(const Player& player, const ISession& session) {
  return &session == player.session_.get();
}

Lobby::Lobby(net::io_context& ctx, string code, ILobbyManager& manager, Maze (*maze_generator)())
    : manager_(manager),
      strand_(net::make_strand(ctx)),
      code_(std::move(code)),
      state_(State::kCharacterSelect),
      maze_generator_(maze_generator) {}

void Lobby::join(ISession& session) {
  using State = ISession::State;

  session.set_state(State::kWaitingForLobby);
  net::dispatch(strand_, [session = session.get_shared(), me = shared_from_this()] {
    bool ok = false;

    if (!me->p1_.connected()) {
      me->p1_.set_session(session);
      me->p2_.msg(resp::other_join());
      ok = true;
    } else if (!me->p2_.connected()) {
      me->p2_.set_session(session);
      me->p1_.msg(resp::other_join());
      ok = true;
    }

    if (ok) {
      session->set_lobby(me);
      session->set_state(State::kInLobby);
      session->write(resp::join(me->code()));
    } else {
      session->set_lobby(nullptr);
      session->set_state(State::kPrelobby);
      session->write(resp::join());
    }
  });
}

void Lobby::update(ISession& session, string data) {
  net::dispatch(
      strand_,
      [session = session.get_shared(), data = std::move(data), me = shared_from_this()]() mutable {
        // Lambda must be marked mutable so data isn't const so
        // JSON library can apply SIMDJSON_PADDING
        auto doc = me->parser_.iterate(data);
        if (doc.error()) return;

        auto opt_players = me->get_players(*session);
        if (!opt_players.has_value()) return;
        auto& [self, other] = *opt_players;

        switch (me->state()) {
          case State::kCharacterSelect:
            return me->character_select(self, other, doc.value_unsafe());
          case State::kInGame:       return me->in_game(self, other, doc.value_unsafe());
          case State::kGameFinished: return me->game_finished(self, other, doc.value_unsafe());
        }
      });
}

void Lobby::leave(ISession& session) {
  net::dispatch(strand_, [session = session.get_shared(), me = shared_from_this()] {
    auto opt = me->get_players(*session);
    if (!opt.has_value()) return;
    auto& [self, other] = *opt;

    me->leave_impl(self, other);
  });
}

bool Lobby::full() const {
  return p1_.connected() && p2_.connected();
}

bool Lobby::empty() const {
  return !p1_.connected() && !p2_.connected();
}

auto Lobby::state() const -> State {
  return state_;
}

const string& Lobby::code() const {
  return code_;
}

optional<pair<Player&, Player&>> Lobby::get_players(const ISession& session) {
  if (session == p1_) {
    return optional<pair<Player&, Player&>>{
        {p1_, p2_}
    };
  }

  if (session == p2_) {
    return optional<pair<Player&, Player&>>{
        {p2_, p1_}
    };
  }

  return nullopt;
}

optional<pair<const Player&, const Player&>> Lobby::get_players(const ISession& session) const {
  if (session == p1_) {
    return optional<pair<const Player&, const Player&>>{
        {p1_, p2_}
    };
  }

  if (session == p2_) {
    return optional<pair<const Player&, const Player&>>{
        {p2_, p1_}
    };
  }

  return nullopt;
}

namespace {
void msg(Player& player, document& doc) {
  string msg;
  if (doc[req::SharedState.msg._].get_string(msg) != 0) {
    return;
  }

  player.msg(resp::msg(std::move(msg)));
}
}  // namespace

void Lobby::character_select(Player& self, Player& other, document& doc) {
  string type;
  if (doc[req::SharedState.type._].get_string(type) != 0) return;

  if (type == req::SharedState.type.msg) {
    msg(other, doc);
    return;
  }

  if (type == req::SharedState.type.leave) {
    leave_impl(self, other);
    return;
  }

  if (type == req::CharacterSelect.type.hover) {
    int64_t hover;
    if (doc[req::CharacterSelect.hover._].get_int64().get(hover) != 0) return;

    auto opt_char = to_character(hover);
    if (!opt_char.has_value() || opt_char == Character::kUnset) return;

    other.msg(resp::hover(*opt_char));
    return;
  }

  if (type == req::CharacterSelect.type.confirm) {
    int64_t confirm;
    if (doc[req::CharacterSelect.confirm._].get_int64().get(confirm) != 0) return;

    auto opt_char = to_character(confirm);
    if (!opt_char.has_value()) return;

    if (opt_char != other.character || opt_char == Character::kUnset) {
      self.character = *opt_char;
      other.msg(resp::confirm(*opt_char));
    }

    if (self.character != Character::kUnset && other.character != Character::kUnset) {
      generate_maze();
      state_ = State::kInGame;
    }
    return;
  }
}

void Lobby::leave_impl(Player& self, Player& other) {
  state_ = State::kCharacterSelect;

  self.reset();

  other.character = Character::kUnset;
  other.msg(resp::other_leave());

  if (empty()) manager_.remove(code());
}

void Lobby::generate_maze() {
  maze_ = maze_generator_();

  p1_.position = maze_.start();
  p2_.position = maze_.start();

  assert(p1_.character == Character::kIO && p2_.character == Character::kBlair
         || p1_.character == Character::kBlair && p2_.character == Character::kIO);

  if (p1_.character == Character::kIO) {
    p1_.msg(resp::maze(maze_.start(), maze_.end(), maze_.serialize_io()));
    p2_.msg(resp::maze(maze_.start(), maze_.end(), maze_.serialize_blair()));
  } else {
    p1_.msg(resp::maze(maze_.start(), maze_.end(), maze_.serialize_blair()));
    p2_.msg(resp::maze(maze_.start(), maze_.end(), maze_.serialize_io()));
  }
}

void Lobby::in_game(Player& self, Player& other, document& doc) {
  string type;
  if (doc[req::SharedState.type._].get_string(type) != 0) return;

  if (type == req::SharedState.type.msg) {
    msg(other, doc);
    return;
  }

  if (type == req::SharedState.type.leave) {
    leave_impl(self, other);
    return;
  }

  if (type == req::InGame.type.move) {
    int64_t row;
    if (doc[req::InGame.row._].get_int64().get(row) != 0) return;

    int64_t col;
    if (doc[req::InGame.col._].get_int64().get(col) != 0) return;

    Maze::position destination{row, col};

    // if illegal move
    if (!Maze::are_neighbors(self.position, destination)) return;

    // if no path from pos to dest.
    if (!maze_.valid_move(self.position, destination)) {
      self.position = maze_.start();
      self.msg(resp::move_self(self.position));
      other.msg(resp::move_other(self.position));
      return;
    }

    self.position = destination;
    other.msg(resp::move_other(destination));

    if (self.position == maze_.end() && other.position == maze_.end()) {
      self.msg(resp::win());
      other.msg(resp::win());

      state_ = State::kGameFinished;
    }
    return;
  }
}

void Lobby::game_finished(Player& self, Player& other, document& doc) {
  string type;
  if (doc[req::SharedState.type._].get_string(type) != 0) return;

  if (type == req::SharedState.type.msg) {
    msg(other, doc);
    return;
  }

  if (type == req::SharedState.type.leave) {
    leave_impl(self, other);
    return;
  }

  if (type == req::GameFinished.type.restart) {
    other.msg(resp::restart());

    self.character  = Character::kUnset;
    other.character = Character::kUnset;
    state_          = State::kCharacterSelect;
    return;
  }
}

LobbyManager::LobbyManager(net::io_context& ctx)
    : ctx_(ctx) {}

void LobbyManager::create(ISession& session) {
  lock guard(mutex_);

  string code = generate_code();
  while (lobbies_.contains(code)) {
    code = generate_code();
  }

  auto new_lobby = std::make_shared<Lobby>(ctx_, code, *this);
  lobbies_[code] = new_lobby;

  new_lobby->join(session);
}

void LobbyManager::join(const std::string& code, ISession& session) {
  lock guard(mutex_);

  auto it = lobbies_.find(code);
  if (it == lobbies_.end()) return;

  it->second->join(session);
}

void LobbyManager::remove(const string& code) {
  lock guard(mutex_);

  assert(lobbies_.contains(code));
  lobbies_.erase(code);
}

string LobbyManager::generate_code() {
  static constexpr auto kCodeLength = 6;

  static constexpr auto kCharacters = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  static std::mt19937 gen{std::random_device{}()};
  static std::uniform_int_distribution<string::size_type> pick(0, std::strlen(kCharacters) - 1);

  string res(kCodeLength, '\0');
  std::generate_n(res.begin(), kCodeLength, [&] { return kCharacters[pick(gen)]; });

  return res;
}

}  // namespace io_blair
