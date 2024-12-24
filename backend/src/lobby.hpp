#pragma once

#include <simdjson.h>

#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

#include "character.hpp"
#include "maze.hpp"
#include "net_common.hpp"

namespace io_blair {

class ISession;

class Player {
   public:
    void msg(std::string msg);

    void set_session(std::shared_ptr<ISession> session);

    void set_position(Maze::position position);

    bool connected() const;

    void reset();

    friend bool operator==(const Player& player, const ISession& session);

    Character character = Character::kUnset;

   private:
    std::shared_ptr<ISession> session_;
    Maze::position position_;
};

class ILobby {
   public:
    virtual ~ILobby() = default;

    enum class State : int8_t {
        /*
            Expecting incoming msgs of chat msgs,
            leaving, hovering/confirming a character
        */
        kCharacterSelect,
        /*
            Expecting incoming msgs of chat msgs,
            moving character, leaving lobby
        */
        kInGame,
        /*
            Expecting incoming msg of chat msgs,
            leaving, requesting new map
        */
        kGameFinished,
    };

    virtual void join(ISession& session) = 0;

    virtual void update(ISession& session, std::string data) = 0;

    virtual void leave(ISession& session) = 0;

    virtual bool full() const = 0;

    virtual bool empty() const = 0;

    virtual State state() const = 0;

    virtual const std::string& code() const = 0;
};

class ILobbyManager;

class Lobby : public ILobby, public std::enable_shared_from_this<Lobby> {
   public:
    Lobby(net::io_context& ctx, std::string code, ILobbyManager& manager);

    void join(ISession& session) override;

    void update(ISession& session, std::string data) override;

    void leave(ISession& session) override;

    bool full() const override;

    bool empty() const override;

    State state() const override;

    const std::string& code() const override;

   private:
    using document = simdjson::ondemand::document;

    std::optional<std::pair<Player&, Player&>> get_players(
        const ISession& session);

    std::optional<std::pair<const Player&, const Player&>> get_players(
        const ISession& session) const;

    void character_select(ISession& session, document& doc);

    void leave_impl(ISession& session, Player& self, Player& other);

    void generate_maze();

    void in_game(ISession& session, document& doc);

    ILobbyManager& manager_;
    net::strand<net::io_context::executor_type> strand_;
    simdjson::ondemand::parser parser_;
    const std::string code_;
    State state_;
    Player p1_;
    Player p2_;
    Maze maze_;
};

class ILobbyManager {
   public:
    virtual ~ILobbyManager() = default;

    virtual void create(ISession& session) = 0;

    virtual void join(const std::string& code, ISession& session) = 0;

    virtual void remove(const std::string& code) = 0;
};

class LobbyManager : public ILobbyManager {
   public:
    explicit LobbyManager(net::io_context& ctx);

    void create(ISession& session) override;

    void join(const std::string& code, ISession& session) override;

    void remove(const std::string& code) override;

   private:
    static std::string generate_code();

    net::io_context& ctx_;
    std::unordered_map<std::string, std::shared_ptr<Lobby>> lobbies_;
    std::mutex mutex_;
};
}  // namespace io_blair
