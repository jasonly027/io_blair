#include "handler.hpp"

#include <memory>
#include <utility>

#include "igame.hpp"
#include "ilobby.hpp"
#include "lobby_context.hpp"


namespace io_blair {
using std::make_unique;
using std::string_view;
using std::unique_ptr;
namespace jin  = json::in;
namespace jout = json::out;

Game::Game(SessionContext ctx, unique_ptr<IGameHandler>&& handler)
    : ctx_(std::move(ctx)), state_(std::move(handler)) {}

void Game::transition_to(unique_ptr<IGameHandler> handler) {
  state_ = std::move(handler);
}

void Game::operator()(const jin::LobbyCreate& ev) {
  (*state_)(*this, ctx_, ev);
}
void Game::operator()(const jin::LobbyJoin& ev) {
  (*state_)(*this, ctx_, ev);
}
void Game::operator()(const jin::LobbyLeave& ev) {
  (*state_)(*this, ctx_, ev);
}
void Game::operator()(const jin::Chat& ev) {
  (*state_)(*this, ctx_, ev);
}
void Game::operator()(const jin::CharacterHover& ev) {
  (*state_)(*this, ctx_, ev);
}
void Game::operator()(const jin::CharacterConfirm& ev) {
  (*state_)(*this, ctx_, ev);
}
void Game::operator()(const jin::CharacterMove& ev) {
  (*state_)(*this, ctx_, ev);
}
void Game::operator()(SessionEvent ev) {
  (*state_)(*this, ctx_, ev);
}

void Prelobby::operator()(IGame& game, SessionContext& ctx, const jin::LobbyCreate&) {
  transition_to_lobby(game, ctx.lobby_manager.create(ctx.session));
}

void Prelobby::operator()(IGame& game, SessionContext& ctx, const jin::LobbyJoin& ev) {
  auto opt = ctx.lobby_manager.join(ctx.session, ev.code);
  if (!opt.has_value()) {
    return;
  }

  transition_to_lobby(game, std::move(*opt));
}

void Prelobby::transition_to_lobby(IGame& game, LobbyContext lob_ctx) {
  game.transition_to(make_unique<Lobby>(std::move(lob_ctx)));
}

Lobby::Lobby(LobbyContext ctx, unique_ptr<ILobbyHandler>&& handler)
    : ctx_(std::move(ctx)), state_(std::move(handler)) {}

void Lobby::transition_to(std::unique_ptr<ILobbyHandler> handler) {
  state_ = std::move(handler);
}

void Lobby::operator()(IGame& game, SessionContext& sess_ctx, const jin::LobbyLeave&) {
  sess_ctx.lobby_manager.leave(sess_ctx.session, ctx_.code);
  game.transition_to(make_unique<Prelobby>());
}

void Lobby::operator()(IGame&, SessionContext&, const jin::Chat& ev) {
  ctx_.other.async_send(jout::chat_msg(ev.msg));
}

void Lobby::operator()(IGame&, SessionContext& sess_ctx, const jin::CharacterHover& ev) {
  (*state_)(*this, sess_ctx, ctx_, ev);
}
void Lobby::operator()(IGame&, SessionContext& sess_ctx, const jin::CharacterConfirm& ev) {
  (*state_)(*this, sess_ctx, ctx_, ev);
}
void Lobby::operator()(IGame&, SessionContext& sess_ctx, const jin::CharacterMove& ev) {
  (*state_)(*this, sess_ctx, ctx_, ev);
}

void Lobby::operator()(IGame& game, SessionContext& sess_ctx, SessionEvent ev) {
  switch (ev) {
    case SessionEvent::kCloseSession: (*this)(game, sess_ctx, jin::LobbyLeave{}); break;
    default:                          (*state_)(*this, sess_ctx, ctx_, ev); break;
  }
}

void CharacterSelect::operator()(ILobby&, SessionContext&, LobbyContext& lob_ctx,
                                 const jin::CharacterHover& ev) {
  lob_ctx.other.async_send(jout::character_hover(ev.character));
}

void CharacterSelect::operator()(ILobby&, SessionContext&, LobbyContext& lob_ctx,
                                 const jin::CharacterConfirm& ev) {
  lob_ctx.controller->set_character(ev.character);
}

void CharacterSelect::operator()(ILobby& lobby, SessionContext&, LobbyContext&, SessionEvent ev) {
  switch (ev) {
    case SessionEvent::kTransitionToInGame: lobby.transition_to(make_unique<InGame>()); break;
    default:                                break;
  }
}

void InGame::operator()(ILobby&, SessionContext&, LobbyContext& lob_ctx,
                        const jin::CharacterMove& ev) {
  int x = ev.coordinate[0];
  int y = ev.coordinate[1];
  lob_ctx.controller->move_character({x, y});
}

}  // namespace io_blair
