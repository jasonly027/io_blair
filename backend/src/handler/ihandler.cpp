#include "ihandler.hpp"

namespace io_blair {
void IHandler::operator()(const json::in::LobbyCreate&) {}
void IHandler::operator()(const json::in::LobbyJoin&) {}
void IHandler::operator()(const json::in::LobbyLeave&) {}

}  // namespace io_blair
