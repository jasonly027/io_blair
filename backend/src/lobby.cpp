#include "lobby.hpp"

#include <utility>

#include "character.hpp"
#include "game.hpp"
#include "maze.hpp"
#include "response.hpp"
#include "session.hpp"

using std::shared_ptr, std::string, std::pair;
using lock = std::lock_guard<std::mutex>;
using rec_lock = std::lock_guard<std::recursive_mutex>;

namespace io_blair {

namespace resp = response;

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

Lobby::Lobby(string code, ILobbyManager& manager)
    : manager_(manager), code_(std::move(code)) {}

bool Lobby::join(ISession& session) {
    rec_lock guard(mutex_);

    if (!p1_.connected()) {
        p1_.set_session(session.get_shared());
        p2_.msg(resp::other_join());
        return true;
    }

    if (!p2_.connected()) {
        p2_.set_session(session.get_shared());
        p1_.msg(resp::other_join());
        return true;
    }

    return false;
}

void Lobby::msg_all(string msg) {
    rec_lock guard(mutex_);
    p1_.msg(msg);
    p2_.msg(std::move(msg));
}

void Lobby::msg_other(const ISession& session, string msg) {
    rec_lock guard(mutex_);

    auto [self, other] = get_players(session);

    other.msg(std::move(msg));
}

bool Lobby::confirm_character(const ISession& session, Character character) {
    rec_lock guard(mutex_);

    auto [self, other] = get_players(session);

    if (character != other.character || character == Character::kUnset) {
        self.character = character;
        other.msg(resp::confirm(character));
    }

    if (self.character == Character::kUnset ||
        other.character == Character::kUnset) {
        return false;
    }

    generate_maze();
    return true;
}

void Lobby::generate_maze() {
    rec_lock guard(mutex_);

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

void Lobby::leave(const ISession& session) {
    rec_lock guard(mutex_);

    auto [self, other] = get_players(session);

    self.reset();
    other.msg(resp::other_leave());

    if (empty()) manager_.remove(code_);
}

bool Lobby::full() const {
    rec_lock guard(mutex_);
    return p1_.connected() && p2_.connected();
}

bool Lobby::empty() const {
    rec_lock guard(mutex_);
    return !p1_.connected() && !p2_.connected();
}

const string& Lobby::code() const { return code_; }

pair<Player&, Player&> Lobby::get_players(const ISession& session) {
    rec_lock guard(mutex_);
    if (session == p1_) {
        return {p1_, p2_};
    }
    return {p2_, p1_};
}

pair<const Player&, const Player&> Lobby::get_players(
    const ISession& session) const {
    rec_lock guard(mutex_);
    if (session == p1_) {
        return {p1_, p2_};
    }
    return {p2_, p1_};
}

shared_ptr<ILobby> LobbyManager::create(ISession& session) {
    lock guard(mutex_);

    string code = generate_code();
    while (lobbies_.contains(code)) {
        code = generate_code();
    }

    auto lobby = std::make_shared<Lobby>(code, *this);
    lobbies_[code] = lobby;

    return lobby->join(session) ? lobby : nullptr;
}

shared_ptr<ILobby> LobbyManager::join(const std::string& code,
                                      ISession& session) {
    lock guard(mutex_);

    auto it = lobbies_.find(code);
    if (it == lobbies_.end()) return nullptr;

    return it->second->join(session) ? it->second : nullptr;
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
