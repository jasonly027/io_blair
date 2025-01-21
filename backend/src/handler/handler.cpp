#include "handler.hpp"

#include <memory>
#include <utility>

#include "json.hpp"
#include "lobby_context.hpp"


namespace io_blair {
using std::nullopt;
using std::string_view;

namespace jout = json::out;

Game::Game(SessionContext ctx)
    : ctx_(std::move(ctx)), state_(std::make_unique<Prelobby>()) {}

void Game::transition_to(std::unique_ptr<IGameHandler> handler) {
  state_ = std::move(handler);
}

void Game::operator()(const json::in::LobbyCreate& ev) {
  (*state_)(*this, ctx_, ev);
}
void Game::operator()(const json::in::LobbyJoin& ev) {
  (*state_)(*this, ctx_, ev);
}

void Prelobby::operator()(IGame& game, SessionContext& ctx, const json::in::LobbyCreate&) {
  transition_to_lobby(game, ctx, ctx.lobby_manager.create(ctx.session));
}

void Prelobby::operator()(IGame& game, SessionContext& ctx, const json::in::LobbyJoin& ev) {
  auto opt = ctx.lobby_manager.join(ctx.session, ev.code);
  if (!opt.has_value()) {
    ctx.session.lock()->async_send(jout::lobby_join(nullopt));
    return;
  }

  transition_to_lobby(game, ctx, std::move(*opt));
}

void Prelobby::transition_to_lobby(IGame& game, SessionContext& ctx, LobbyContext lob_ctx) {
  auto code = lob_ctx.code;
  game.transition_to(std::make_unique<Lobby>(std::move(lob_ctx)));
  ctx.session.lock()->async_send(jout::lobby_join(code));
}

Lobby::Lobby(LobbyContext ctx)
    : ctx_(std::move(ctx)) {}

void Lobby::transition_to(std::unique_ptr<ILobbyHandler> handler) {
  state_ = std::move(handler);
}


void Lobby::operator()(IGame& game, SessionContext& ctx, const json::in::LobbyLeave&) {

}

}  // namespace io_blair
