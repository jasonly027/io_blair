#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <optional>
#include <string>
#include <utility>

#include "handler.hpp"
#include "json.hpp"
#include "lobby_context.hpp"
#include "mock/mock_game.hpp"
#include "mock/mock_lobby_manager.hpp"
#include "mock/mock_session.hpp"
#include "session_context.hpp"
#include "session_view.hpp"


namespace io_blair::testing {
using std::make_shared;
using std::nullopt;
using std::shared_ptr;
using std::string;
using ::testing::Return;
using ::testing::StrictMock;
namespace jin  = json::in;
namespace jout = json::out;

class PrelobbyShould : public ::testing::Test {
 protected:
  StrictMock<MockGame> game_;

  shared_ptr<StrictMock<MockSession>> sess_ = make_shared<StrictMock<MockSession>>();
  MockLobbyManager manager_;
  SessionContext sess_ctx_{sess_, manager_};

  const string code_ = "arbitrary";
  SessionView view_;
  LobbyContext lob_ctx_{code_, view_};
};

TEST_F(PrelobbyShould, CreateLobby) {
  Prelobby prelobby;

  EXPECT_CALL(manager_, create).WillOnce(Return(std::move(lob_ctx_)));
  EXPECT_CALL(game_, transition_to);

  prelobby(game_, sess_ctx_, jin::LobbyCreate{});
}

TEST_F(PrelobbyShould, TransitionOnLobbyCtx) {
  Prelobby prelobby;

  EXPECT_CALL(manager_, join).WillOnce(Return(std::move(lob_ctx_)));
  EXPECT_CALL(game_, transition_to);

  prelobby(game_, sess_ctx_, jin::LobbyJoin{code_});
}

TEST_F(PrelobbyShould, NotTransitionOnNulloptLobbyCtx) {
  Prelobby prelobby;

  EXPECT_CALL(manager_, join).WillOnce(Return(nullopt));

  prelobby(game_, sess_ctx_, jin::LobbyJoin{code_});
}

}  // namespace io_blair::testing
