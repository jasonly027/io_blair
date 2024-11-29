#include "lobby.hpp"

#include <memory>

#include "lobby_manager.hpp"
#include "session.hpp"

using std::string, std::shared_ptr;

namespace io_blair {
Lobby::Lobby(string code, shared_ptr<Session> p1, shared_ptr<Session> p2)
    : code_(std::move(code)), p1_(std::move(p1)), p2_(std::move(p2)) {}

bool Lobby::join(shared_ptr<Session> ptr) {
    if (!p1_) {
        p1_ = std::move(ptr);
        p1_->lobby() = shared_from_this();
        return true;
    }

    if (!p2_) {
        p2_ = std::move(ptr);
        p2_->lobby() = shared_from_this();
        return true;
    }

    return false;
}

void Lobby::leave(Session* const session) {
    assert((session == p1_.get() || session == p2_.get()) &&
           "Session is neither in lobby");

    session->lobby().reset();

    if (session == p1_.get()) {
        p1_.reset();
    } else if (session == p2_.get()) {
        p2_.reset();
    }

    if (!p1_ && !p2_) LobbyManager::get().remove(code_);
}

bool Lobby::full() const { return p1_ && p2_; }
}  // namespace io_blair
