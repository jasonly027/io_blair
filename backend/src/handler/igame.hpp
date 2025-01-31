/**
 * @file igame.hpp
 */
#pragma once

#include <memory>

#include "event.hpp"
#include "json.hpp"
#include "session_context.hpp"


namespace io_blair {
class IGameHandler;

/**
 * @brief An interface for a state context of IGameHandler.
 */
class IGame {
 public:
  /**
   * @brief Transitions the current state to \p handler.
   * 
   * @param handler The new state.
   */
  virtual void transition_to(std::unique_ptr<IGameHandler>) = 0;
};

/**
 * @brief An interface for a state of IGame. Event handlers do nothing by default.
 */
class IGameHandler {
 public:
  virtual ~IGameHandler() = default;
  virtual void operator()(IGame&, SessionContext&, const json::in::LobbyCreate&);
  virtual void operator()(IGame&, SessionContext&, const json::in::LobbyJoin&);
  virtual void operator()(IGame&, SessionContext&, const json::in::LobbyLeave&);
  virtual void operator()(IGame&, SessionContext&, const json::in::Chat&);
  virtual void operator()(IGame&, SessionContext&, const json::in::CharacterHover&);
  virtual void operator()(IGame&, SessionContext&, const json::in::CharacterConfirm&);
  virtual void operator()(IGame&, SessionContext&, SessionEvent);
};

}  // namespace io_blair
