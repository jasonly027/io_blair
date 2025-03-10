/**
 * @file handler.hpp
 */
#pragma once

#include <memory>

#include "event.hpp"
#include "igame.hpp"
#include "ihandler.hpp"
#include "ilobby.hpp"
#include "json.hpp"
#include "lobby_context.hpp"
#include "session_context.hpp"


namespace io_blair {
/**
 * @brief A state of IGame before the client has joined a lobby.
 * Expecting events to create/join a lobby.
 */
class Prelobby : public IGameHandler {
 public:
  void operator()(IGame&, SessionContext&, const json::in::LobbyCreate&) override;
  void operator()(IGame&, SessionContext&, const json::in::LobbyJoin&) override;

 private:
  // Transitions the IGame to the Lobby state.
  static void transition_to_lobby(IGame&, LobbyContext);
};

/**
 * @brief A state context that forwards events to the current state. Possible states
 * are Prelobby and Lobby.
 */
class Game : public IGame, public IHandler {
 public:
  /**
   * @brief Construct a new Game object.
   * 
   * @param ctx Context from the Session.
   */
  explicit Game(SessionContext ctx,
                std::unique_ptr<IGameHandler>&& handler = std::make_unique<Prelobby>());

  void transition_to(std::unique_ptr<IGameHandler> handler) override;

  void operator()(const json::in::LobbyCreate&) override;
  void operator()(const json::in::LobbyJoin&) override;
  void operator()(const json::in::LobbyLeave&) override;
  void operator()(const json::in::Chat&) override;
  void operator()(const json::in::CharacterHover&) override;
  void operator()(const json::in::CharacterConfirm&) override;
  void operator()(SessionEvent) override;

 private:
  // Session::make() creates a Session that uses Game. That Game
  // has the same lifetime as the session so that Game's ctx_.session
  // will always be a valid pointer to the session.
  SessionContext ctx_;

  // The current state to forward events to.
  std::unique_ptr<IGameHandler> state_;
};

/**
 * @brief A state of ILobby where the client is selecting their character.
 * Expecting client messages of character hovering/confirming, chat messages,
 * and leaving.
 */
class CharacterSelect : public ILobbyHandler {
 public:
  void operator()(ILobby&, SessionContext&, LobbyContext&,
                  const json::in::CharacterHover&) override;
  void operator()(ILobby&, SessionContext&, LobbyContext&,
                  const json::in::CharacterConfirm&) override;
  void operator()(ILobby&, SessionContext&, LobbyContext&, SessionEvent) override;

 private:
};

/**
 * @brief A state of IGame where the client is in a lobby. Lobby itself
 * is a state context that forwards events to the current state.
 * Possible substates are CharacterSelect, InGame, GameFinished.
 */
class Lobby : public ILobby, public IGameHandler {
 public:
  /**
   * @brief Construct a new Lobby object.
   * 
   * @param ctx Context from the lobby.
   */
  explicit Lobby(LobbyContext ctx,
                 std::unique_ptr<ILobbyHandler>&& handler = std::make_unique<CharacterSelect>());

  void transition_to(std::unique_ptr<ILobbyHandler>) override;

  void operator()(IGame&, SessionContext&, const json::in::LobbyLeave&) override;
  void operator()(IGame&, SessionContext&, const json::in::Chat&) override;
  void operator()(IGame&, SessionContext&, const json::in::CharacterHover&) override;
  void operator()(IGame&, SessionContext&, const json::in::CharacterConfirm&) override;
  void operator()(IGame&, SessionContext&, SessionEvent) override;

 private:
  LobbyContext ctx_;

  // The current state to forward events to.
  std::unique_ptr<ILobbyHandler> state_;
};


/**
 * @brief A state of ILobby where the client is trying to finish the game.
 */
class InGame : public ILobbyHandler {
 public:
 private:
};

/**
 * @brief A state of ILobby where the client has completed the game.
 */
class GameFinished : public ILobbyHandler {
 public:
 private:
};

}  // namespace io_blair
