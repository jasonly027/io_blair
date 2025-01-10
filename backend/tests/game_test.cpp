#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <game.hpp>
#include <optional>
#include <request.hpp>
#include <response.hpp>
#include <rfl/json.hpp>
#include <string>

#include "lobby.hpp"
#include "lobby_mock.hpp"
#include "session.hpp"
#include "session_mock.hpp"

namespace io_blair {

using request::Prelobby;
using std::string, std::optional;
using testing::Return, testing::Ref, testing::StrictMock, testing::InSequence;
using SessionState = ISession::State;
using LobbyState   = Lobby::State;

namespace json = rfl::json;

TEST(GameTest, RequestIsInvalidJson) {
  StrictMock<MockSession> session;

  {
    InSequence _;

    EXPECT_CALL(session, state).Times(2).WillRepeatedly(Return(SessionState::kPrelobby));

    EXPECT_CALL(session, state).Times(2).WillRepeatedly(Return(SessionState::kWaitingForLobby));

    EXPECT_CALL(session, state).WillOnce(Return(SessionState::kInLobby));
    EXPECT_CALL(session, try_lobby_update);

    EXPECT_CALL(session, state).WillOnce(Return(SessionState::kInLobby));
    EXPECT_CALL(session, try_lobby_update);
  }

  StrictMock<MockLobbyManager> manager;
  Game game(session, manager);

  static constexpr const char kEmpty[]   = "";
  static constexpr const char kInvalid[] = "abc123";

  game.update(kEmpty);
  game.update(kInvalid);

  game.update(kEmpty);
  game.update(kInvalid);

  game.update(kEmpty);
  game.update(kInvalid);
}

TEST(GameTest, PreLobbyRequests) {
  MockSession session;
  MockLobbyManager manager;

  const string code = "code";

  {
    InSequence _;
    EXPECT_CALL(session, state).WillOnce(Return(SessionState::kPrelobby));
    EXPECT_CALL(manager, create(Ref(session)));

    EXPECT_CALL(session, state).WillOnce(Return(SessionState::kPrelobby));

    EXPECT_CALL(session, state).WillOnce(Return(SessionState::kPrelobby));
    EXPECT_CALL(manager, join(code, Ref(session)));
  }

  Game game = {session, manager};

  struct Join {
    string type;
    optional<string> code;
  };

  const string r_create = json::write(Join{.type = Prelobby.type.create});
  game.update(r_create);

  const string r_join_no_code = json::write(Join{.type = Prelobby.type.join});
  game.update(r_join_no_code);

  const string r_join = json::write(Join{.type = Prelobby.type.join, .code = code});
  game.update(r_join);
}

}  // namespace io_blair
