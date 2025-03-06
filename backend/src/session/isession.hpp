/**
 * @file isession.hpp
 */
#pragma once

#include <memory>
#include <string>

#include "event.hpp"


namespace io_blair {
/**
 * @brief An interface for sessions that communicate with a client.
 */
class ISession {
 public:
  virtual ~ISession() = default;

  /**
   * @brief Queues a message to be sent to the client and immediately return.
   * 
   * @param msg The message to send.
   */
  virtual void async_send(std::shared_ptr<const std::string> msg) = 0;

  /**
   * @brief Convenience method for async_send() that automatically wraps the string
   * in a shared_ptr.
   * 
   * @param msg The message to send.
   */
  virtual void async_send(std::string msg) = 0;

  /**
   * @brief Queues an event to be handled by the session.
   * 
   * @param ev The event to be handled.
   */
  virtual void async_handle(SessionEvent ev) = 0;
};

}  // namespace io_blair
