#include "lobby.hpp"

#include <iostream>

#include "lobby_manager.hpp"
#include "session.hpp"

using std::string;

namespace io_blair {
Lobby::Lobby(std::string code, std::shared_ptr<Session> p1,
             std::shared_ptr<Session> p2)
    : code_(std::move(code)), p1_(std::move(p1)), p2_(std::move(p2)) {}

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
}  // namespace io_blair
