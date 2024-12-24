#include "game.hpp"

#include "lobby.hpp"
#include "request.hpp"
#include "session.hpp"

namespace io_blair {

using std::string;
using State = ISession::State;

namespace req = request;

Game::Game(ISession& session, ILobbyManager& manager)
    : session_(session), manager_(manager) {}

void Game::update(string data) {
    switch (session_.state()) {
        case State::kPrelobby: {
            auto doc = parser_.iterate(data);
            if (doc.error()) return;

            string type;
            if (doc[req::SharedState.type._].get_string(type) != 0) return;

            if (type == req::Prelobby.type.create) {
                return manager_.create(session_);
            }

            if (type == req::Prelobby.type.join) {
                string code;
                if (doc[req::Prelobby.code._].get_string(code) != 0) return;

                return manager_.join(code, session_);
            }

            return;
        }
        case State::kWaitingForLobby:
            return;
        case State::kInLobby:
            return session_.try_lobby_update(std::move(data));
    }
}

// void Game::in_game(document& doc) {
//     string type;
//     if (doc[req::SharedState.type._].get_string(type) != 0) return;

//     if (type == req::SharedState.type.msg) {
//         msg(doc);
//     }

//     else if (type == req::SharedState.type.leave) {
//         leave();
//     }

//     else if (type == req::InGame.type.move) {
//         int64_t num;
//         if (doc[req::InGame.move._].get_int64().get(num) != 0) return;

//         if (optional<Move> move = to_move(num); move.has_value()) {

//         }
//     }
// }

}  // namespace io_blair
