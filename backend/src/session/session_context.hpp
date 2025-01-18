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
  /**
   * @brief Convenience method for sending a message to the session.
   * If the weak_ptr to session has expired, the message isn't sent.
   * 
   * @param msg The message to send.
   */
  void send(std::shared_ptr<const std::string> msg) const;

  /**
   * @brief Convenience method for send() that wraps \p msg
   * in a shared_ptr automatically.
   * 
   * @param msg The message to send.
   */
  void send(std::string msg) const;

  std::weak_ptr<ISession> session;
  ILobbyManager& lobby_manager;
};

}  // namespace io_blair
