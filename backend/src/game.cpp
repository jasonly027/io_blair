#include "game.hpp"

#include <iostream>

#include "net_common.hpp"
#include "response.hpp"
#include "session.hpp"

using std::string;

namespace io_blair {

namespace resp = response;
using request::SharedState, request::Prelobby, request::CharacterSelect;

Game::Game(ISession& session) : session_(session), state_(State::kPrelobby) {}

void Game::log_err(error_code ec, const char* what) {
    std::cerr << what << ": " << ec.what() << '\n';
}

auto Game::state() const -> State { return state_; }

void Game::write(string msg) { session_.write(std::move(msg)); }

void Game::write_other(string msg) { session_.write_other(std::move(msg)); }

void Game::parse(string data) {
    // passed data must be non-const to allow library to add SIMDJSON_PADDING
    auto doc = parser_.iterate(data);
    if (doc.error()) return;

    switch (state_) {
        case State::kPrelobby:
            return prelobby(doc.value());
        case State::kCharacterSelect:
            return character_select(doc.value());
        case State::kInGame:
        case State::kGameDone:
            break;
    }
}

void Game::prelobby(document& doc) {
    string type;
    if (doc[SharedState.type._].get_string(type) != 0) return;

    if (type == Prelobby.type.create) {
        if (session_.join_new_lobby()) {
            write(resp::join(true, string(session_.code())));
            state_ = State::kCharacterSelect;
        } else {
            write(resp::join(false));
        }
    }

    else if (type == Prelobby.type.join) {
        string code;
        if (doc[Prelobby.code._].get_string(code) != 0) return;

        if (session_.join_lobby(code)) {
            write(resp::join(true, std::move(code)));
            state_ = State::kCharacterSelect;
        } else {
            write(resp::join(false));
        }
    }
}

void Game::character_select(document& doc) {
    string type;
    if (doc[SharedState.type._].get_string(type) != 0) return;

    if (type == SharedState.type.chat) {
        msg(doc);
    }

    else if (type == SharedState.type.leave) {
        leave();
    }

    else if (type == CharacterSelect.type.hover) {
        string hover;
        if (doc[CharacterSelect.hover._].get_string(hover) != 0) return;

        // todo
    }

    else if (type == CharacterSelect.type.confirm) {
        string confirm;
        if (doc[CharacterSelect.confirm._].get_string(confirm) != 0) return;

        // todo
    }
}

void Game::msg(document& doc) {
    string msg;
    if (doc[SharedState.msg._].get_string(msg) != 0) return;
    session_.write_other(resp::chat(std::move(msg)));
}

void Game::leave() {
    session_.leave_lobby();
    state_ = State::kPrelobby;
}

}  // namespace io_blair
