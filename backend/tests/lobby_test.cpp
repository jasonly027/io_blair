#include "lobby.hpp"

#include <gtest/gtest.h>

#include <memory>

#include "gmock/gmock.h"
#include "session_mock.hpp"

using std::shared_ptr;
using testing::NotNull, testing::IsNull;

namespace io_blair {
class LobbyManagerTest : public testing::Test {
   protected:
    LobbyManager manager_;
};

TEST_F(LobbyManagerTest, OneSessionCreatingAndLeavingALobby) {
    auto session = std::make_shared<MockSession>();
    shared_ptr<ILobby> lobby = manager_.create(session);

    ASSERT_THAT(lobby, NotNull());

    EXPECT_FALSE(lobby->full()) << "Should only be 1/2 capacity";

    EXPECT_EQ(lobby.use_count(), 2)
        << "Local var and manager should each hold a ref";

    EXPECT_EQ(session.use_count(), 2)
        << "Local var and lobby should each hold a ref";

    lobby->leave(*session);

    EXPECT_EQ(lobby.use_count(), 1)
        << "Manager should have removed its ref to empty lobby";

    EXPECT_EQ(session.use_count(), 1)
        << "Lobby should have removed its ref to session. ";
}

TEST_F(LobbyManagerTest, ThreeSessionsTryingToJoinANewLobby) {
    auto s1 = std::make_shared<MockSession>();
    shared_ptr<ILobby> lobby1 = manager_.create(s1);

    ASSERT_THAT(lobby1, NotNull());

    EXPECT_FALSE(lobby1->full()) << "Should only be 1/2 capacity";

    EXPECT_EQ(lobby1.use_count(), 2)
        << "Local var and manager should each hold a ref";

    EXPECT_EQ(s1.use_count(), 2)
        << "Local var and lobby should each hold a ref";

    auto s2 = std::make_shared<MockSession>();
    const auto code = lobby1->code();
    shared_ptr<ILobby> lobby2 = manager_.join(code, s2);

    ASSERT_THAT(lobby2, NotNull());

    ASSERT_EQ(lobby2, lobby1) << "Should be the same lobby";

    EXPECT_TRUE(lobby2->full()) << "Should be 2/2 capacity";

    EXPECT_EQ(lobby2.use_count(), 3)
        << "Two local vars and manager should each hold a ref";

    EXPECT_EQ(s2.use_count(), 2)
        << "Local var and lobby should each hold a ref";

    auto s3 = std::make_shared<MockSession>();

    EXPECT_THAT(manager_.join(code, s3), IsNull());

    EXPECT_TRUE(lobby2->full()) << "Should remain 2/2 capacity";

    EXPECT_EQ(lobby2.use_count(), 3)
        << "Should remain two local vars and manager "
           "holding a ref";

    EXPECT_EQ(s1.use_count(), 2)
        << "Local var and lobby should remain ref holders";
    EXPECT_EQ(s2.use_count(), 2)
        << "Local var and lobby should remain ref holders";
    EXPECT_EQ(s3.use_count(), 1) << "Local var should be only ref";
}

static void three_sessions_before_leaving(shared_ptr<MockSession>& s1,
                                          shared_ptr<MockSession>& s2,
                                          shared_ptr<MockSession>& s3,
                                          shared_ptr<ILobby>& lobby,
                                          LobbyManager& manager) {
    EXPECT_EQ(manager.join(lobby->code(), s2), lobby);
    ASSERT_TRUE(lobby->full());

    EXPECT_EQ(s1.use_count(), 2);
    EXPECT_EQ(s2.use_count(), 2);

    EXPECT_THAT(manager.join(lobby->code(), s3), IsNull());

    EXPECT_EQ(s1.use_count(), 2);
    EXPECT_EQ(s2.use_count(), 2);
    EXPECT_EQ(s3.use_count(), 1);
}

TEST_F(LobbyManagerTest, ThirdSessionJoinsLobbyAfterFirstSessionLeaves) {
    auto s1 = std::make_shared<MockSession>();
    auto s2 = std::make_shared<MockSession>();
    auto s3 = std::make_shared<MockSession>();

    shared_ptr<ILobby> lobby = manager_.create(s1);

    {
        SCOPED_TRACE("BeforeFirstSessionLeaves");
        three_sessions_before_leaving(s1, s2, s3, lobby, manager_);
    }

    lobby->leave(*s1);
    EXPECT_FALSE(lobby->full());

    EXPECT_EQ(s1.use_count(), 1);
    EXPECT_EQ(s2.use_count(), 2);
    EXPECT_EQ(s3.use_count(), 1);

    EXPECT_EQ(manager_.join(lobby->code(), s3), lobby);

    EXPECT_EQ(s1.use_count(), 1);
    EXPECT_EQ(s2.use_count(), 2);
    EXPECT_EQ(s3.use_count(), 2);
}

TEST_F(LobbyManagerTest, ThirdSessionJoinsLobbyAfterSecondSessionLeaves) {
    auto s1 = std::make_shared<MockSession>();
    auto s2 = std::make_shared<MockSession>();
    auto s3 = std::make_shared<MockSession>();

    shared_ptr<ILobby> lobby = manager_.create(s1);

    {
        SCOPED_TRACE("BeforeSecondSessionLeaves");
        three_sessions_before_leaving(s1, s2, s3, lobby, manager_);
    }

    lobby->leave(*s2);
    EXPECT_FALSE(lobby->full());

    EXPECT_EQ(s1.use_count(), 2);
    EXPECT_EQ(s2.use_count(), 1);
    EXPECT_EQ(s3.use_count(), 1);

    EXPECT_EQ(manager_.join(lobby->code(), s3), lobby);

    EXPECT_EQ(s1.use_count(), 2);
    EXPECT_EQ(s2.use_count(), 1);
    EXPECT_EQ(s3.use_count(), 2);
}
}  // namespace io_blair
