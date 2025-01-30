#include "ilobby.hpp"

namespace io_blair {
void ILobbyHandler::operator()(ILobby&, SessionContext&, LobbyContext&,
                               const json::in::CharacterHover&) {}
void ILobbyHandler::operator()(ILobby&, SessionContext&, LobbyContext&,
                               const json::in::CharacterConfirm&) {}

}  // namespace io_blair
