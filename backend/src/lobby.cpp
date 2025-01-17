#include "lobby.hpp"

#include "context.hpp"
#include "session.hpp"

namespace io_blair {
LobbyContext& LobbyManager::create(std::weak_ptr<ISession> session) {
  LobbyContext a;
  return a;
}

std::optional<std::reference_wrapper<LobbyContext>> LobbyManager::join(
    std::weak_ptr<ISession> session, std::string_view code) {
  LobbyContext a;
  return a;
}
}  // namespace io_blair
