#include "session_view.hpp"

namespace io_blair {
using guard = std::lock_guard<std::mutex>;

SessionView::SessionView(std::weak_ptr<ISession> session)
    : session_(std::move(session)) {}

void SessionView::async_send(std::shared_ptr<const std::string> msg) {
  guard lock(mutex_);
  if (auto sess = session_.lock()) {
    sess->async_send(std::move(msg));
  }
}

void SessionView::async_send(std::string msg) {
  guard lock(mutex_);
  if (auto sess = session_.lock()) {
    sess->async_send(std::move(msg));
  }
}

bool SessionView::try_set(std::weak_ptr<ISession> session) {
  guard lock(mutex_);

  if (session_.expired()) {
    session_ = std::move(session);
    return true;
  }
  return false;
}

void SessionView::reset() {
  guard lock(mutex_);
  session_.reset();
}

bool operator==(SessionView& lhs, const std::shared_ptr<ISession>& rhs) {
  guard lock(lhs.mutex_);

  if (lhs.session_.expired() || rhs == nullptr) {
    return false;
  }

  return lhs.session_.lock() == rhs;
}

}  // namespace io_blair
