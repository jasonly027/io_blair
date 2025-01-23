#include "igame.hpp"

namespace io_blair {
void IGameHandler::operator()(IGame&, SessionContext&, const json::in::LobbyCreate&) {}
void IGameHandler::operator()(IGame&, SessionContext&, const json::in::LobbyJoin&) {}
void IGameHandler::operator()(IGame&, SessionContext&, const json::in::LobbyLeave&) {}
void IGameHandler::operator()(IGame&, SessionContext&, const json::in::Chat&) {}

}  // namespace io_blair
