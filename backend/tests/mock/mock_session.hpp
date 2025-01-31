#pragma once

#include <gmock/gmock.h>

#include <memory>
#include <string>

#include "event.hpp"
#include "isession.hpp"


namespace io_blair::testing {
class MockSession : public ISession {
 public:
  MOCK_METHOD(void, async_send, (std::shared_ptr<const std::string> msg), (override));
  MOCK_METHOD(void, async_send, (std::string msg), (override));
  MOCK_METHOD(void, async_handle, (SessionEvent ev), (override));
};

}  // namespace io_blair::testing
