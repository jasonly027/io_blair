#include "ihandler.hpp"

namespace io_blair {

void IHandler::operator()(const json::in::LobbyCreate&) {}
void IHandler::operator()(const json::in::LobbyJoin&) {}

}  // namespace io_blair
