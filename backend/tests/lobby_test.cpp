#include "lobby.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "session_mock.hpp"

using std::shared_ptr;
using testing::NotNull, testing::IsNull, testing::Return;

namespace io_blair {
class LobbyManagerTest : public testing::Test {
   protected:
    LobbyManager manager_;

    shared_ptr<MockSession> s1_ = std::make_shared<MockSession>();
    shared_ptr<MockSession> s2_ = std::make_shared<MockSession>();
    shared_ptr<MockSession> s3_ = std::make_shared<MockSession>();
};

TEST_F(LobbyManagerTest, OneSessionCreatingAndLeavingALobby) {
    EXPECT_CALL(*s1_, get_shared).WillOnce([this] { return s1_; });

    shared_ptr<ILobby> lobby = manager_.create(*s1_);

    ASSERT_THAT(lobby, NotNull());

    EXPECT_FALSE(lobby->full()) << "Should only be 1/2 capacity";

    EXPECT_EQ(lobby.use_count(), 2)
        << "Local var and manager should each hold a ref";

    EXPECT_EQ(s1_.use_count(), 2)
        << "Local var and lobby should each hold a ref";

    lobby->leave(*s1_);

    EXPECT_EQ(lobby.use_count(), 1)
        << "Manager should have removed its ref to empty lobby";

    EXPECT_EQ(s1_.use_count(), 1)
        << "Lobby should have removed its ref to session. ";
}

TEST_F(LobbyManagerTest, ThreeSessionsTryingToJoinANewLobby) {
    EXPECT_CALL(*s1_, get_shared).WillOnce([this] { return s1_; });
    shared_ptr<ILobby> lobby1 = manager_.create(*s1_);

    ASSERT_THAT(lobby1, NotNull());

    EXPECT_FALSE(lobby1->full()) << "Should only be 1/2 capacity";

    EXPECT_EQ(lobby1.use_count(), 2)
        << "Local var and manager should each hold a ref";

    EXPECT_EQ(s1_.use_count(), 2)
        << "Local var and lobby should each hold a ref";

    const auto code = lobby1->code();

    EXPECT_CALL(*s2_, get_shared).WillOnce([this] { return s2_; });
    shared_ptr<ILobby> lobby2 = manager_.join(code, *s2_);

    ASSERT_THAT(lobby2, NotNull());

    ASSERT_EQ(lobby2, lobby1) << "Should be the same lobby";

    EXPECT_TRUE(lobby2->full()) << "Should be 2/2 capacity";

    EXPECT_EQ(lobby2.use_count(), 3)
        << "Two local vars and manager should each hold a ref";

    EXPECT_EQ(s2_.use_count(), 2)
        << "Local var and lobby should each hold a ref";

    EXPECT_THAT(manager_.join(code, *s3_), IsNull());

    EXPECT_TRUE(lobby2->full()) << "Should remain 2/2 capacity";

    EXPECT_EQ(lobby2.use_count(), 3)
        << "Should remain two local vars and manager "
           "holding a ref";

    EXPECT_EQ(s1_.use_count(), 2)
        << "Local var and lobby should remain ref holders";
    EXPECT_EQ(s2_.use_count(), 2)
        << "Local var and lobby should remain ref holders";
    EXPECT_EQ(s3_.use_count(), 1) << "Local var should be only ref";
}

static void three_sessions_before_leaving(shared_ptr<MockSession>& s1,
                                          shared_ptr<MockSession>& s2,
                                          shared_ptr<MockSession>& s3,
                                          shared_ptr<ILobby>& lobby,
                                          LobbyManager& manager) {
    EXPECT_CALL(*s2, get_shared).WillOnce(Return(s2));
    EXPECT_EQ(manager.join(lobby->code(), *s2), lobby);
    ASSERT_TRUE(lobby->full());

    EXPECT_EQ(s1.use_count(), 2);
    EXPECT_EQ(s2.use_count(), 2);

    EXPECT_THAT(manager.join(lobby->code(), *s3), IsNull());

    EXPECT_EQ(s1.use_count(), 2);
    EXPECT_EQ(s2.use_count(), 2);
    EXPECT_EQ(s3.use_count(), 1);
}

TEST_F(LobbyManagerTest, ThirdSessionJoinsLobbyAfterFirstSessionLeaves) {
    EXPECT_CALL(*s1_, get_shared).WillOnce([this] { return s1_; });
    shared_ptr<ILobby> lobby = manager_.create(*s1_);

    EXPECT_CALL(*s3_, get_shared).WillOnce([this] { return s3_; });

    {
        SCOPED_TRACE("BeforeFirstSessionLeaves");
        three_sessions_before_leaving(s1_, s2_, s3_, lobby, manager_);
        ASSERT_FALSE(testing::Test::HasFailure());
    }

    lobby->leave(*s1_);
    EXPECT_FALSE(lobby->full());

    EXPECT_EQ(s1_.use_count(), 1);
    EXPECT_EQ(s2_.use_count(), 2);
    EXPECT_EQ(s3_.use_count(), 1);

    EXPECT_EQ(manager_.join(lobby->code(), *s3_), lobby);

    EXPECT_EQ(s1_.use_count(), 1);
    EXPECT_EQ(s2_.use_count(), 2);
    EXPECT_EQ(s3_.use_count(), 2);
}

TEST_F(LobbyManagerTest, ThirdSessionJoinsLobbyAfterSecondSessionLeaves) {
    EXPECT_CALL(*s1_, get_shared).WillOnce([this] { return s1_; });
    shared_ptr<ILobby> lobby = manager_.create(*s1_);

    EXPECT_CALL(*s3_, get_shared).WillOnce([this] { return s3_; });

    {
        SCOPED_TRACE("BeforeSecondSessionLeaves");
        three_sessions_before_leaving(s1_, s2_, s3_, lobby, manager_);
        ASSERT_FALSE(testing::Test::HasFailure());
    }

    lobby->leave(*s2_);
    EXPECT_FALSE(lobby->full());

    EXPECT_EQ(s1_.use_count(), 2);
    EXPECT_EQ(s2_.use_count(), 1);
    EXPECT_EQ(s3_.use_count(), 1);

    EXPECT_EQ(manager_.join(lobby->code(), *s3_), lobby);

    EXPECT_EQ(s1_.use_count(), 2);
    EXPECT_EQ(s2_.use_count(), 1);
    EXPECT_EQ(s3_.use_count(), 2);
}
}  // namespace io_blair
