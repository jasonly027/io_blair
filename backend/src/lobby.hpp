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
    virtual bool join(std::shared_ptr<ISession> ptr) = 0;

    virtual void msg(const ISession& session, std::string msg) = 0;

    virtual bool confirm_character(const ISession& session,
                                   Character character) = 0;

    virtual void leave(const ISession& session) = 0;

    virtual bool full() const = 0;

    virtual const std::string& code() const = 0;
};

class Lobby : public ILobby, public std::enable_shared_from_this<Lobby> {
   public:
    explicit Lobby(std::string code, ILobbyManager& manager);

    bool join(std::shared_ptr<ISession> ptr) override;

    void msg(const ISession& session, std::string msg) override;

    bool confirm_character(const ISession& session,
                           Character character) override;

    void leave(const ISession& session) override;

    bool full() const override;

    const std::string& code() const override;

   private:
    const std::string code_;
    std::shared_ptr<ISession> p1_;
    std::shared_ptr<ISession> p2_;
    mutable std::mutex mutex_;
    ILobbyManager& manager_;
};

class ILobbyManager {
   public:
    virtual std::shared_ptr<ILobby> create(
        std::shared_ptr<ISession> session) = 0;

    virtual std::shared_ptr<ILobby> join(const std::string& code,
                                         std::shared_ptr<ISession> session) = 0;

    virtual void remove(const std::string& code) = 0;
};

class LobbyManager : public ILobbyManager {
   public:
    std::shared_ptr<ILobby> create(std::shared_ptr<ISession> session) override;

    std::shared_ptr<ILobby> join(const std::string& code,
                                 std::shared_ptr<ISession> session) override;

    void remove(const std::string& code) override;

   private:
    static std::string generate_code();

    std::unordered_map<std::string, std::shared_ptr<Lobby>> lobbies_;
    std::mutex mutex_;
};
}  // namespace io_blair
