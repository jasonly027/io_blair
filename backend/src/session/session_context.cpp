#include "session_context.hpp"

namespace io_blair {
void SessionContext::send(std::shared_ptr<const std::string> msg) const {
  if (auto sess = session.lock()) {
    sess->async_send(std::move(msg));
  }
}

void SessionContext::send(std::string msg) const {
  send(std::make_shared<const std::string>(std::move(msg)));
}

}  // namespace io_blair
