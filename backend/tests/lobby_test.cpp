#include "lobby.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <character.hpp>
#include <functional>
#include <limits>
#include <memory>
#include <optional>
#include <request.hpp>
#include <response.hpp>
#include <rfl/json.hpp>
#include <session.hpp>
#include <string>

#include "lobby_mock.hpp"
#include "matcher.hpp"
#include "session_mock.hpp"

namespace io_blair {

using std::shared_ptr, std::string, std::optional, std::ref,
    std::numeric_limits;
using testing::IsNull, testing::InSequence, testing::MockFunction,
    testing::SaveArg, testing::StrictMock, testing::ReturnPointee, testing::Eq;

using SessionState = ISession::State;
using LobbyState = ILobby::State;

namespace resp = response;
namespace req = request;
namespace json = rfl::json;

class LobbyTest : public testing::Test {
   protected:
    net::io_context ctx_;
    static string code_;
    MockLobbyManager manager_;

    shared_ptr<ILobby> lobby_ = std::make_shared<Lobby>(ctx_, code_, manager_);

    shared_ptr<MockSession> s1_ = std::make_shared<MockSession>();
    shared_ptr<MockSession> s2_ = std::make_shared<MockSession>();
    shared_ptr<MockSession> s3_ = std::make_shared<MockSession>();

    LobbyTest() {
        EXPECT_CALL(*s1_, get_shared()).WillRepeatedly(ReturnPointee(&s1_));
        EXPECT_CALL(*s2_, get_shared()).WillRepeatedly(ReturnPointee(&s2_));
        EXPECT_CALL(*s3_, get_shared()).WillRepeatedly(ReturnPointee(&s3_));
    }

