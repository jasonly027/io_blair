#include "igame.hpp"

namespace io_blair {
void IGameHandler::operator()(IGame&, SessionContext&, const json::in::LobbyCreate&) {}
void IGameHandler::operator()(IGame&, SessionContext&, const json::in::LobbyJoin&) {}

}  // namespace io_blair
