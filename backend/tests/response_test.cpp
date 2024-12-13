#include "response.hpp"

#include <gtest/gtest.h>

namespace io_blair {
namespace resp = response;

TEST(Response, Join) {
    EXPECT_EQ(R"({"success":true,"code":"12345"})", resp::join(true, "12345"));
    EXPECT_EQ(R"({"success":false})", resp::join(false));
}

}  // namespace io_blair
