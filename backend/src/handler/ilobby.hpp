/**
 * @file ilobby.hpp
 */
#pragma once

#include <memory>

#include "igame.hpp"
#include "json.hpp"
#include "lobby_context.hpp"
#include "session_context.hpp"


namespace io_blair {
class ILobbyHandler;

/**
 * @brief An interface for a state context of ILobbyHandler. Implementations
 * should look to forward the event handlers of IGameHandler to the current
 * ILobbyHandler state of ILobby.
 */
class ILobby : public IGameHandler {
 public:
  /**
   * @brief Transitions the current state to \p handler.
   * 
   * @param handler The new state.
   */
  virtual void transition_to(std::unique_ptr<ILobbyHandler>) = 0;
};

/**
 * @brief An interface for a state of ILobby. Event handlers do nothing by default.
 */
class ILobbyHandler {
 public:
  virtual ~ILobbyHandler() = default;
  virtual void operator()(ILobby&, SessionContext&, LobbyContext&, const json::in::CharacterHover&);
  virtual void operator()(ILobby&, SessionContext&, LobbyContext&,
                          const json::in::CharacterConfirm&);
};
}  // namespace io_blair
