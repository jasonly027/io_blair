/**
 * @file lobby_context.hpp
 */
#pragma once

#include <string_view>

#include "session_view.hpp"

namespace io_blair {
/**
 * @brief Context related to a lobby.
 */
struct LobbyContext {
  /**
   * @brief The lobby's join code.
   */
  const std::string_view code;
  /**
   * @brief A reference to the other session in the lobby.
   */
  SessionView& other;
};

}  // namespace io_blair
