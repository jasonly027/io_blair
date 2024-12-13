#include <gtest/gtest.h>

#include <game.hpp>
#include <optional>
#include <rfl/json.hpp>

#include "gmock/gmock.h"
#include "response.hpp"
#include "session_mock.hpp"

using std::string, std::optional;
namespace json = rfl::json;
using testing::Return, testing::StrictMock;

namespace io_blair::request {
struct Join {
    rfl::Rename<"lobbyType", string> lobby_type;
    optional<string> code;
};
}  // namespace io_blair::request

namespace io_blair {

using request::Join, request::kPrelobby;
using State = Game::State;

class GameTest : public testing::Test {
   protected:
    void SetUp() override { ASSERT_EQ(game_.state(), State::kPrelobby); }

    MockSession session_;
    Game game_{session_};
};

TEST_F(GameTest, RequestIsInvalidJson) {
    StrictMock<MockSession> session;
    Game game(session);

    // Game should return immediately on invalid JSON
    // There should be no calls to session

    static constexpr const char* kEmpty = "";
    game.parse(kEmpty);

    static constexpr const char* kInvalid = "abc123";
    game.parse(kInvalid);
}

TEST_F(GameTest, RequestCreateLobby) {
    const string& req = json::write(Join{.lobby_type = kPrelobby.type_create});

    EXPECT_CALL(session_, join_new_lobby)
        .WillOnce(Return(false))
        .WillOnce(Return(true));

    EXPECT_CALL(session_, write).Times(2);

    ON_CALL(session_, code).WillByDefault(Return(""));

    game_.parse(req);
    EXPECT_EQ(game_.state(), State::kPrelobby)
        << "Shouldn't advance state after join_new_lobby() returned false";

    game_.parse(req);
    EXPECT_EQ(game_.state(), State::kCharacterSelect)
        << "Should advance state after join_new_lobby() returned true";
}

TEST_F(GameTest, RequestJoinLobby) {
    const string& code = "code";

    const string& req =
        json::write(Join{.lobby_type = kPrelobby.type_join, .code = code});
    const string& req_missing_code =
        json::write(Join{.lobby_type = kPrelobby.type_join});

    EXPECT_CALL(session_, join_lobby(code))
        .WillOnce(Return(false))
        .WillOnce(Return(true));

    EXPECT_CALL(session_, write).Times(2);

    game_.parse(req_missing_code);
    EXPECT_EQ(game_.state(), State::kPrelobby)
        << "Should have returned early and without state change"
           " because missing join code";

    game_.parse(req);
    EXPECT_EQ(game_.state(), State::kPrelobby)
        << "Shouldn't advance state after join_lobby() returned false";

    game_.parse(req);
    EXPECT_EQ(game_.state(), State::kCharacterSelect)
        << "Should advance state after join_lobby() returned true";
}

}  // namespace io_blair
