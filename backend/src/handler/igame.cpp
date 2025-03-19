#include "igame.hpp"

namespace io_blair {
void IGameHandler::operator()(IGame&, SessionContext&, const json::in::LobbyCreate&) {}
void IGameHandler::operator()(IGame&, SessionContext&, const json::in::LobbyJoin&) {}
void IGameHandler::operator()(IGame&, SessionContext&, const json::in::LobbyLeave&) {}
void IGameHandler::operator()(IGame&, SessionContext&, const json::in::Chat&) {}
void IGameHandler::operator()(IGame&, SessionContext&, const json::in::CharacterHover&) {}
void IGameHandler::operator()(IGame&, SessionContext&, const json::in::CharacterConfirm&) {}
void IGameHandler::operator()(IGame&, SessionContext&, const json::in::CharacterMove&) {}
void IGameHandler::operator()(IGame&, SessionContext&, const json::in::CheckWin&) {}
void IGameHandler::operator()(IGame&, SessionContext&, const json::in::NewGame&) {}
void IGameHandler::operator()(IGame&, SessionContext&, SessionEvent) {}

}  // namespace io_blair
