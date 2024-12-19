#include "lobby.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "character.hpp"
#include "lobby_mock.hpp"
#include "response.hpp"
#include "session_mock.hpp"

using std::shared_ptr, std::string;
using testing::NotNull, testing::IsNull, testing::InSequence;

namespace io_blair {

namespace resp = response;

class LobbyTest : public testing::Test {
   protected:
    static string code_;

    MockLobbyManager manager_;

    Lobby lobby_ = {code_, manager_};

    shared_ptr<MockSession> s1_ = std::make_shared<MockSession>();
    shared_ptr<MockSession> s2_ = std::make_shared<MockSession>();
    shared_ptr<MockSession> s3_ = std::make_shared<MockSession>();

    LobbyTest() {
        ON_CALL(*s1_, get_shared).WillByDefault([this] { return s1_; });
        ON_CALL(*s2_, get_shared).WillByDefault([this] { return s2_; });
        ON_CALL(*s3_, get_shared).WillByDefault([this] { return s3_; });
    }
};
string LobbyTest::code_ = "code";

TEST_F(LobbyTest, ThreeSessionsJoiningALobby) {
    {
        InSequence _;
        EXPECT_CALL(*s1_, write(resp::other_join()));
        EXPECT_CALL(*s2_, write(resp::other_leave()));
        EXPECT_CALL(*s2_, write(resp::other_join()));
    }

    EXPECT_TRUE(lobby_.join(*s1_));
    EXPECT_EQ(s1_.use_count(), 2);

    EXPECT_TRUE(lobby_.join(*s2_));
    EXPECT_EQ(s2_.use_count(), 2);

    EXPECT_FALSE(lobby_.join(*s3_));
    EXPECT_EQ(s3_.use_count(), 1);

    lobby_.leave(*s1_);
    EXPECT_EQ(s1_.use_count(), 1);

    EXPECT_TRUE(lobby_.join(*s3_));
    EXPECT_EQ(s3_.use_count(), 2);
}

TEST_F(LobbyTest, MsgAll) {
    static const string kMsg = "msg";
    {
        InSequence _;
        EXPECT_CALL(*s1_, write(kMsg));
        EXPECT_CALL(*s1_, write(resp::other_join()));

        EXPECT_CALL(*s1_, write(kMsg));
        EXPECT_CALL(*s2_, write(kMsg));
    }

    EXPECT_TRUE(lobby_.join(*s1_));

    lobby_.msg_all(kMsg);

    EXPECT_TRUE(lobby_.join(*s2_));

    lobby_.msg_all(kMsg);
}

TEST_F(LobbyTest, MsgOther) {
    static const string kMsg = "msg";
    {
        InSequence _;
        EXPECT_CALL(*s1_, write(resp::other_join()));
        EXPECT_CALL(*s2_, write(kMsg));
        EXPECT_CALL(*s1_, write(kMsg));
    }

    EXPECT_TRUE(lobby_.join(*s1_));

    lobby_.msg_other(*s1_, kMsg);

    EXPECT_TRUE(lobby_.join(*s2_));

    lobby_.msg_other(*s1_, kMsg);

    lobby_.msg_other(*s2_, kMsg);
}

TEST_F(LobbyTest, ConfirmCharacter) {
    {
        InSequence _;
        EXPECT_CALL(*s1_, write(resp::other_join()));

        EXPECT_CALL(*s2_, write(resp::confirm(Character::kUnset)));
        EXPECT_CALL(*s1_, write(resp::confirm(Character::kUnset)));

        EXPECT_CALL(*s2_, write(resp::confirm(Character::kIO)));

        EXPECT_CALL(*s2_, write(resp::confirm(Character::kBlair)));
        EXPECT_CALL(*s1_, write(resp::confirm(Character::kIO)));

        EXPECT_CALL(*s1_, write);
        EXPECT_CALL(*s2_, write);
    }

    EXPECT_TRUE(lobby_.join(*s1_));
    EXPECT_TRUE(lobby_.join(*s2_));

    EXPECT_FALSE(lobby_.confirm_character(*s1_, Character::kUnset));
    EXPECT_FALSE(lobby_.confirm_character(*s2_, Character::kUnset));

    EXPECT_FALSE(lobby_.confirm_character(*s1_, Character::kIO));
    EXPECT_FALSE(lobby_.confirm_character(*s2_, Character::kIO));

    EXPECT_FALSE(lobby_.confirm_character(*s1_, Character::kBlair));
    EXPECT_TRUE(lobby_.confirm_character(*s2_, Character::kIO));
}

TEST_F(LobbyTest, Leave) {
    {
        InSequence _;
        EXPECT_CALL(*s1_, write(resp::other_join()));

        EXPECT_CALL(*s2_, write(resp::other_leave()));
    }

    EXPECT_TRUE(lobby_.join(*s1_));
    EXPECT_TRUE(lobby_.join(*s2_));

    lobby_.leave(*s1_);
    EXPECT_EQ(s1_.use_count(), 1);

    lobby_.leave(*s2_);
    EXPECT_EQ(s2_.use_count(), 1);
}

TEST_F(LobbyTest, Full) {
    EXPECT_FALSE(lobby_.full());

    EXPECT_TRUE(lobby_.join(*s1_));
    EXPECT_FALSE(lobby_.full());

    EXPECT_TRUE(lobby_.join(*s2_));
    EXPECT_TRUE(lobby_.full());

    lobby_.leave(*s2_);
    EXPECT_FALSE(lobby_.full());
}

TEST_F(LobbyTest, Empty) {
    EXPECT_CALL(manager_, remove(code_));

    EXPECT_TRUE(lobby_.empty());

    EXPECT_TRUE(lobby_.join(*s1_));
    EXPECT_FALSE(lobby_.empty());

    EXPECT_TRUE(lobby_.join(*s2_));
    EXPECT_FALSE(lobby_.empty());

    lobby_.leave(*s1_);
    EXPECT_FALSE(lobby_.empty());

    lobby_.leave(*s2_);
    EXPECT_TRUE(lobby_.empty());
}

class LobbyManagerTest : public testing::Test {
   protected:
    LobbyManager manager_;

    shared_ptr<MockSession> s1_ = std::make_shared<MockSession>();
    shared_ptr<MockSession> s2_ = std::make_shared<MockSession>();
    shared_ptr<MockSession> s3_ = std::make_shared<MockSession>();

    LobbyManagerTest() {
        ON_CALL(*s1_, get_shared).WillByDefault([this] { return s1_; });
        ON_CALL(*s2_, get_shared).WillByDefault([this] { return s2_; });
        ON_CALL(*s3_, get_shared).WillByDefault([this] { return s3_; });
    }
};

TEST_F(LobbyManagerTest, Create) {
    auto lobby = manager_.create(*s1_);

    EXPECT_THAT(lobby, NotNull());
    EXPECT_EQ(lobby.use_count(), 2);

    EXPECT_EQ(s1_.use_count(), 2);
}

TEST_F(LobbyManagerTest, Join) {
    auto lobby = manager_.create(*s1_);

    EXPECT_THAT(manager_.join("not the code", *s2_), IsNull());

    EXPECT_EQ(manager_.join(lobby->code(), *s2_), lobby);

    EXPECT_THAT(manager_.join(lobby->code(), *s3_), IsNull());
}

TEST_F(LobbyManagerTest, Remove) {
    auto lobby = manager_.create(*s1_);

    EXPECT_THAT(lobby, NotNull());
    EXPECT_EQ(lobby.use_count(), 2);

    manager_.remove(lobby->code());
    EXPECT_EQ(lobby.use_count(), 1);
}
}  // namespace io_blair
