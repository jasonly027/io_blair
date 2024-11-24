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
    enum class State { kPrelobby, kCharacterSelect, kInGame, kGameDone };

    explicit Game(Session& session);

    void parse(std::string data);

    static void log_err(error_code ec, const char* what);

   private:
    void parse_prelobby(json::document& doc);

    Session& session_;
    State state_;
    json_parser parser_;
};
}  // namespace io_blair
