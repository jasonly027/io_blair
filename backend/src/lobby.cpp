#include "lobby.hpp"

#include "lobby_manager.hpp"
#include "session.hpp"

using std::string, std::shared_ptr;

namespace io_blair {

Lobby::Lobby(string code, LobbyManager& manager)
    : code_(std::move(code)), manager_(manager) {}

bool Lobby::join(shared_ptr<ISession> ptr) {
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
    assert((session == p1_.get() || session == p2_.get()) &&
           "Session is neither in lobby");

    if (session == p1_.get()) {
        p1_.reset();
    } else if (session == p2_.get()) {
        p2_.reset();
    }

    if (!p1_ && !p2_) manager_.remove(code_);
}

bool Lobby::full() const { return p1_ && p2_; }

}  // namespace io_blair
