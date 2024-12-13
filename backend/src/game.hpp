#pragma once

#include <simdjson.h>

#include <string>

#include "net_common.hpp"

namespace io_blair {

class ISession;

class Game {
   public:
    // Game states
    enum class State {
        /*
            Before joining a lobby. Expecting incoming msg
            to request to create/join a lobby
        */
        kPrelobby,
        /*
            In a lobby, Expecting incoming msgs of self/other
            joining/leaving, changing/confirming desired character
        */
        kCharacterSelect,
        /*
            In game, Expecting incoming msgs of self/other
            movement, self/other leaving
        */
        kInGame,
        /*
            Finished game, Expecting incoming msg of self/other
            leaving, requesting new map
        */
        kGameDone
    };

    explicit Game(ISession& session);

    // Print to stderr
    static void log_err(error_code ec, const char* what);

    // Parse and act on incoming message
    void parse(std::string data);

    State state() const;

   private:
    using parser = simdjson::ondemand::parser;
    using document = simdjson::ondemand::document;

    void write(std::string msg);

    void parse_prelobby(document& doc);

    void create_lobby();

    void join_lobby(document& doc);

    // Reference to session that owns this game state
    ISession& session_;
    // Current game state to understand what msgs to expect
    State state_;
    parser parser_;
};
}  // namespace io_blair
