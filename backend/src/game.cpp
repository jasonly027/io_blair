#include "game.hpp"

#include <iostream>

#include "lobby_manager.hpp"
#include "net_common.hpp"
#include "response.hpp"
#include "session.hpp"

using std::string;

namespace io_blair {
namespace resp = response;
Game::Game(Session& session) : session_(session), state_(State::kPrelobby) {}

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

void Game::log_err(error_code ec, const char* what) {
    std::cerr << what << ": " << ec.what() << '\n';
}

void Game::parse_prelobby(json::document& doc) {
    using fields::kPrelobby;

    string lobby_type;
    if (auto ec = doc[kPrelobby.type].get_string(lobby_type); ec) {
        return;
    }

    if (lobby_type == kPrelobby.type_create) {
        session_.lobby() =
            session_.manager().create(session_.shared_from_this());
        session_.write(resp::create_lobby(session_.lobby()->code_));
        state_ = State::kCharacterSelect;
    } else if (lobby_type == kPrelobby.type_join) {
        // todo
    }
}

}  // namespace io_blair
