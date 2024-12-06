#include "game.hpp"

#include <iostream>

#include "net_common.hpp"
#include "response.hpp"
#include "session.hpp"

using std::string;

namespace io_blair {
// namespace resp = response;

Game::Game(ISession& session) : session_(session), state_(State::kPrelobby) {}

void Game::log_err(error_code ec, const char* what) {
    std::cerr << what << ": " << ec.what() << '\n';
}

void Game::parse(string data) {
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

void Game::parse_prelobby(json::document& doc) {
    using fields::kPrelobby;

    string lobby_type;
    if (doc[kPrelobby.type].get_string(lobby_type) != 0) return;

    if (lobby_type == kPrelobby.type_create)
        create_lobby();
    else if (lobby_type == kPrelobby.type_join)
        join_lobby(doc);
}

void Game::create_lobby() {
    session_.join_new_lobby();
    state_ = State::kCharacterSelect;
}

void Game::join_lobby(json::document& doc) {
    using fields::kPrelobby;

    string code;
    if (doc[kPrelobby.join_code].get_string(code) != 0) return;

    if (session_.join_lobby(code)) {
        state_ = State::kCharacterSelect;
    }
}

void Game::write(string msg) { session_.write(std::move(msg)); }

}  // namespace io_blair
