#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <json.hpp>

#include "mock/mock_handler.hpp"


namespace io_blair::testing {
using ::testing::StrictMock;

TEST(JsonDecodeShould, NotCallHandlerOnInvalidJson) {
  StrictMock<MockHandler> handler;

  json::decode("", handler);
  json::decode("{}", handler);
  json::decode("invalid json", handler);

  FAIL();
}

}  // namespace io_blair::testing
