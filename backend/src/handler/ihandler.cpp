#include "ihandler.hpp"

namespace io_blair {
void IHandler::operator()(const json::in::LobbyCreate&) {}
void IHandler::operator()(const json::in::LobbyJoin&) {}
void IHandler::operator()(const json::in::LobbyLeave&) {}
void IHandler::operator()(const json::in::Chat&) {}
void IHandler::operator()(const json::in::CharacterHover&) {}
void IHandler::operator()(const json::in::CharacterConfirm&) {}
void IHandler::operator()(const json::in::CharacterMove&) {}
void IHandler::operator()(SessionEvent) {}

}  // namespace io_blair
