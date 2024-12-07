#include "lobby.hpp"

#include "session.hpp"

using std::shared_ptr, std::string, std::optional;
using lock = std::lock_guard<std::mutex>;

namespace io_blair {

Lobby::Lobby(string code, LobbyManager& manager)
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

void Lobby::leave(const ISession* const session) {
    lock guard(mutex_);

    assert((session == p1_.get() || session == p2_.get()) &&
           "Session is neither in lobby");

    if (session == p1_.get()) {
        p1_.reset();
    } else if (session == p2_.get()) {
        p2_.reset();
    }

    if (!p1_ && !p2_) manager_.remove(code_);
}

bool Lobby::full() const {
    lock guard(mutex_);
    return p1_ && p2_;
}

shared_ptr<Lobby> LobbyManager::create() {
    lock guard(mutex_);

    string code = generate_code();
    while (lobbies_.contains(code)) {
        code = generate_code();
    }

    auto lobby = std::make_shared<Lobby>(code, *this);
    lobbies_[code] = lobby;

    return lobby;
}

optional<shared_ptr<Lobby>> LobbyManager::find(const std::string& code) {
    lock guard(mutex_);

    auto it = lobbies_.find(code);
    if (it == lobbies_.end()) return std::nullopt;

    return optional{it->second};
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
