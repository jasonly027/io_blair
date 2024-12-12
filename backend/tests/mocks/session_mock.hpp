#pragma once

#include <gmock/gmock.h>

#include <string>

#include "session.hpp"

namespace io_blair {
class MockSession : public ISession {
   public:
    MOCK_METHOD(void, run, (), (override));
    MOCK_METHOD(void, write, (std::string msg), (override));
    MOCK_METHOD(bool, join_new_lobby, (), (override));
    MOCK_METHOD(bool, join_lobby, (const std::string& code), (override));
    MOCK_METHOD(void, leave_lobby, (), (override));
};
}  // namespace io_blair