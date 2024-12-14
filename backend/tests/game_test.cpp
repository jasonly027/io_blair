#include <gtest/gtest.h>

#include <game.hpp>
#include <memory>
#include <string>
#include <optional>
#include <rfl/json.hpp>

#include "gmock/gmock.h"
#include "lobby_mock.hpp"
#include "response.hpp"
#include "session_mock.hpp"

using std::string, std::optional, std::shared_ptr;
namespace json = rfl::json;
using testing::Return, testing::ReturnRef, testing::StrictMock,
    testing::InSequence;

namespace io_blair {

using request::Prelobby;
using State = Game::State;
namespace resp = response;

class GameTest : public testing::Test {
   protected:
    void SetUp() override {
        ASSERT_EQ(g1_.state(), State::kPrelobby);
        ASSERT_EQ(g2_.state(), State::kPrelobby);
    }

    MockLobbyManager manager_;
    shared_ptr<MockLobby> lobby_ = std::make_shared<MockLobby>();

    MockSession s1_;
    Game g1_{s1_, manager_};

    MockSession s2_;
    Game g2_{s2_, manager_};
};

TEST_F(GameTest, RequestIsInvalidJson) {
    StrictMock<MockSession> session;
    StrictMock<MockLobbyManager> manager;
    Game game(session, manager);

    // Game should return immediately on invalid JSON
    // There should be no calls to session or manager

    static constexpr const char* kEmpty = "";
    game.parse(kEmpty);

    static constexpr const char* kInvalid = "abc123";
    game.parse(kInvalid);
}

struct Join {
    string type;
    optional<string> code;
};

TEST_F(GameTest, RequestCreateLobby) {
    const string& code = "code";

    EXPECT_CALL(manager_, create)
        .WillOnce(Return(nullptr))
        .WillOnce(Return(lobby_));

    EXPECT_CALL(*lobby_, code).WillOnce(ReturnRef(code));

    {
        InSequence _;
        EXPECT_CALL(s1_, write(resp::join(false)));
        EXPECT_CALL(s1_, write(resp::join(true, code)));
    }

    const string& req = json::write(Join{.type = Prelobby.type.create});

    g1_.parse(req);
    EXPECT_EQ(g1_.state(), State::kPrelobby)
        << "Shouldn't advance state after manager returned nullptr";

    g1_.parse(req);
    EXPECT_EQ(g1_.state(), State::kCharacterSelect)
        << "Should advance state after manager returned a lobby";
}

TEST_F(GameTest, RequestJoinLobby) {
    const string& code = "code";

    EXPECT_CALL(manager_, join(code, testing::_))
        .WillOnce(Return(nullptr))
        .WillOnce(Return(lobby_));

    {
        InSequence _;
        EXPECT_CALL(s1_, write(resp::join(false)));
        EXPECT_CALL(s1_, write(resp::join(true, code)));
    }

    const string& req_missing_code =
        json::write(Join{.type = Prelobby.type.join});

    g1_.parse(req_missing_code);
    EXPECT_EQ(g1_.state(), State::kPrelobby)
        << "Should have returned early and without state change"
           " because missing join code";

    const string& req =
        json::write(Join{.type = Prelobby.type.join, .code = code});

    g1_.parse(req);
    EXPECT_EQ(g1_.state(), State::kPrelobby)
        << "Shouldn't advance state after manager returned nullptr";

    g1_.parse(req);
    EXPECT_EQ(g1_.state(), State::kCharacterSelect)
        << "Should advance state after manager returned a lobby";
}

}  // namespace io_blair
