#pragma once

#include <memory>
#include <string>

#include "character.hpp"
#include "gmock/gmock.h"
#include "lobby.hpp"
#include "session.hpp"

namespace io_blair {

class MockLobby : public ILobby {
   public:
    MOCK_METHOD(bool, join, (std::shared_ptr<ISession> ptr), (override));
    MOCK_METHOD(void, msg, (const ISession& session, std::string msg),
                (override));
    MOCK_METHOD(bool, confirm_character,
                (const ISession& session, Character msg), (override));
    MOCK_METHOD(void, leave, (const ISession& session), (override));
    MOCK_METHOD(bool, full, (), (const, override));
    MOCK_METHOD(const std::string&, code, (), (const, override));
};

class MockLobbyManager : public ILobbyManager {
   public:
    MOCK_METHOD(std::shared_ptr<ILobby>, create,
                (std::shared_ptr<ISession> session), (override));

    MOCK_METHOD(std::shared_ptr<ILobby>, join,
                (const std::string& code, std::shared_ptr<ISession> session),
                (override));

    MOCK_METHOD(void, remove, (const std::string& code), (override));
};
}  // namespace io_blair