#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "character.hpp"

namespace io_blair {

class ILobbyManager;
class ISession;

class ILobby {
   public:
    virtual bool join(ISession& session) = 0;

    virtual void msg_other(const ISession& session, std::string msg) = 0;

    virtual void confirm_character(const ISession& session,
                                   Character character) = 0;

    virtual void leave(const ISession& session) = 0;

    virtual bool full() const = 0;

    virtual const std::string& code() const = 0;
};

class Lobby : public ILobby, public std::enable_shared_from_this<Lobby> {
   public:
    explicit Lobby(std::string code, ILobbyManager& manager);

    bool join(ISession& session) override;

    void msg_other(const ISession& session, std::string msg) override;

    void confirm_character(const ISession& session,
                           Character character) override;

    void leave(const ISession& session) override;

    bool full() const override;

    const std::string& code() const override;

   private:
    const std::string code_;
    std::shared_ptr<ISession> p1_;
    std::shared_ptr<ISession> p2_;
    struct PlayerData {
        Character character = Character::kUnset;
    } d1_, d2_;

    mutable std::recursive_mutex mutex_;
    ILobbyManager& manager_;
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
