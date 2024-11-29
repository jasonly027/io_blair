#include "lobby.hpp"

#include <memory>

#include "lobby_manager.hpp"
#include "session.hpp"
#include "session_mock.hpp"

using std::string, std::shared_ptr;

namespace io_blair {

template <typename Session>
BasicLobby<Session>::BasicLobby(string code, shared_ptr<Session> p1,
                                shared_ptr<Session> p2)
    : code_(std::move(code)), p1_(std::move(p1)), p2_(std::move(p2)) {}

template <typename Session>
bool BasicLobby<Session>::join(shared_ptr<Session> ptr) {
    if (!p1_) {
        p1_ = std::move(ptr);
        p1_->lobby() = this->shared_from_this();
        return true;
    }

    if (!p2_) {
        p2_ = std::move(ptr);
        p2_->lobby() = this->shared_from_this();
        return true;
    }

    return false;
}

template <typename Session>
void BasicLobby<Session>::leave(Session* const session) {
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

template <typename Session>
bool BasicLobby<Session>::full() const {
    return p1_ && p2_;
}

template class BasicLobby<Session>;
template class BasicLobby<MockSession>;
}  // namespace io_blair
