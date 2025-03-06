/**
 * @file session_context.hpp
 */
#pragma once

#include <memory>

#include "ilobby_manager.hpp"
#include "isession.hpp"


namespace io_blair {
/**
 * @brief Context related to a Session.
 */
struct SessionContext {
  std::weak_ptr<ISession> session;
  ILobbyManager& lobby_manager;
};

}  // namespace io_blair
