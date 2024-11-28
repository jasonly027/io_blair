#pragma once

#include <simdjson.h>

#include <string>

#include "net_common.hpp"

namespace io_blair {

namespace json = simdjson::ondemand;

using json_parser = json::parser;

class Session;

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
            joining/leaving, changing desired character
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

    explicit Game(Session& session);

    // Print to stderr
    static void log_err(error_code ec, const char* what);

    // Parse and act on incoming message
    void parse(std::string data);

   private:
    void write(const std::shared_ptr<std::string>& str);

    void parse_prelobby(json::document& doc);

    void create_lobby();

    void join_lobby(json::document& doc);

    // Reference to session that owns this game state
    Session& session_;
    // Current game state to understand what msgs to expect
    State state_;
    json_parser parser_;
};
}  // namespace io_blair
