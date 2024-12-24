#include "lobby.hpp"

#include "character.hpp"
#include "game.hpp"
#include "request.hpp"
#include "response.hpp"
#include "session.hpp"

namespace io_blair {

using std::shared_ptr, std::string, std::pair, std::optional, std::nullopt;
using lock = std::lock_guard<std::mutex>;
using document = simdjson::ondemand::document;

namespace resp = response;
namespace req = request;

void Player::msg(string msg) {
    if (connected()) return session_->write(std::move(msg));
}

void Player::set_session(shared_ptr<ISession> session) {
    session_ = std::move(session);
}

void Player::set_position(Maze::position position) {
    position_ = std::move(position);
}

bool Player::connected() const { return session_ != nullptr; }

void Player::reset() {
    character = Character::kUnset;
    session_.reset();
}

bool operator==(const Player& player, const ISession& session) {
    return &session == player.session_.get();
}

Lobby::Lobby(net::io_context& ctx, string code, ILobbyManager& manager)
    : manager_(manager),
      strand_(net::make_strand(ctx)),
      code_(std::move(code)),
      state_(State::kCharacterSelect) {}

void Lobby::join(ISession& session) {
    using State = ISession::State;

    session.set_state(State::kWaitingForLobby);
    net::dispatch(strand_,
                  [session = session.get_shared(), me = shared_from_this()] {
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
        strand_, [session = session.get_shared(), data = std::move(data),
                  me = shared_from_this()]() mutable {
            // Lambda must be marked mutable so data isn't const so
            // JSON library can apply SIMDJSON_PADDING
            auto doc = me->parser_.iterate(data);
            if (doc.error()) return;

            switch (me->state()) {
                case State::kCharacterSelect:
                    return me->character_select(*session, doc.value_unsafe());
                case State::kInGame:
                    return me->in_game(*session, doc.value_unsafe());
                case State::kGameFinished:
                    break;
            }
        });
}

void Lobby::leave(ISession& session) {
    net::dispatch(strand_,
                  [session = session.get_shared(), me = shared_from_this()] {
                      auto opt = me->get_players(*session);
                      if (!opt.has_value()) return;
                      auto& [self, other] = *opt;

                      me->leave_impl(*session, self, other);
                  });
}

bool Lobby::full() const { return p1_.connected() && p2_.connected(); }

bool Lobby::empty() const { return !p1_.connected() && !p2_.connected(); }

auto Lobby::state() const -> State { return state_; }

const string& Lobby::code() const { return code_; }

optional<pair<Player&, Player&>> Lobby::get_players(const ISession& session) {
    if (session == p1_) {
        return optional<pair<Player&, Player&>>{{p1_, p2_}};
    }

    if (session == p2_) {
        return optional<pair<Player&, Player&>>{{p2_, p1_}};
    }

    return nullopt;
}

optional<pair<const Player&, const Player&>> Lobby::get_players(
    const ISession& session) const {
    if (session == p1_) {
        return optional<pair<const Player&, const Player&>>{{p1_, p2_}};
    }

    if (session == p2_) {
        return optional<pair<const Player&, const Player&>>{{p2_, p1_}};
    }

    return nullopt;
}

static void msg(Player& player, document& doc) {
    string msg;
    if (doc[req::SharedState.msg._].get_string(msg) != 0) return;
    player.msg(resp::msg(std::move(msg)));
}

void Lobby::character_select(ISession& session, document& doc) {
    auto opt = get_players(session);
    if (!opt.has_value()) return;
    auto& [self, other] = *opt;

    string type;
    if (doc[req::SharedState.type._].get_string(type) != 0) return;

    if (type == req::SharedState.type.msg) {
        return msg(other, doc);
    }

    if (type == req::SharedState.type.leave) {
        return leave_impl(session, self, other);
    }

    if (type == req::CharacterSelect.type.hover) {
        int64_t hover;
        if (doc[req::CharacterSelect.hover._].get_int64().get(hover) != 0)
            return;

        if (optional<Character> character = to_character(hover);
            character.has_value()) {
            if (*character != Character::kUnset) {
                other.msg(resp::hover(*character));
            }
        }
        return;
    }

    if (type == req::CharacterSelect.type.confirm) {
        int64_t confirm;
        if (doc[req::CharacterSelect.confirm._].get_int64().get(confirm) != 0)
            return;

        if (optional<Character> character = to_character(confirm);
            character.has_value()) {
            if (character != other.character ||
                character == Character::kUnset) {
                self.character = *character;
                other.msg(resp::confirm(*character));
            }

            if (self.character != Character::kUnset &&
                other.character != Character::kUnset) {
                generate_maze();
                state_ = State::kInGame;
            }
        }
        return;
    }
}

void Lobby::leave_impl(ISession& session, Player& self, Player& other) {
    state_ = State::kCharacterSelect;

    self.reset();

    session.set_lobby(nullptr);
    session.set_state(ISession::State::kPrelobby);

    other.character = Character::kUnset;
    other.msg(resp::other_leave());

    if (empty()) manager_.remove(code());
}

void Lobby::generate_maze() {
    maze_ = Maze::generate_maze();
    p1_.set_position(maze_.start());
    p2_.set_position(maze_.start());

    assert(
        p1_.character == Character::kIO && p2_.character == Character::kBlair ||
        p1_.character == Character::kBlair && p2_.character == Character::kIO);

    if (p1_.character == Character::kIO) {
        p1_.msg(resp::maze(maze_.start(), maze_.end(), maze_.serialize_io()));
        p2_.msg(
            resp::maze(maze_.start(), maze_.end(), maze_.serialize_blair()));
    } else {
        p1_.msg(
            resp::maze(maze_.start(), maze_.end(), maze_.serialize_blair()));
        p2_.msg(resp::maze(maze_.start(), maze_.end(), maze_.serialize_io()));
    }
}

void Lobby::in_game(ISession& session, document& doc) {
    auto opt = get_players(session);
    if (!opt.has_value()) return;
    auto& [self, other] = *opt;

    string type;
    if (doc[req::SharedState.type._].get_string(type) != 0) return;

    if (type == req::SharedState.type.msg) {
        return msg(other, doc);
    }

    if (type == req::SharedState.type.leave) {
        return leave_impl(session, self, other);
    }

}

LobbyManager::LobbyManager(net::io_context& ctx) : ctx_(ctx) {}

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
    static std::uniform_int_distribution<std::string::size_type> pick(
        0, std::strlen(kCharacters) - 1);

    string res(kCodeLength, '\0');
    std::generate_n(res.begin(), kCodeLength,
                    [&] { return kCharacters[pick(gen)]; });

    return res;
}

}  // namespace io_blair
