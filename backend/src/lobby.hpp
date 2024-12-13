#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace io_blair {

class LobbyManager;

class ISession;

class Lobby : public std::enable_shared_from_this<Lobby> {
   public:
    explicit Lobby(std::string code, LobbyManager& manager);

    bool join(std::shared_ptr<ISession> ptr);

    void msg(const ISession* session, std::string msg);

    void leave(const ISession* session);

    bool full() const;

    const std::string code_;

   private:
    std::shared_ptr<ISession> p1_;
    std::shared_ptr<ISession> p2_;
    mutable std::mutex mutex_;
    LobbyManager& manager_;
};

class LobbyManager {
   public:
    std::shared_ptr<Lobby> create(std::shared_ptr<ISession> session);

    std::shared_ptr<Lobby> join(const std::string& code, std::shared_ptr<ISession> session);

    void remove(const std::string& code);

   private:
    static std::string generate_code();

    std::unordered_map<std::string, std::shared_ptr<Lobby>> lobbies_;
    std::mutex mutex_;
};
}  // namespace io_blair
