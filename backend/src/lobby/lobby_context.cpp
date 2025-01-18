#include "lobby_context.hpp"

#include <mutex>


namespace io_blair {
using std::recursive_mutex;
using std::shared_ptr;
using std::string;
using std::string_view;
using std::weak_ptr;
using guard = std::lock_guard<std::recursive_mutex>;

LobbyContext::LobbyContext(string_view code, weak_ptr<ISession>& other, recursive_mutex& mutex)
    : code(code), mutex_(mutex), other_(other) {}

void LobbyContext::send_other(shared_ptr<const string> msg) {
  guard lock(mutex_);

  if (auto other = other_.lock()) {
    other->async_send(std::move(msg));
  }
}

void LobbyContext::send_other(string msg) {
  guard lock(mutex_);
  send_other(std::make_shared<const string>(std::move(msg)));
}

}  // namespace io_blair