    // Temporarily use this thread to do lobby strand work
    void process() {
        ctx_.run();
        ctx_.restart();
    }
};
string LobbyTest::code_ = "code";

TEST_F(LobbyTest, ThreeSessionsJoiningALobby) {
    MockFunction<void(string checkpoint)> check;

    {
        InSequence _;
        EXPECT_CALL(*s1_, set_state(SessionState::kWaitingForLobby));
        EXPECT_CALL(*s1_, set_lobby(SharedPtrEq(lobby_)));
        EXPECT_CALL(*s1_, set_state(SessionState::kInLobby));
        EXPECT_CALL(*s1_, write(resp::join(code_)));

        EXPECT_CALL(check, Call("1"));

        EXPECT_CALL(*s2_, set_state(SessionState::kWaitingForLobby));
        EXPECT_CALL(*s1_, write(resp::other_join()));
        EXPECT_CALL(*s2_, set_lobby(SharedPtrEq(lobby_)));
        EXPECT_CALL(*s2_, set_state(SessionState::kInLobby));
        EXPECT_CALL(*s2_, write(resp::join(code_)));

        EXPECT_CALL(check, Call("2"));

        EXPECT_CALL(*s3_, set_state(SessionState::kWaitingForLobby));
        EXPECT_CALL(*s3_, set_lobby(IsNull()));
        EXPECT_CALL(*s3_, set_state(SessionState::kPrelobby));
        EXPECT_CALL(*s3_, write(resp::join()));

        EXPECT_CALL(check, Call("3"));

        EXPECT_CALL(*s1_, set_lobby(IsNull()));
        EXPECT_CALL(*s1_, set_state(SessionState::kPrelobby));
        EXPECT_CALL(*s2_, write(resp::other_leave()));

        EXPECT_CALL(check, Call("4"));

        EXPECT_CALL(*s3_, set_state(SessionState::kWaitingForLobby));
        EXPECT_CALL(*s2_, write(resp::other_join()));
        EXPECT_CALL(*s3_, set_lobby(SharedPtrEq(lobby_)));
        EXPECT_CALL(*s3_, set_state(SessionState::kInLobby));
        EXPECT_CALL(*s3_, write(resp::join(code_)));
    }

    lobby_->join(*s1_);
    process();
    EXPECT_EQ(s1_.use_count(), 2);
    check.Call("1");

    lobby_->join(*s2_);
    process();
    EXPECT_EQ(s2_.use_count(), 2);
    check.Call("2");

    lobby_->join(*s3_);
    process();
    EXPECT_EQ(s3_.use_count(), 1);
    check.Call("3");

    lobby_->leave(*s1_);
    process();
    EXPECT_EQ(s1_.use_count(), 1);
    check.Call("4");

    lobby_->join(*s3_);
    process();
    EXPECT_EQ(s3_.use_count(), 2);
}

TEST_F(LobbyTest, Leave) {
    EXPECT_CALL(manager_, remove);

    lobby_->join(*s1_);
    lobby_->join(*s2_);
    process();

    lobby_->leave(*s1_);
    process();
    EXPECT_EQ(s1_.use_count(), 1);

    lobby_->leave(*s2_);
    process();
    EXPECT_EQ(s2_.use_count(), 1);

    EXPECT_EQ(lobby_.use_count(), 1);
}

TEST_F(LobbyTest, Full) {
    EXPECT_FALSE(lobby_->full());

    lobby_->join(*s1_);
    process();
    EXPECT_FALSE(lobby_->full());

    lobby_->join(*s2_);
    process();
    EXPECT_TRUE(lobby_->full());

    lobby_->leave(*s2_);
    process();
    EXPECT_FALSE(lobby_->full());
}

TEST_F(LobbyTest, Empty) {
    EXPECT_TRUE(lobby_->empty());

    lobby_->join(*s1_);
    process();
    EXPECT_FALSE(lobby_->empty());

    lobby_->join(*s2_);
    process();
    EXPECT_FALSE(lobby_->empty());

    lobby_->leave(*s1_);
    process();
    EXPECT_FALSE(lobby_->empty());

    lobby_->leave(*s2_);
    process();
    EXPECT_TRUE(lobby_->empty());
}

TEST(LobbyUpdate, InvalidJson) {
    net::io_context ctx;
    StrictMock<MockLobbyManager> manager;

    auto lobby = std::make_shared<Lobby>(ctx, "code", manager);
    auto session = std::make_shared<StrictMock<MockSession>>();

    {
        InSequence _;
        EXPECT_CALL(*session, set_state(SessionState::kWaitingForLobby));
        EXPECT_CALL(*session, get_shared())
            .WillRepeatedly(ReturnPointee(&session));
        EXPECT_CALL(*session, set_lobby(Eq(ref(lobby))));
        EXPECT_CALL(*session, set_state(SessionState::kInLobby));
        EXPECT_CALL(*session, write);
    }

    lobby->join(*session);

    static constexpr const char kEmpty[] = "";
    static constexpr const char kInvalid[] = "abc123";

    lobby->update(*session, kEmpty);
    lobby->update(*session, kInvalid);

    ctx.run();
}

class LobbyUpdateCharacterSelectTest : public testing::Test {
   protected:
    net::io_context ctx_;
    static string code_;
    MockLobbyManager manager_;

    shared_ptr<ILobby> lobby_ = std::make_shared<Lobby>(ctx_, code_, manager_);

    shared_ptr<MockSession> s1_ = std::make_shared<MockSession>();
    shared_ptr<MockSession> s2_ = std::make_shared<MockSession>();

    MockFunction<void(string checkpoint)> check_;

    void SetUp() override {
        EXPECT_CALL(*s1_, get_shared()).WillRepeatedly(ReturnPointee(&s1_));
        EXPECT_CALL(*s2_, get_shared()).WillRepeatedly(ReturnPointee(&s2_));

        // Expected when sessions join the lobby
        EXPECT_CALL(*s1_, set_state(SessionState::kWaitingForLobby));
        EXPECT_CALL(*s1_, set_lobby(SharedPtrEq(lobby_)));
        EXPECT_CALL(*s1_, set_state(SessionState::kInLobby));
        EXPECT_CALL(*s1_, write(resp::join(code_)));

        EXPECT_CALL(*s2_, set_state(SessionState::kWaitingForLobby));
        EXPECT_CALL(*s1_, write(resp::other_join()));
        EXPECT_CALL(*s2_, set_lobby(SharedPtrEq(lobby_)));
        EXPECT_CALL(*s2_, set_state(SessionState::kInLobby));
        EXPECT_CALL(*s2_, write(resp::join(code_)));
    }

