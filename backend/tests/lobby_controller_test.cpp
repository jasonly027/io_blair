#include "lobby_controller.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <optional>
#include <string>

#include "mock/mock_session.hpp"


namespace io_blair::testing {
using std::make_shared;
using std::nullopt;
using std::string;
using ::testing::_;
using ::testing::Matcher;
using ::testing::NiceMock;

TEST(LobbyControllerShould, SaveCode) {
  const string code = "arbitrary";

  LobbyController controller(code);

  EXPECT_EQ(controller.code_, code);
}

TEST(LobbyControllerShould, PlaceSessionInEmptyData) {
  auto sess = make_shared<MockSession>();
  LobbyController controller("");

  EXPECT_NE(controller.join(sess), nullopt);
}

TEST(LobbyControllerShould, SendOnOtherSessionJoining) {
  LobbyController controller("");
  auto s1 = make_shared<MockSession>();
  auto s2 = make_shared<MockSession>();

  EXPECT_CALL(*s1, async_send(Matcher<string>(_)));

  controller.join(s1);
  controller.join(s2);
}

TEST(LobbyControllerShould, NotPlaceSessionWhenFull) {
  LobbyController controller("");
  auto s1 = make_shared<NiceMock<MockSession>>();
  auto s2 = make_shared<NiceMock<MockSession>>();
  auto s3 = make_shared<NiceMock<MockSession>>();

  EXPECT_NE(controller.join(s1), nullopt);
  EXPECT_NE(controller.join(s2), nullopt);
  EXPECT_EQ(controller.join(s3), nullopt);
}

TEST(LobbyControllerShould, ReplaceFirstSessionWhenExpired) {
  LobbyController controller("");
  auto s1 = make_shared<NiceMock<MockSession>>();
  auto s2 = make_shared<NiceMock<MockSession>>();
  auto s3 = make_shared<NiceMock<MockSession>>();

  EXPECT_NE(controller.join(s1), nullopt);
  EXPECT_NE(controller.join(s2), nullopt);
  s1.reset();
  EXPECT_NE(controller.join(s3), nullopt);
}

TEST(LobbyControllerShould, ReplaceSecondSessionWhenExpired) {
  LobbyController controller("");
  auto s1 = make_shared<NiceMock<MockSession>>();
  auto s2 = make_shared<NiceMock<MockSession>>();
  auto s3 = make_shared<NiceMock<MockSession>>();

  EXPECT_NE(controller.join(s1), nullopt);
  EXPECT_NE(controller.join(s2), nullopt);
  s2.reset();
  EXPECT_NE(controller.join(s3), nullopt);
}

}  // namespace io_blair::testing
