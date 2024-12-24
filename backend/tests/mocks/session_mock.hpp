#pragma once

#include <gmock/gmock.h>

#include <memory>
#include <string>

#include "lobby.hpp"
#include "session.hpp"

namespace io_blair {
class MockSession : public ISession {
   public:
    MOCK_METHOD(void, run, (), (override));
    MOCK_METHOD(void, write, (std::string msg), (override));

    MOCK_METHOD(State, state, (), (const, override));
    MOCK_METHOD(void, set_state, (State state), (override));

    MOCK_METHOD(void, try_lobby_update, (std::string data), (override));
    MOCK_METHOD(void, set_lobby, (std::shared_ptr<ILobby> lobby), (override));

    MOCK_METHOD(std::shared_ptr<ISession>, get_shared, (), (override));
    MOCK_METHOD(std::shared_ptr<const ISession>, get_shared, (),
                (const, override));
};
}  // namespace io_blair
