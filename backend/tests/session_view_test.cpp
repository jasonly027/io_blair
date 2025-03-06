#include "session_view.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "mock/mock_session.hpp"

namespace io_blair::testing {
using std::make_shared;
using std::string;
using std::weak_ptr;
using ::testing::StrictMock;

TEST(SessionViewShould, ForwardSendStrToRealSession) {
  auto sess = make_shared<MockSession>();
  const string str = "arbitrary";

  EXPECT_CALL(*sess, async_send(str));

  SessionView view(sess);
  view.async_send(str);
}

TEST(SessionViewShould, ForwardSendSharedStrToRealSession) {
  auto sess = make_shared<MockSession>();
  const auto str = make_shared<const string>("arbitrary");

  EXPECT_CALL(*sess, async_send(str));

  SessionView view(sess);
  view.async_send(str);
}

TEST(SessionViewShould, NotForwardSendWhenReset) {
  auto sess = make_shared<StrictMock<MockSession>>();
  SessionView view(sess);
  view.reset();

  view.async_send("");
  view.async_send(make_shared<const string>());
}

TEST(SessionViewShould, SetWhenExpired) {
  SessionView view;

  EXPECT_TRUE(view.try_set(weak_ptr<MockSession>{}));
}

TEST(SessionViewShould, NotSetWhenNotExpired) {
  auto sess = make_shared<MockSession>();
  SessionView view(sess);

  EXPECT_FALSE(view.try_set(weak_ptr<MockSession>{}));
}

TEST(SessionViewEqualityShould, BeFalseWhenExpired) {
  SessionView view;
  auto ptr = make_shared<MockSession>();

  EXPECT_NE(view , ptr);
  EXPECT_NE(view , nullptr);
}

TEST(SessionViewEqualityShould, BeFalseWhenPtrIsNull) {
  SessionView v1;
  EXPECT_NE(v1, nullptr);

  auto sess = make_shared<MockSession>();
  SessionView v2(sess);
  EXPECT_NE(v2, nullptr);
}

TEST(SessionViewEqualityShould, BeFalseWhenDiffSessions) {
  auto s1 = make_shared<MockSession>();
  auto s2 = make_shared<MockSession>();
  SessionView view(s1);

  EXPECT_NE(view, s2);
}

TEST(SessionViewEqualityShould, BeTrueWhenSameSession) {
  auto sess = make_shared<MockSession>();
  SessionView view(sess);

  EXPECT_EQ(view, sess);
}

TEST(SessionViewEqualityShould, BeTrueWhenViewingItself) {
  auto view = make_shared<SessionView>();
  view->try_set(view);

  EXPECT_EQ(*view, view);
}

} // namespace io_blair::testing
