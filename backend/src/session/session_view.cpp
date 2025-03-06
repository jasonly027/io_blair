#include "session_view.hpp"

#include "event.hpp"


namespace io_blair {
using std::shared_ptr;
using std::string;
using std::weak_ptr;

using guard = std::lock_guard<std::mutex>;

SessionView::SessionView(weak_ptr<ISession> session)
    : session_(std::move(session)) {}

void SessionView::async_send(shared_ptr<const string> msg) {
  guard lock(mutex_);
  if (auto sess = session_.lock()) {
    sess->async_send(std::move(msg));
  }
}

void SessionView::async_send(string msg) {
  guard lock(mutex_);
  if (auto sess = session_.lock()) {
    sess->async_send(std::move(msg));
  }
}

void SessionView::async_handle(SessionEvent ev) {
  guard lock(mutex_);
  if (auto sess = session_.lock()) {
    sess->async_handle(ev);
  }
}

bool SessionView::try_set(weak_ptr<ISession> session) {
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

bool SessionView::expired() const {
  guard lock(mutex_);
  return session_.expired();
}

bool operator==(const SessionView& lhs, const shared_ptr<ISession>& rhs) {
  guard lock(lhs.mutex_);

  if (lhs.session_.expired() || rhs == nullptr) {
    return false;
  }

  return lhs.session_.lock() == rhs;
}

}  // namespace io_blair
