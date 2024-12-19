#include "game.hpp"

#include <iostream>
#include <optional>

#include "character.hpp"
#include "lobby.hpp"
#include "net_common.hpp"
#include "request.hpp"
#include "response.hpp"
#include "session.hpp"

namespace io_blair {

using std::string, std::optional;

namespace json = simdjson::ondemand;
namespace resp = response;
namespace req = request;

Game::Game(ISession& session, ILobbyManager& manager)
    : session_(session), manager_(manager), state_(State::kPrelobby) {}

void Game::log_err(error_code ec, const char* what) {
    std::cerr << what << ": " << ec.what() << '\n';
}

auto Game::state() const -> State { return state_; }

void Game::write(string msg) { session_.write(std::move(msg)); }

void Game::msg_other(string msg) {
    lobby_->msg_other(session_, std::move(msg));
}

void Game::parse(string data) {
    // passed data must be non-const to allow library to add SIMDJSON_PADDING
    auto doc = parser_.iterate(data);
    if (doc.error()) return;

    switch (state_) {
        case State::kPrelobby:
            return prelobby(doc.value_unsafe());
        case State::kCharacterSelect:
            return character_select(doc.value_unsafe());
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
    if (doc[req::SharedState.type._].get_string(type) != 0) return;

    if (type == req::Prelobby.type.create) {
        lobby_ = manager_.create(session_);

        if (lobby_) {
            write(resp::join(lobby_->code()));
            state_ = State::kCharacterSelect;
        } else {
            write(resp::join());
        }
    }

    else if (type == req::Prelobby.type.join) {
        string code;
        if (doc[req::Prelobby.code._].get_string(code) != 0) return;

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
    if (doc[req::SharedState.type._].get_string(type) != 0) return;

    if (type == req::SharedState.type.msg) {
        msg(doc);
    }

    else if (type == req::SharedState.type.leave) {
        leave();
    }

    else if (type == req::CharacterSelect.type.hover) {
        json::number hover;
        if (doc[req::CharacterSelect.hover._].get_number().get(hover) != 0)
            return;
        if (!hover.is_int64()) return;

        if (optional<Character> character = to_character(hover.get_int64());
            character.has_value()) {
            if (*character != Character::kUnset) {
                msg_other(resp::hover(*character));
            }
        }
    }

    else if (type == req::CharacterSelect.type.confirm) {
        json::number confirm;
        if (doc[req::CharacterSelect.confirm._].get_number().get(confirm) != 0)
            return;
        if (!confirm.is_int64()) return;

        if (optional<Character> character = to_character(confirm.get_int64());
            character.has_value()) {
            if (lobby_->confirm_character(session_, *character)) {
                state_ = State::kInGame;
            }
        }
    }
}

void Game::msg(document& doc) {
    string msg;
    if (doc[req::SharedState.msg._].get_string(msg) != 0) return;
    msg_other(resp::msg(std::move(msg)));
}

}  // namespace io_blair
