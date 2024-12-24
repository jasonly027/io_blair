#pragma once

#include <gmock/gmock.h>

#include <lobby.hpp>
#include <session.hpp>
#include <string>

namespace io_blair {

class MockLobby : public ILobby {
   public:
    MOCK_METHOD(void, join, (ISession & session), (override));
    MOCK_METHOD(void, update, (ISession & session, std::string data),
                (override));
    MOCK_METHOD(void, leave, (ISession & session), (override));
    MOCK_METHOD(bool, full, (), (const, override));
    MOCK_METHOD(bool, empty, (), (const, override));
    MOCK_METHOD(State, state, (), (const, override));
    MOCK_METHOD(const std::string&, code, (), (const, override));
};

class MockLobbyManager : public ILobbyManager {
   public:
    MOCK_METHOD(void, create, (ISession & session), (override));
    MOCK_METHOD(void, join, (const std::string& code, ISession& session),
                (override));
    MOCK_METHOD(void, remove, (const std::string& code), (override));
};
}  // namespace io_blair
