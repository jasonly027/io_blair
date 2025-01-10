#include "response.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <character.hpp>


namespace io_blair {

using testing::StartsWith;

namespace resp = response;

TEST(Response, Join) {
  EXPECT_EQ(R"({"type":"join","code":"12345"})", resp::join("12345"));
  EXPECT_EQ(R"({"type":"join"})", resp::join());
}

TEST(Response, Msg) {
  EXPECT_EQ(R"({"type":"msg","msg":"Hello World"})", resp::msg("Hello World"));
}

TEST(Response, Hover) {
  EXPECT_EQ(R"({"type":"hover","hover":0})", resp::hover(Character::kUnset));
  EXPECT_EQ(R"({"type":"hover","hover":1})", resp::hover(Character::kIO));
  EXPECT_EQ(R"({"type":"hover","hover":2})", resp::hover(Character::kBlair));
}

TEST(Response, OtherJoin) {
  EXPECT_EQ(R"({"type":"otherJoin"})", resp::other_join());
}

TEST(Response, OtherLeave) {
  EXPECT_EQ(R"({"type":"otherLeave"})", resp::other_leave());
}

TEST(Response, Confirm) {
  EXPECT_EQ(R"({"type":"confirm","confirm":0})", resp::confirm(Character::kUnset));
  EXPECT_EQ(R"({"type":"confirm","confirm":1})", resp::confirm(Character::kIO));
  EXPECT_EQ(R"({"type":"confirm","confirm":2})", resp::confirm(Character::kBlair));
}

TEST(Response, Maze) {
  Maze::position start = {0, 0};
  Maze::position end   = {1, 1};
  EXPECT_THAT(resp::maze(start, end, {}),
              StartsWith(R"({"type":"maze","start":[0,0],"end":[1,1],"maze":[[)"));
}

TEST(Response, MoveSelfAndOther) {
  Maze::position pos = {0, 0};
  EXPECT_EQ(R"({"type":"move","who":"self","pos":[0,0]})", resp::move_self(pos));
  EXPECT_EQ(R"({"type":"move","who":"other","pos":[0,0]})", resp::move_other(pos));
}

TEST(Response, Win) {
  EXPECT_EQ(R"({"type":"win"})", resp::win());
}

TEST(Response, Restart) {
  EXPECT_EQ(R"({"type":"restart"})", resp::restart());
}


}  // namespace io_blair
