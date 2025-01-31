#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "handler.hpp"
#include "json.hpp"
#include "lobby_context.hpp"
#include "mock/mock_game.hpp"
#include "mock/mock_lobby_manager.hpp"
#include "mock/mock_session.hpp"
#include "session_context.hpp"
#include "session_view.hpp"


namespace io_blair::testing {
using ::std::make_shared;
using ::std::shared_ptr;
using ::std::string;
using ::testing::StrictMock;
namespace jin  = json::in;
namespace jout = json::out;

class LobbyShould : public ::testing::Test {
 protected:
  StrictMock<MockGame> game_;

  shared_ptr<StrictMock<MockSession>> sess_ = make_shared<StrictMock<MockSession>>();
  MockLobbyManager manager_;
  SessionContext sess_ctx_{sess_, manager_};

  const string code_                         = "arbitrary";
  shared_ptr<StrictMock<MockSession>> other_ = make_shared<StrictMock<MockSession>>();
  SessionView view_{other_};
  LobbyContext lob_ctx_{code_, view_};
};

TEST_F(LobbyShould, TransitionOnLeave) {
  Lobby lobby(std::move(lob_ctx_));

  EXPECT_CALL(manager_, leave);
  EXPECT_CALL(game_, transition_to);

  lobby(game_, sess_ctx_, jin::LobbyLeave{});
}

TEST_F(LobbyShould, SendOnMsg) {
  Lobby lobby(std::move(lob_ctx_));
  const string msg = "arbitrary";

  EXPECT_CALL(*other_, async_send(jout::chat_msg(msg)));

  lobby(game_, sess_ctx_, jin::Chat{msg});
}


}  // namespace io_blair::testing
