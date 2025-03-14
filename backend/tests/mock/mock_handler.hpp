#pragma once

#include <gmock/gmock.h>

#include "ihandler.hpp"


namespace io_blair::testing {
class MockHandler : public IHandler {
 public:
  MOCK_METHOD(void, EvLobbyCreate, (const json::in::LobbyCreate&));
  inline void operator()(const json::in::LobbyCreate& ev) override {
    return EvLobbyCreate(ev);
  }

  MOCK_METHOD(void, EvLobbyJoin, (const json::in::LobbyJoin&));
  inline void operator()(const json::in::LobbyJoin& ev) override {
    return EvLobbyJoin(ev);
  }

  MOCK_METHOD(void, EvLobbyLeave, (const json::in::LobbyLeave&));
  inline void operator()(const json::in::LobbyLeave& ev) override {
    return EvLobbyLeave(ev);
  }

  MOCK_METHOD(void, EvChat, (const json::in::Chat&));
  inline void operator()(const json::in::Chat& ev) override {
    return EvChat(ev);
  }

  MOCK_METHOD(void, EvHover, (const json::in::CharacterHover&));
  inline void operator()(const json::in::CharacterHover& ev) override {
    return EvHover(ev);
  }

  MOCK_METHOD(void, EvMove, (const json::in::CharacterMove&));
  inline void operator()(const json::in::CharacterMove& ev) override {
    return EvMove(ev);
  }
};

}  // namespace io_blair
