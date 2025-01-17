#include "context.hpp"

namespace io_blair {
void SessionContext::send(std::shared_ptr<const std::string> msg) {
  session->async_send(std::move(msg));
}

void SessionContext::send(std::string msg) {
  send(std::make_shared<const std::string>(std::move(msg)));
}
}  // namespace io_blair
