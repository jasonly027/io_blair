#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdint>
#include <game.hpp>
#include <limits>
#include <memory>
#include <optional>
#include <request.hpp>
#include <response.hpp>
#include <rfl/json.hpp>
#include <string>

#include "lobby_mock.hpp"
#include "session_mock.hpp"

using std::string, std::optional, std::shared_ptr, std::numeric_limits;
namespace json = rfl::json;
using testing::Return, testing::ReturnRef, testing::StrictMock,
    testing::InSequence;

namespace io_blair {

using request::Prelobby, request::CharacterSelect;
using State = Game::State;
namespace resp = response;

TEST(GameTest, RequestIsInvalidJson) {
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

class PrelobbyTest : public testing::Test {
   protected:
    MockLobbyManager manager_;
    shared_ptr<MockLobby> lobby_ = std::make_shared<MockLobby>();
    const string code_ = "code";

    MockSession s1_;
    Game g1_{s1_, manager_};

    MockSession s2_;
    Game g2_{s2_, manager_};

    void SetUp() override {
        ON_CALL(*lobby_, code).WillByDefault(ReturnRef(code_));

        EXPECT_EQ(g1_.state(), State::kPrelobby);
        EXPECT_EQ(g2_.state(), State::kPrelobby);
    }
};

struct Join {
    string type;
    optional<string> code;
};

TEST_F(PrelobbyTest, RequestCreateLobby) {
    EXPECT_CALL(manager_, create)
        .WillOnce(Return(nullptr))
        .WillOnce(Return(lobby_));

    {
        InSequence _;
        EXPECT_CALL(s1_, write(resp::join()));
        EXPECT_CALL(s1_, write(resp::join(code_)));
    }

    const string req = json::write(Join{.type = Prelobby.type.create});

    g1_.parse(req);
    EXPECT_EQ(g1_.state(), State::kPrelobby)
        << "Shouldn't advance state after manager returned nullptr";

    g1_.parse(req);
    EXPECT_EQ(g1_.state(), State::kCharacterSelect)
        << "Should advance state after manager returned a lobby";
}

TEST_F(PrelobbyTest, RequestJoinLobby) {
    EXPECT_CALL(manager_, join(code_, testing::_))
        .WillOnce(Return(nullptr))
        .WillOnce(Return(lobby_));

    {
        InSequence _;
        EXPECT_CALL(s1_, write(resp::join()));
        EXPECT_CALL(s1_, write(resp::join(code_)));
    }

    const string req_no_code = json::write(Join{.type = Prelobby.type.join});

    g1_.parse(req_no_code);
    EXPECT_EQ(g1_.state(), State::kPrelobby)
        << "Should have returned early and without state change"
           " because missing join code";

    const string req =
        json::write(Join{.type = Prelobby.type.join, .code = code_});

    g1_.parse(req);
    EXPECT_EQ(g1_.state(), State::kPrelobby)
        << "Shouldn't advance state after manager returned nullptr";

    g1_.parse(req);
    EXPECT_EQ(g1_.state(), State::kCharacterSelect)
        << "Should advance state after manager returned a lobby";
}

class CharacterSelectTest : public PrelobbyTest {
   protected:
    void SetUp() override {
        PrelobbyTest::SetUp();

        {
            InSequence _;
            EXPECT_CALL(manager_, create).WillOnce(Return(lobby_));
            EXPECT_CALL(manager_, join).WillOnce(Return(lobby_));
        }

        EXPECT_CALL(s1_, write);
        g1_.parse(json::write(Join{.type = Prelobby.type.create}));
        EXPECT_EQ(g1_.state(), State::kCharacterSelect);

        EXPECT_CALL(s2_, write);
        g2_.parse(json::write(Join{.type = Prelobby.type.join, .code = code_}));
        EXPECT_EQ(g2_.state(), State::kCharacterSelect);
    }
};

TEST_F(CharacterSelectTest, MsgOther) {
    const string msg = "Hello World";

    EXPECT_CALL(*lobby_, msg_other(testing::_, resp::msg(msg)));

    struct Msg {
        string type = "msg";
        optional<string> msg;
    };

    const string req_no_msg = json::write(Msg{});
    g1_.parse(req_no_msg);

    const string req = json::write(Msg{.msg = msg});
    g1_.parse(req);
}

TEST_F(CharacterSelectTest, Leave) {
    EXPECT_CALL(*lobby_, leave);

    struct Leave {
        string type = "leave";
    };

    const string req = json::write(Leave{});
    g1_.parse(req);
}

template <typename T>
struct Hover {
    string type = "hover";
    optional<T> hover;
};

TEST_F(CharacterSelectTest, Hover) {
    {
        InSequence _;
        EXPECT_CALL(*lobby_,
                    msg_other(testing::_, resp::hover(Character::kIO)));
        EXPECT_CALL(*lobby_,
                    msg_other(testing::_, resp::hover(Character::kBlair)));
    }

    // These should not trigger EXPECT_CALLS

    const string req_no_hover = json::write(Hover<string>{});
    g1_.parse(req_no_hover);

    const string req_str = json::write(Hover<string>{.hover = ""});
    g1_.parse(req_str);

    const string req_min64 =
        json::write(Hover<int64_t>{.hover = numeric_limits<int64_t>::min()});
    g1_.parse(req_min64);

    const string req_max64 =
        json::write(Hover<int64_t>{.hover = numeric_limits<int64_t>::max()});
    g1_.parse(req_max64);

    const string req_maxu64 =
        json::write(Hover<uint64_t>{.hover = numeric_limits<uint64_t>::max()});
    g1_.parse(req_maxu64);

    const string req_decimal = json::write(Hover<double>{.hover = 1.0});
    g1_.parse(req_decimal);

    // These should trigger EXPECT_CALLS

    const string req_io =
        json::write(Hover<CharacterImpl>{.hover = CharacterSelect.hover.io});
    g1_.parse(req_io);

    const string req_blair =
        json::write(Hover<CharacterImpl>{.hover = CharacterSelect.hover.blair});
    g1_.parse(req_blair);
}

}  // namespace io_blair
