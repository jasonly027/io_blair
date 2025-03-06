/**
 * @file lobby_context.hpp
 */
#pragma once

#include <memory>
#include <string_view>

#include "isession.hpp"
#include "isession_controller.hpp"

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
  ISession& other;

  std::unique_ptr<ISessionController> controller;
};

}  // namespace io_blair
