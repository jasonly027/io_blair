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
    MOCK_METHOD(bool, join, (ISession& session), (override));
    MOCK_METHOD(void, msg_other, (const ISession& session, std::string msg),
                (override));
    MOCK_METHOD(void, confirm_character,
                (const ISession& session, Character msg), (override));
    MOCK_METHOD(void, leave, (const ISession& session), (override));
    MOCK_METHOD(bool, full, (), (const, override));
    MOCK_METHOD(const std::string&, code, (), (const, override));
};

class MockLobbyManager : public ILobbyManager {
   public:
    MOCK_METHOD(std::shared_ptr<ILobby>, create,
                (ISession& session), (override));

    MOCK_METHOD(std::shared_ptr<ILobby>, join,
                (const std::string& code, ISession& session),
                (override));

    MOCK_METHOD(void, remove, (const std::string& code), (override));
};
}  // namespace io_blair
