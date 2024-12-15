#include "game.hpp"

#include <iostream>
#include <optional>

#include "character.hpp"
#include "lobby.hpp"
#include "net_common.hpp"
#include "response.hpp"
#include "session.hpp"

using std::string, std::optional;
namespace json = simdjson::ondemand;

namespace io_blair {

namespace resp = response;
using Character::kUnset, Character::kIO, Character::kBlair;
using request::SharedState, request::Prelobby, request::CharacterSelect;

Game::Game(ISession& session, ILobbyManager& manager)
    : session_(session), manager_(manager), state_(State::kPrelobby) {}

void Game::log_err(error_code ec, const char* what) {
    std::cerr << what << ": " << ec.what() << '\n';
}

auto Game::state() const -> State { return state_; }

void Game::write(string msg) { session_.write(std::move(msg)); }

void Game::write_other(string msg) {
    lobby_->msg_other(session_, std::move(msg));
}

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

void Game::leave() {
    if (lobby_) {
        lobby_->leave(session_);
        lobby_.reset();
    }

    state_ = State::kPrelobby;
}

void Game::prelobby(document& doc) {
    string type;
    if (doc[SharedState.type._].get_string(type) != 0) return;

    if (type == Prelobby.type.create) {
        lobby_ = manager_.create(session_);

        if (lobby_) {
            write(resp::join(lobby_->code()));
            state_ = State::kCharacterSelect;
        } else {
            write(resp::join());
        }
    }

    else if (type == Prelobby.type.join) {
        string code;
        if (doc[Prelobby.code._].get_string(code) != 0) return;

        lobby_ = manager_.join(code, session_);

        if (lobby_) {
            write(resp::join(std::move(code)));
            state_ = State::kCharacterSelect;
        } else {
            write(resp::join());
        }
    }
}

void Game::character_select(document& doc) {
    string type;
    if (doc[SharedState.type._].get_string(type) != 0) return;

    if (type == SharedState.type.msg) {
        msg(doc);
    }

    else if (type == SharedState.type.leave) {
        leave();
    }

    else if (type == CharacterSelect.type.hover) {
        json::number hover;
        if (doc[CharacterSelect.hover._].get_number().get(hover) != 0) return;
        if (!hover.is_int64()) return;

        switch (hover.get_int64()) {
            case CharacterSelect.hover.io:
                write_other(resp::hover(kIO));
                break;
            case CharacterSelect.hover.blair:
                write_other(resp::hover(kBlair));
                break;
        }
    }

    else if (type == CharacterSelect.type.confirm) {
        json::number confirm;
        if (doc[CharacterSelect.confirm._].get_number().get(confirm) != 0)
            return;
        if (!confirm.is_int64()) return;

        if (optional<Character> character = to_character(confirm.get_int64());
            character.has_value()) {
            lobby_->confirm_character(session_, *character);
        }
    }
}

void Game::msg(document& doc) {
    string msg;
    if (doc[SharedState.msg._].get_string(msg) != 0) return;
    write_other(resp::msg(std::move(msg)));
}

}  // namespace io_blair
