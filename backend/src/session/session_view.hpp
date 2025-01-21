/**
 * @file session_view.hpp
 */
#pragma once

#include <memory>
#include <mutex>

#include "isession.hpp"


namespace io_blair {
/**
 * @brief A thread-safe view of an ISession. SessionView holds a weak_ptr to
 * an actual ISession. Sending a message to an expired ISession is safely aborted.
 */
class SessionView : public ISession {
 public:
  /**
   * @brief Construct a new Session View object.
   * 
   * @param session The session to view.
   */
  explicit SessionView(std::weak_ptr<ISession> session = {});

  /**
   * @brief Queues a message to be sent to the client and immediately return.
   * If the ISession has expired, nothing occurs.
   * 
   * @param msg The message to send.
   */
  void async_send(std::shared_ptr<const std::string> msg) override;

  /**
   * @brief Convenience method for async_send() that automatically wraps the string
   * in a shared_ptr. If the ISession has expired, nothing occurs.
   * 
   * @param msg The message to send.
   */
  void async_send(std::string msg) override;

  /**
   * @brief Attempt to reassign an expired session with a new session.
   * 
   * @param session The new session to view.
   * @return true. SessionView now views \p session.
   * @return false. The currently viewed session isn't expired, no view change occurs.
   */
  bool try_set(std::weak_ptr<ISession> session);

  /**
   * @brief Resets the underlying ISession weak_ptr.
   */
  void reset();

  /**
   * @brief Checks whether the SessionView is a view of the same ISession
   * pointed to by the shared_ptr.
   *
   * @note If the SessionView is expired or the
   * shared_ptr is null, false is returned.
   * 
   * @param lhs. The view of an ISession.
   * @param rhs. The pointer to an ISession.
   * @return bool 
   */
  friend bool operator==(SessionView& lhs, const std::shared_ptr<ISession>& rhs);

 private:
  std::weak_ptr<ISession> session_;
  std::mutex mutex_;
};

}  // namespace io_blair