    void process() {
        ctx_.run();
        ctx_.restart();
    }
};
string LobbyUpdateCharacterSelectTest::code_ = "code";

TEST_F(LobbyUpdateCharacterSelectTest, Msg) {
    const string msg = "Hello World";
    EXPECT_CALL(*s2_, write(resp::msg(msg)))
        .After(EXPECT_CALL(check_, Call("1")));

    lobby_->join(*s1_);
    lobby_->join(*s2_);

    struct Msg {
        string type = "msg";
        optional<string> msg;
    };

    const string req_no_msg = json::write(Msg{});
    lobby_->update(*s1_, req_no_msg);

    check_.Call("1");

    const string req = json::write(Msg{.msg = msg});
    lobby_->update(*s1_, req);

    process();
}

TEST_F(LobbyUpdateCharacterSelectTest, Leave) {
    EXPECT_CALL(*s1_, set_lobby(IsNull()));
    EXPECT_CALL(*s1_, set_state(SessionState::kPrelobby));
    EXPECT_CALL(*s2_, write(resp::other_leave()));

    lobby_->join(*s1_);
    lobby_->join(*s2_);

    struct Leave {
        string type = "leave";
    };

    const string req = json::write(Leave{});
    lobby_->update(*s1_, req);

    process();
}

template <typename T = void>
struct Hover {
    string type = "hover";
    optional<T> hover;
};

template <>
struct Hover<void> {
    string type = "hover";
};

TEST_F(LobbyUpdateCharacterSelectTest, Hover) {
    {
        InSequence _;
        EXPECT_CALL(check_, Call("1"));
        EXPECT_CALL(*s2_, write(resp::hover(Character::kIO)));
        EXPECT_CALL(*s2_, write(resp::hover(Character::kBlair)));
    }

    lobby_->join(*s1_);
    lobby_->join(*s2_);

    // These should not trigger EXPECT_CALLS

    const string req_no_hover = json::write(Hover{});
    lobby_->update(*s1_, req_no_hover);

    const string req_str = json::write(Hover<string>{.hover = ""});
    lobby_->update(*s1_, req_str);

    const string req_min64 =
        json::write(Hover<int64_t>{.hover = numeric_limits<int64_t>::min()});
    lobby_->update(*s1_, req_min64);

    const string req_max64 =
        json::write(Hover<int64_t>{.hover = numeric_limits<int64_t>::max()});
    lobby_->update(*s1_, req_max64);

    const string req_maxu64 =
        json::write(Hover<uint64_t>{.hover = numeric_limits<uint64_t>::max()});
    lobby_->update(*s1_, req_maxu64);

    const string req_decimal = json::write(Hover<double>{.hover = 1.0});
    lobby_->update(*s1_, req_decimal);

    const string req_unset = json::write(Hover<CharacterImpl>{
        .hover = static_cast<CharacterImpl>(Character::kUnset)});
    lobby_->update(*s1_, req_unset);

    // These should trigger EXPECT_CALLS

    check_.Call("1");

    const string req_io = json::write(
        Hover<CharacterImpl>{.hover = req::CharacterSelect.hover.io});
    lobby_->update(*s1_, req_io);

    const string req_blair = json::write(
        Hover<CharacterImpl>{.hover = req::CharacterSelect.hover.blair});
    lobby_->update(*s1_, req_blair);

    process();
}

template <typename T = void>
struct Confirm {
    string type = "confirm";
    optional<T> confirm;
};

template <>
struct Confirm<void> {
    string type = "confirm";
};

TEST_F(LobbyUpdateCharacterSelectTest, ConfirmCharacter) {
    {
        InSequence _;
        EXPECT_CALL(check_, Call("1"));

        EXPECT_CALL(*s2_, write(resp::confirm(Character::kUnset)));
        EXPECT_CALL(*s1_, write(resp::confirm(Character::kUnset)));

        EXPECT_CALL(check_, Call("2"));

        EXPECT_CALL(*s2_, write(resp::confirm(Character::kIO)));

        EXPECT_CALL(check_, Call("3"));

        EXPECT_CALL(*s2_, write(resp::confirm(Character::kBlair)));
        EXPECT_CALL(*s1_, write(resp::confirm(Character::kIO)));
        EXPECT_CALL(*s1_, write);
        EXPECT_CALL(*s2_, write);
    }

    lobby_->join(*s1_);
    lobby_->join(*s2_);

    // These should not trigger EXPECT_CALLS

    const string req_no_confirm = json::write(Confirm{});
    lobby_->update(*s1_, req_no_confirm);

    const string req_str = json::write(Confirm<string>{.confirm = ""});
    lobby_->update(*s1_, req_str);

    const string req_min64 = json::write(
        Confirm<int64_t>{.confirm = numeric_limits<int64_t>::min()});
    lobby_->update(*s1_, req_min64);

    const string req_max64 = json::write(
        Confirm<int64_t>{.confirm = numeric_limits<int64_t>::max()});
    lobby_->update(*s1_, req_max64);

    const string req_maxu64 = json::write(
        Confirm<uint64_t>{.confirm = numeric_limits<uint64_t>::max()});
    lobby_->update(*s1_, req_maxu64);

    const string req_decimal = json::write(Confirm<double>{.confirm = 1.0});
    lobby_->update(*s1_, req_decimal);

    process();
    check_.Call("1");

    // These should trigger EXPECT_CALLS

    const string req_unset = json::write(
        Confirm<CharacterImpl>{.confirm = req::CharacterSelect.confirm.unset});
    const string req_io = json::write(
        Confirm<CharacterImpl>{.confirm = req::CharacterSelect.confirm.io});
    const string req_blair = json::write(
        Confirm<CharacterImpl>{.confirm = req::CharacterSelect.confirm.blair});

    lobby_->update(*s1_, req_unset);
    lobby_->update(*s2_, req_unset);
    process();

    check_.Call("2");

    lobby_->update(*s1_, req_io);
    lobby_->update(*s2_, req_io);
    process();

    check_.Call("3");

    lobby_->update(*s1_, req_blair);
    lobby_->update(*s2_, req_io);
    process();
}

class LobbyManagerTest : public testing::Test {
   protected:
    net::io_context ctx_;
    LobbyManager manager_{ctx_};

