#pragma once

#include <boost/json.hpp>
#include <memory>
#include <optional>
#include <string>

#include "boost/beast/core/string_type.hpp"
#include "boost/json/object.hpp"
#include "lobby_manager.hpp"
#include "net_common.hpp"

namespace io_blair {
    namespace json = boost::json;
class Game {
   public:
    enum class State { kOutsideLobby, kCharacterSelect, kInGame, kGameDone };

    explicit Game(const std::shared_ptr<LobbyManager>& manager);

    std::optional<std::shared_ptr<std::string>> parse(beast::string_view sv);

    static void log_err(error_code ec, const char* what);

   private:
    std::optional<json::object> parse_json(beast::string_view sv);

    std::optional<std::shared_ptr<std::string>> parse_outside_lobby(const json::object& obj);

    std::shared_ptr<LobbyManager> manager_;
    State state_;
    json::parser parser_;
};
}  // namespace io_blair
