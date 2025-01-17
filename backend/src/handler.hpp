/**
 * @file handler.hpp
 */
#pragma once

#include <string_view>

#include "json.hpp"
#include "lobby.hpp"

namespace io_blair {
/**
 * @brief An interface for handlers usable with JsonDecoder. The default
 * implementation for the operator()'s is to do nothing.
 */
class IHandler {
 public:
  virtual ~IHandler() = default;
  virtual void operator()(const json::in::LobbyCreate&);
  virtual void operator()(const json::in::LobbyJoin&);
};

/**
 * @brief Represents a state in Game.
 */
class GameState : public IHandler {};

class Game;
class SessionContext;

/**
 * @brief A state of Game before the client has joined a lobby.
 * Expecting client messages to create/join a lobby.
 */
class Prelobby : public GameState {
 public:
  explicit Prelobby(Game& game, SessionContext& ctx);

  void operator()(const json::in::LobbyCreate&) override;
  void operator()(const json::in::LobbyJoin&) override;

 private:
  Game& game_;
  SessionContext& ctx_;
};

/**
 * @brief Represents a State of Lobby.
 */
class LobbyState : public IHandler {};

/**
 * @brief A state of Lobby where the client is selecting their character.
 * Expecting client messages of character hovering/confirming, chat messages,
 * and leaving.
 */
class CharacterSelect : public LobbyState {
 public:
 private:
};

/**
 * @brief A state of Lobby where the client is trying to finish the game.
 */
class InGame : public LobbyState {
 public:
 private:
};

/**
 * @brief A state of Lobby where the client has completed the game.
 */
class GameFinished : public LobbyState {
 public:
 private:
};

/**
 * @brief A state of Game where the client is in a lobby. The substates
 * of this state are CharacterSelect, InGame, GameFinished.
 */
class Lobby : public GameState {
 public:
  explicit Lobby(Game& game, SessionContext& session_ctx);

  void set_lobby_ctx(LobbyContext& lobby_ctx);

  std::string_view code() const;

 private:
  Game& game_;
  SessionContext& session_ctx_;
  LobbyContext* lobby_ctx_;

  CharacterSelect character_select_;
  InGame in_game_;
  GameFinished game_finished_;
  LobbyState* state_;
};

/**
 * @brief A state manager that forwards events to the current state. Possible states
 * are Prelobby and Lobby.
 */
class Game : public IHandler {
 public:
  explicit Game(SessionContext ctx);

  Prelobby& prelobby();

  Lobby& lobby();

  void set_state(GameState& state);

  void operator()(const json::in::LobbyCreate&) override;
  void operator()(const json::in::LobbyJoin&) override;

 private:
  Prelobby prelobby_;
  Lobby lobby_;
  GameState* state_;
};

}  // namespace io_blair
