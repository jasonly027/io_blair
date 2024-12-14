#include "response.hpp"

#include <gtest/gtest.h>

namespace io_blair {
namespace resp = response;

TEST(Response, Join) {
    EXPECT_EQ(R"({"type":"join","success":true,"code":"12345"})",
              resp::join(true, "12345"));
    EXPECT_EQ(R"({"type":"join","success":false})", resp::join(false));
}

TEST(Response, Msg) {
    EXPECT_EQ(R"({"type":"msg","msg":"Hello World"})",
              resp::msg("Hello World"));
}

}  // namespace io_blair
