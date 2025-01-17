#include "handler.hpp"

#include <optional>
#include <string_view>

#include "context.hpp"
#include "json.hpp"
#include "lobby.hpp"

namespace io_blair {

using std::nullopt;
using std::string_view;

namespace jout = json::out;

void IHandler::operator()(const json::in::LobbyCreate&) {}
void IHandler::operator()(const json::in::LobbyJoin&) {}

Prelobby::Prelobby(Game& game, SessionContext& ctx)
    : game_(game), ctx_(ctx) {}

void Prelobby::operator()(const json::in::LobbyCreate&) {
  game_.lobby().set_lobby_ctx(ctx_.lobby_manager.create(ctx_.session));
  game_.set_state(game_.lobby());

  ctx_.send(jout::lobby_join(game_.lobby().code()));
}

void Prelobby::operator()(const json::in::LobbyJoin& ev) {
  if (auto lob_ctx = ctx_.lobby_manager.join(ctx_.session, ev.code); lob_ctx.has_value()) {
    game_.lobby().set_lobby_ctx(*lob_ctx);
    game_.set_state(game_.lobby());

    ctx_.send(jout::lobby_join(game_.lobby().code()));
    return;
  }
  ctx_.send(jout::lobby_join(nullopt));
}

Lobby::Lobby(Game& game, SessionContext& session_ctx)
    : game_(game), session_ctx_(session_ctx), lobby_ctx_(nullptr), state_(&character_select_) {}

void Lobby::set_lobby_ctx(LobbyContext& lobby_ctx) {
  lobby_ctx_ = &lobby_ctx;
}

string_view Lobby::code() const {
  return lobby_ctx_->code;
}


Game::Game(SessionContext ctx)
    : prelobby_(*this, ctx), lobby_(*this, ctx), state_(&prelobby_) {}

Prelobby& Game::prelobby() {
  return prelobby_;
}

Lobby& Game::lobby() {
  return lobby_;
}

void Game::set_state(GameState& state) {
  state_ = &state;
}

void Game::operator()(const json::in::LobbyCreate& ev) {
  (*state_)(ev);
}
void Game::operator()(const json::in::LobbyJoin& ev) {
  (*state_)(ev);
}

}  // namespace io_blair
