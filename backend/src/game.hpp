#pragma once

#include <simdjson.h>

#include <cstdint>
#include <memory>
#include <string>

#include "net_common.hpp"

namespace io_blair {

class ISession;
class ILobbyManager;
class ILobby;

class Game {
   public:
    // Game states
    enum class State : uint8_t {
        /*
            Before joining a lobby. Expecting incoming msg
            to request to create/join a lobby
        */
        kPrelobby,
        /*
            In a lobby, Expecting incoming msgs of sending chat msgs,
            leaving, hovering/confirming a character
        */
        kCharacterSelect,
        /*
            In game, Expecting incoming msgs of moving or
            leaving lobby
        */
        kInGame,
        /*
            Finished game, Expecting incoming msg of self
            leaving or requesting new map
        */
        kGameDone
    };

    explicit Game(ISession& session, ILobbyManager& manager);

    // Print to stderr
    static void log_err(error_code ec, const char* what);

    // Parse and act on incoming message
    void parse(std::string data);

    void leave();

    State state() const;

   private:
    using parser = simdjson::ondemand::parser;
    using document = simdjson::ondemand::document;

    void write(std::string msg);

    void msg_other(std::string msg);

    void prelobby(document& doc);

    void character_select(document& doc);

    void msg(document& doc);

    // Reference to session that owns this game state
    ISession& session_;
    ILobbyManager& manager_;
    std::shared_ptr<ILobby> lobby_;
    // Current game state to understand what msgs to expect
    State state_;
    parser parser_;
};
}  // namespace io_blair
