/**
 * @file session_view.hpp
 */
#pragma once

#include <memory>
#include <mutex>

#include "event.hpp"
#include "isession.hpp"


namespace io_blair {
/**
 * @brief A thread-safe view of an ISession. SessionView holds a weak_ptr to
 * an actual ISession. Overrides safely abort if the weak_ptr has expired.
 */
class SessionView : public ISession {
 public:
  /**
   * @brief Construct a new Session View object.
   * 
   * @param session The session to view. If no session is passed, view
   * is considered expired.
   */
  explicit SessionView(std::weak_ptr<ISession> session = {});

  void async_send(std::shared_ptr<const std::string> msg) override;

  void async_send(std::string msg) override;

  void async_handle(SessionEvent ev) override;

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
   * @brief Determines whether the underlying weak_ptr is expired.
   * 
   * @return true The session is expired.
   * @return false The session isn't expired.
   */
  bool expired() const;

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
  friend bool operator==(const SessionView& lhs, const std::shared_ptr<ISession>& rhs);

 private:
  std::weak_ptr<ISession> session_;
  mutable std::mutex mutex_;
};

}  // namespace io_blair
