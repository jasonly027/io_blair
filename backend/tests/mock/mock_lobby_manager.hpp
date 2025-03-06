#pragma once

#include <gmock/gmock.h>

#include <optional>

#include "ilobby_manager.hpp"
#include "lobby_context.hpp"


namespace io_blair::testing {
class MockLobbyManager : public ILobbyManager {
 public:
  MOCK_METHOD(LobbyContext, create, (std::weak_ptr<ISession>), (override));
  MOCK_METHOD(std::optional<LobbyContext>, join,
              (std::weak_ptr<ISession> session, std::string_view code), (override));
  MOCK_METHOD(void, leave, (const std::weak_ptr<ISession>& session, std::string_view code),
              (override));
};
}  // namespace io_blair::testing
