#include "game.hpp"

#include <iostream>

#include "net_common.hpp"
#include "response.hpp"
#include "session.hpp"

using std::string;

namespace io_blair {
namespace resp = response;

Game::Game(ISession& session) : session_(session), state_(State::kPrelobby) {}

void Game::log_err(error_code ec, const char* what) {
    std::cerr << what << ": " << ec.what() << '\n';
}

auto Game::state() const -> State { return state_; }

void Game::write(string msg) { session_.write(std::move(msg)); }

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

void Game::parse_prelobby(document& doc) {
    using request::kPrelobby;

    string lobby_type;
    if (doc[kPrelobby.type].get_string(lobby_type) != 0) return;

    if (lobby_type == kPrelobby.type_create)
        create_lobby();
    else if (lobby_type == kPrelobby.type_join)
        join_lobby(doc);
}

void Game::create_lobby() {
    if (session_.join_new_lobby()) {
        write(resp::join(true, string(session_.code())));
        state_ = State::kCharacterSelect;
    } else {
        write(resp::join(false));
    }
}

void Game::join_lobby(document& doc) {
    using request::kPrelobby;

    string code;
    if (doc[kPrelobby.join_code].get_string(code) != 0) return;

    if (session_.join_lobby(code)) {
        write(resp::join(true, std::move(code)));
        state_ = State::kCharacterSelect;
    } else {
        write(resp::join(false));
    }
}

}  // namespace io_blair
