#include "game.hpp"

#include <iostream>
#include <memory>
#include <optional>

#include "lobby.hpp"
#include "lobby_manager.hpp"
#include "net_common.hpp"
#include "response.hpp"
#include "session.hpp"
#include "session_mock.hpp"

using std::string, std::shared_ptr, std::optional;

namespace io_blair {
namespace resp = response;

template <typename Session>
BasicGame<Session>::BasicGame(Session& session)
    : session_(session), state_(State::kPrelobby) {}

template <typename Session>
void BasicGame<Session>::log_err(error_code ec, const char* what) {
    std::cerr << what << ": " << ec.what() << '\n';
}

template <typename Session>
void BasicGame<Session>::parse(string data) {
    // passed data must be non-const to allow library to add SIMDJSON_PADDING
    auto doc = parser_.iterate(data);
    if (doc.error()) return;

    switch (state_) {
        case State::kPrelobby:
            return parse_prelobby(doc.value());
        case State::kCharacterSelect:
        case State::kInGame:
        case State::kGameDone:
            break;
    }
}

template <typename Session>
void BasicGame<Session>::parse_prelobby(json::document& doc) {
    using fields::kPrelobby;

    string lobby_type;
    if (doc[kPrelobby.type].get_string(lobby_type) != 0) return;

    if (lobby_type == kPrelobby.type_create)
        create_lobby();
    else if (lobby_type == kPrelobby.type_join)
        join_lobby(doc);
}

template <typename Session>
void BasicGame<Session>::create_lobby() {
    LobbyManager::get().create()->join(session_.shared_from_this());
    write(resp::create_lobby(session_.lobby()->code_));

    state_ = State::kCharacterSelect;
}

template <typename Session>
void BasicGame<Session>::join_lobby(json::document& doc) {
    using fields::kPrelobby;

    string code;
    if (doc[kPrelobby.join_code].get_string(code) != 0) return;

    optional<shared_ptr<Lobby>> lobby = LobbyManager::get().find(code);
    if (!lobby || lobby.value()->full()) return write(resp::join(false));

    lobby.value()->join(session_.shared_from_this());
    write(resp::join(true));

    state_ = State::kCharacterSelect;
}

template <typename Session>
void BasicGame<Session>::write(const std::shared_ptr<string>& str) {
    session_.write(str);
}

template class BasicGame<Session>;
template class BasicGame<MockSession>;

}  // namespace io_blair
