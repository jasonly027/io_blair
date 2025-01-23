/**
 * @file ihandler.hpp
 */
#pragma once

#include "json.hpp"

namespace io_blair {
/**
 * @brief An interface for handlers usable with JsonDecoder. The default
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
};

}  // namespace io_blair
