#include "lobby_controller.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <optional>
#include <string>

#include "character.hpp"
#include "json.hpp"
#include "mock/mock_session.hpp"


namespace io_blair::testing {
using std::make_shared;
using std::nullopt;
using std::shared_ptr;
using std::string;
using ::testing::HasSubstr;
using ::testing::Matcher;
using ::testing::NiceMock;
using ::testing::Pointee;
using ::testing::StrictMock;
namespace jout = json::out;

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

  EXPECT_CALL(*s1, async_send(Matcher<string>(HasSubstr("lobbyJoin"))));
  EXPECT_CALL(*s1, async_send(Matcher<string>(HasSubstr("lobbyOtherJoin"))));
  EXPECT_CALL(*s2, async_send(Matcher<string>(HasSubstr("lobbyJoin"))));

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

TEST(LobbyControllerShould, SendOnOtherSessionLeaving) {
  LobbyController controller("");
  auto s1 = make_shared<NiceMock<MockSession>>();
  auto s2 = make_shared<NiceMock<MockSession>>();

  EXPECT_CALL(*s1, async_send(Matcher<string>(HasSubstr("lobbyJoin"))));
  EXPECT_CALL(*s1, async_send(Matcher<string>(HasSubstr("lobbyOtherJoin"))));
  EXPECT_CALL(*s2, async_send(Matcher<string>(HasSubstr("lobbyJoin"))));
  EXPECT_CALL(*s2, async_send(jout::lobby_other_leave()));

  controller.join(s1);
  controller.join(s2);
  controller.leave(s1);
}

class LobbyControllerFShould : public ::testing::Test {
 protected:
  std::shared_ptr<StrictMock<MockSession>> s1_ = std::make_shared<StrictMock<MockSession>>();
  std::shared_ptr<StrictMock<MockSession>> s2_ = std::make_shared<StrictMock<MockSession>>();

  Player p1_;
  Player p2_;

  LobbyControllerFShould() {
    p1_.character = Character::unknown;
    p2_.character = Character::unknown;
  }
};

TEST_F(LobbyControllerFShould, NotSetCharacterWhenAlreadyCharacter) {
  p1_.session.try_set(s1_);
  p1_.character = Character::Io;
  p2_.session.try_set(s2_);
  LobbyController controller("");

  // Shouldn't trigger StrictMock's
  controller.set_character(p1_, p2_, p1_.character);
}

TEST_F(LobbyControllerFShould, NotSetCharacterWhenOtherIsAlreadyCharacter) {
  p1_.session.try_set(s1_);
  p1_.character = Character::Io;
  p2_.session.try_set(s2_);
  p2_.character = Character::Blair;
  LobbyController controller("");

  controller.set_character(p1_, p2_, p2_.character);

  EXPECT_NE(p1_.character, p2_.character);
}

TEST_F(LobbyControllerFShould, SetCharacterAndSendOther) {
  constexpr Character kCharacter = Character::Io;
  EXPECT_CALL(*s2_, async_send(jout::character_confirm(kCharacter)));

  p1_.session.try_set(s1_);
  p2_.session.try_set(s2_);
  LobbyController controller("");

  controller.set_character(p1_, p2_, kCharacter);

  EXPECT_EQ(p1_.character, kCharacter);
}

TEST_F(LobbyControllerFShould, SetCharacterToUnknown) {
  EXPECT_CALL(*s2_, async_send(jout::character_confirm(Character::unknown)));

  p1_.session.try_set(s1_);
  p1_.character = Character::Io;
  p2_.session.try_set(s2_);
  LobbyController controller("");

  controller.set_character(p1_, p2_, Character::unknown);

  EXPECT_EQ(p1_.character, Character::unknown);
}

TEST_F(LobbyControllerFShould, SetCharactersAndTransitionToInGame) {
  using MatcherSharedStr = Matcher<shared_ptr<const string>>;

  EXPECT_CALL(*s1_, async_send(jout::character_confirm(Character::Blair)));
  EXPECT_CALL(*s1_, async_handle(SessionEvent::kTransitionToInGame));
  EXPECT_CALL(*s1_, async_send(MatcherSharedStr(Pointee(jout::transition_to_ingame()))));

  EXPECT_CALL(*s2_, async_send(jout::character_confirm(Character::Io)));
  EXPECT_CALL(*s2_, async_handle(SessionEvent::kTransitionToInGame));
  EXPECT_CALL(*s2_, async_send(MatcherSharedStr(Pointee(jout::transition_to_ingame()))));

  p1_.session.try_set(s1_);
  p2_.session.try_set(s2_);
  LobbyController controller("");

  controller.set_character(p1_, p2_, Character::Io);
  controller.set_character(p2_, p1_, Character::Blair);
}

}  // namespace io_blair::testing
