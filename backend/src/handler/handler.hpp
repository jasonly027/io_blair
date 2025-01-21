/**
 * @file handler.hpp
 */
#pragma once

#include <memory>

#include "igame.hpp"
#include "ilobby.hpp"
#include "json.hpp"
#include "lobby_context.hpp"
#include "session_context.hpp"


namespace io_blair {
/**
 * @brief A state context that forwards events to the current state. Possible states
 * are Prelobby and Lobby.
 */
class Game : public IGame {
 public:
  /**
   * @brief Construct a new Game object.
   * 
   * @param ctx Context from the Session.
   */
  explicit Game(SessionContext ctx);

  void transition_to(std::unique_ptr<IGameHandler> handler) override;

  void operator()(const json::in::LobbyCreate&) override;
  void operator()(const json::in::LobbyJoin&) override;

 private:
  // Session::make() creates a Session that uses Game. That Game
  // has the same lifetime as the session so that Game's ctx_.session
  // will always be a valid pointer to the session.
  SessionContext ctx_;

  // The current state to forward events to.
  std::unique_ptr<IGameHandler> state_;
};

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
  static void transition_to_lobby(IGame&, SessionContext&, LobbyContext);
};

/**
 * @brief A state of IGame where the client is in a lobby. Lobby itself
 * is a state context that forwards events to the current state.
 * Possible substates are CharacterSelect, InGame, GameFinished.
 */
class Lobby : public ILobby {
 public:
  /**
   * @brief Construct a new Lobby object.
   * 
   * @param ctx Context from the lobby.
   */
  explicit Lobby(LobbyContext ctx);

  void transition_to(std::unique_ptr<ILobbyHandler>) override;

  void operator()(IGame&, SessionContext&, const json::in::LobbyLeave&) override;

 private:
  LobbyContext ctx_;

  // The current state to forward events to.
  std::unique_ptr<ILobbyHandler> state_;
};


/**
 * @brief A state of ILobby where the client is selecting their character.
 * Expecting client messages of character hovering/confirming, chat messages,
 * and leaving.
 */
class CharacterSelect : public ILobbyHandler {
 public:
 private:
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
