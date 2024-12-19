#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>

#include "character.hpp"
#include "maze.hpp"

namespace io_blair {

class ILobbyManager;
class ISession;

class ILobby {
   public:
    virtual bool join(ISession& session) = 0;

    virtual void msg_all(std::string msg) = 0;

    virtual void msg_other(const ISession& session, std::string msg) = 0;

    virtual bool confirm_character(const ISession& session,
                                   Character character) = 0;

    virtual void leave(const ISession& session) = 0;

    virtual bool full() const = 0;

    virtual bool empty() const = 0;

    virtual const std::string& code() const = 0;
};

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

class Lobby : public ILobby, public std::enable_shared_from_this<Lobby> {
   public:
    Lobby(std::string code, ILobbyManager& manager);

    bool join(ISession& session) override;

    void msg_all(std::string msg) override;

    void msg_other(const ISession& session, std::string msg) override;

    bool confirm_character(const ISession& session,
                           Character character) override;

    void leave(const ISession& session) override;

    bool full() const override;

    bool empty() const override;

    const std::string& code() const override;

   private:
    std::pair<Player&, Player&> get_players(const ISession& session);

    std::pair<const Player&, const Player&> get_players(
        const ISession& session) const;

    void generate_maze();

    ILobbyManager& manager_;
    mutable std::recursive_mutex mutex_;
    const std::string code_;
    Player p1_;
    Player p2_;
    Maze maze_;
};

class ILobbyManager {
   public:
    virtual std::shared_ptr<ILobby> create(ISession& session) = 0;

    virtual std::shared_ptr<ILobby> join(const std::string& code,
                                         ISession& session) = 0;

    virtual void remove(const std::string& code) = 0;
};

class LobbyManager : public ILobbyManager {
   public:
    std::shared_ptr<ILobby> create(ISession& session) override;

    std::shared_ptr<ILobby> join(const std::string& code,
                                 ISession& session) override;

    void remove(const std::string& code) override;

   private:
    static std::string generate_code();

    std::unordered_map<std::string, std::shared_ptr<Lobby>> lobbies_;
    std::mutex mutex_;
};
}  // namespace io_blair
