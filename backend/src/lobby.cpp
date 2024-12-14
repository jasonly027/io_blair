#include "lobby.hpp"

#include <utility>

#include "game.hpp"
#include "session.hpp"

using std::shared_ptr, std::string;
using lock = std::lock_guard<std::mutex>;

namespace io_blair {

Lobby::Lobby(string code, ILobbyManager& manager)
    : code_(std::move(code)), manager_(manager) {}

bool Lobby::join(shared_ptr<ISession> ptr) {
    lock guard(mutex_);

    if (!p1_) {
        p1_ = std::move(ptr);
        return true;
    }

    if (!p2_) {
        p2_ = std::move(ptr);
        return true;
    }

    return false;
}

void Lobby::msg(const ISession& session, string msg) {
    lock guard(mutex_);

    if (&session == p1_.get() && p2_) {
        p2_->write(std::move(msg));
    } else if (&session == p2_.get() && p1_) {
        p1_->write(std::move(msg));
    }
}

bool Lobby::confirm_character(const ISession& session, Character character) {
    lock guard(mutex_);

    return false;
}

void Lobby::leave(const ISession& session) {
    lock guard(mutex_);

    if (&session == p1_.get()) {
        p1_.reset();
    } else if (&session == p2_.get()) {
        p2_.reset();
    }

    if (!p1_ && !p2_) manager_.remove(code_);
}

bool Lobby::full() const {
    lock guard(mutex_);
    return p1_ && p2_;
}

const string& Lobby::code() const { return code_; }

shared_ptr<ILobby> LobbyManager::create(shared_ptr<ISession> session) {
    lock guard(mutex_);

    string code = generate_code();
    while (lobbies_.contains(code)) {
        code = generate_code();
    }

    auto lobby = std::make_shared<Lobby>(code, *this);
    lobbies_[code] = lobby;

    return lobby->join(std::move(session)) ? lobby : nullptr;
}

shared_ptr<ILobby> LobbyManager::join(const std::string& code,
                                      std::shared_ptr<ISession> session) {
    lock guard(mutex_);

    auto it = lobbies_.find(code);
    if (it == lobbies_.end()) return nullptr;

    return it->second->join(std::move(session)) ? it->second : nullptr;
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
