#include "game.hpp"

#include <boost/json.hpp>
#include <iostream>
#include <memory>
#include <optional>

#include "boost/json/object.hpp"
#include "net_common.hpp"
#include "response.hpp"

using std::optional, std::nullopt, std::shared_ptr, std::string;

namespace io_blair {
Game::Game(const std::shared_ptr<LobbyManager>& manager)
    : manager_(manager), state_(State::kOutsideLobby) {}

optional<shared_ptr<string>> Game::parse(beast::string_view sv) {
    optional<json::object> obj = parse_json(sv);
    if (!obj.has_value()) return std::make_shared<string>(response::kError);

    switch (state_) {
        case State::kOutsideLobby:
            return parse_outside_lobby(*obj);
        case State::kCharacterSelect:
        case State::kInGame:
        case State::kGameDone:
            break;
    }
    return {};
}

void Game::log_err(error_code ec, const char* what) {
    std::cerr << what << ": " << ec.what() << '\n';
}

std::optional<json::object> Game::parse_json(beast::string_view sv) {
    parser_.reset();

    try {
        error_code ec;
        parser_.write(sv, ec);
        if (ec) {
            log_err(ec, "parser_write");
            return nullopt;
        }
        return parser_.release().as_object();
    } catch (const boost::system::system_error& e) {
        std::cerr << e.what();
        return nullopt;
    }
}

optional<shared_ptr<string>> Game::parse_outside_lobby(
    const json::object& obj) {
    return nullopt;
}

}  // namespace io_blair
