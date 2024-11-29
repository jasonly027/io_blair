#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <lobby.hpp>
#include <memory>

namespace io_blair {

class MockSession {
   public:
    MOCK_METHOD(std::shared_ptr<MockSession>, shared_from_this, ());
    MOCK_METHOD(std::shared_ptr<Lobby>&, lobby, ());
    MOCK_METHOD(void, write, (const std::shared_ptr<std::string>&));
};

}  // namespace io_blair
