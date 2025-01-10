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
        manager_.create(session_);
        return;
      }

      if (type == req::Prelobby.type.join) {
        string code;
        if (doc[req::Prelobby.code._].get_string(code) != 0) return;

        manager_.join(code, session_);
        return;
      }

      return;
    }
    case State::kWaitingForLobby: return;
    case State::kInLobby:         return session_.try_lobby_update(std::move(data));
  }
}

}  // namespace io_blair
