/**
 * @file ihandler.hpp
 */
#pragma once

#include "event.hpp"
#include "json.hpp"

namespace io_blair {
/**
 * @brief An interface for an event handler. The default
 * implementation for the event handlers is to do nothing.
 */
class IHandler {
 public:
  virtual ~IHandler() = default;
  virtual void operator()(const json::in::LobbyCreate&);
  virtual void operator()(const json::in::LobbyJoin&);
  virtual void operator()(const json::in::LobbyLeave&);
  virtual void operator()(const json::in::Chat&);
  virtual void operator()(const json::in::CharacterHover&);
  virtual void operator()(const json::in::CharacterConfirm&);
  virtual void operator()(const json::in::CharacterMove&);
  virtual void operator()(SessionEvent);
};

}  // namespace io_blair
