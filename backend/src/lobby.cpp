#include "lobby.hpp"

#include <utility>

#include "game.hpp"
#include "response.hpp"
#include "session.hpp"

using std::shared_ptr, std::string;
using lock = std::lock_guard<std::mutex>;
using rec_lock = std::lock_guard<std::recursive_mutex>;

namespace io_blair {

namespace resp = response;

Lobby::Lobby(string code, ILobbyManager& manager)
    : code_(std::move(code)), manager_(manager) {}

bool Lobby::join(ISession& session) {
    rec_lock guard(mutex_);

    if (!p1_) {
        p1_ = session.get_shared();
        return true;
    }

    if (!p2_) {
        p2_ = session.get_shared();
        return true;
    }

    return false;
}

void Lobby::msg_other(const ISession& session, string msg) {
    rec_lock guard(mutex_);

    if (&session == p1_.get() && p2_) {
        p2_->write(std::move(msg));
    } else if (&session == p2_.get() && p1_) {
        p1_->write(std::move(msg));
    }
}

void Lobby::confirm_character(const ISession& session, Character character) {
    rec_lock guard(mutex_);

    switch (character) {
        case Character::kUnset:
            break;
        case Character::kIO:
            break;
        case Character::kBlair:
            break;
    }
}

void Lobby::leave(const ISession& session) {
    rec_lock guard(mutex_);

    if (&session == p1_.get()) {
        p1_.reset();
    } else if (&session == p2_.get()) {
        p2_.reset();
    }

    if (!p1_ && !p2_) manager_.remove(code_);
}

bool Lobby::full() const {
    rec_lock guard(mutex_);
    return p1_ && p2_;
}

const string& Lobby::code() const { return code_; }

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
