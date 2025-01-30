#pragma once

#include <gmock/gmock.h>

#include <memory>

#include "igame.hpp"


namespace io_blair::testing {
class MockGame : public IGame {
 public:
  MOCK_METHOD(void, transition_to, (std::unique_ptr<IGameHandler>), (override));
};
}  // namespace io_blair::testing