    shared_ptr<MockSession> s1_ = std::make_shared<MockSession>();
    shared_ptr<MockSession> s2_ = std::make_shared<MockSession>();
    shared_ptr<MockSession> s3_ = std::make_shared<MockSession>();

    LobbyManagerTest() {
        EXPECT_CALL(*s1_, get_shared()).WillRepeatedly(ReturnPointee(&s1_));
        EXPECT_CALL(*s2_, get_shared()).WillRepeatedly(ReturnPointee(&s2_));
        EXPECT_CALL(*s3_, get_shared()).WillRepeatedly(ReturnPointee(&s3_));
    }

    // Temporarily use this thread to do lobby strand work
    void process() {
        ctx_.run();
        ctx_.restart();
    }
};

TEST_F(LobbyManagerTest, Create) {
    manager_.create(*s1_);

    EXPECT_EQ(s1_.use_count(), 2);
}

TEST_F(LobbyManagerTest, Join) {
    shared_ptr<ILobby> lobby;
    EXPECT_CALL(*s1_, set_lobby).WillOnce(SaveArg<0>(&lobby));

    manager_.create(*s1_);
    process();
    EXPECT_EQ(s1_.use_count(), 2);
    EXPECT_EQ(lobby.use_count(), 2);

    manager_.join("not the code", *s2_);
    process();
    EXPECT_EQ(s2_.use_count(), 1);
    EXPECT_EQ(lobby.use_count(), 2);

    manager_.join(lobby->code(), *s2_);
    process();
    EXPECT_EQ(s2_.use_count(), 2);
    EXPECT_EQ(lobby.use_count(), 2);

    manager_.join(lobby->code(), *s3_);
    process();
    EXPECT_EQ(s3_.use_count(), 1);
    EXPECT_EQ(lobby.use_count(), 2);
}

TEST_F(LobbyManagerTest, Remove) {
    shared_ptr<ILobby> lobby;
    EXPECT_CALL(*s1_, set_lobby).WillOnce(SaveArg<0>(&lobby));

    manager_.create(*s1_);
    process();
    EXPECT_EQ(lobby.use_count(), 2);

    manager_.remove(lobby->code());
    EXPECT_EQ(lobby.use_count(), 1);
}
}  // namespace io_blair
