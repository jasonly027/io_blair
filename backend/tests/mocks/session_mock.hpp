#pragma once

#include <gmock/gmock.h>

#include <memory>
#include <string>

#include "session.hpp"

namespace io_blair {
class MockSession : public ISession {
   public:
    MOCK_METHOD(void, run, (), (override));
    MOCK_METHOD(void, write, (std::string msg), (override));
    MOCK_METHOD(std::shared_ptr<ISession>, get_shared, (), (override));
};
}  // namespace io_blair
