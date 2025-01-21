#include "igame.hpp"

namespace io_blair {
void IGameHandler::operator()(IGame&, SessionContext&, const json::in::LobbyCreate&) {}
void IGameHandler::operator()(IGame&, SessionContext&, const json::in::LobbyJoin&) {}
void IGameHandler::operator()(IGame&, SessionContext&, const json::in::LobbyLeave&) {}

}  // namespace io_blair
