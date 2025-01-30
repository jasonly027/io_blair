#include "lobby_data.hpp"
#include "mock/mock_session.hpp"
#include "gmock/gmock.h"
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <string>

namespace io_blair::testing {
using std::make_shared;
using std::nullopt;
using std::string;
using ::testing::_;
using ::testing::Matcher;
using ::testing::NiceMock;

TEST(LobbyDataShould, SaveCode) {
  const string code = "arbitrary";

  LobbyData data(code);

  EXPECT_EQ(data.code_, code);
}

TEST(LobbyDataShould, PlaceSessionInEmptyData) {
  auto sess = make_shared<MockSession>();
  LobbyData data("");

  EXPECT_NE(data.join(sess), nullopt);
}

TEST(LobbyDataShould, SendOnOtherSessionJoining) {
  LobbyData data("");
  auto s1 = make_shared<MockSession>();
  auto s2 = make_shared<MockSession>();

  EXPECT_CALL(*s1, async_send(Matcher<string>(_)));

  data.join(s1);
  data.join(s2);
}

TEST(LobbyDataShould, NotPlaceSessionWhenFull) {
  LobbyData data("");
  auto s1 = make_shared<NiceMock<MockSession>>();
  auto s2 = make_shared<NiceMock<MockSession>>();
  auto s3 = make_shared<NiceMock<MockSession>>();

  EXPECT_NE(data.join(s1), nullopt);
  EXPECT_NE(data.join(s2), nullopt);
  EXPECT_EQ(data.join(s3), nullopt);
}

TEST(LobbyDataShould, ReplaceFirstSessionWhenExpired) {
  LobbyData data("");
  auto s1 = make_shared<NiceMock<MockSession>>();
  auto s2 = make_shared<NiceMock<MockSession>>();
  auto s3 = make_shared<NiceMock<MockSession>>();

  EXPECT_NE(data.join(s1), nullopt);
  EXPECT_NE(data.join(s2), nullopt);
  s1.reset();
  EXPECT_NE(data.join(s3), nullopt);
}

TEST(LobbyDataShould, ReplaceSecondSessionWhenExpired) {
  LobbyData data("");
  auto s1 = make_shared<NiceMock<MockSession>>();
  auto s2 = make_shared<NiceMock<MockSession>>();
  auto s3 = make_shared<NiceMock<MockSession>>();

  EXPECT_NE(data.join(s1), nullopt);
  EXPECT_NE(data.join(s2), nullopt);
  s2.reset();
  EXPECT_NE(data.join(s3), nullopt);
}

} // namespace io_blair::testing
