#include <gtest/gtest.h>

#include <iostream>
#include <json.hpp>


namespace io_blair {
TEST(Decoder, decode) {
  std::cout << json::out::lobby_join("fljasdlkjfs");

  FAIL();
}
}  // namespace io_blair
