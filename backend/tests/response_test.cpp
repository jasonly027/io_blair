#include "response.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <string>

using namespace std::string_literals;

namespace io_blair {
namespace resp = response;

TEST(Response, MakeCreateLobby) {
    const std::string code = "ABC123";
    const auto resp = resp::create_lobby(code);

    EXPECT_EQ(R"({ "code" : "ABC123" })"s, *resp);
    EXPECT_NE(R"({ "code" : "" })", *resp);
}

TEST(Response, MakeJoin) {
    EXPECT_EQ(R"({ "join" : "success" })", *resp::join(true));
    EXPECT_EQ(R"({ "join" : "fail" })", *resp::join(false));
}

}  // namespace io_blair
