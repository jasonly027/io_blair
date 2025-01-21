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
};

}  // namespace io_blair
