#pragma once

#include <simdjson.h>

#include <string>

namespace io_blair {

class ISession;
class ILobbyManager;
class ILobby;

class Game {
   public:
    Game(ISession& session, ILobbyManager& manager);

    // Parse and act on incoming message
    void update(std::string data);

   private:
    // Reference to session that owns this game state
    ISession& session_;
    ILobbyManager& manager_;
    simdjson::ondemand::parser parser_;
};
}  // namespace io_blair
