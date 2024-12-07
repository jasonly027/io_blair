#pragma once

#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

namespace io_blair {

class LobbyManager;

class ISession;

class Lobby : public std::enable_shared_from_this<Lobby> {
   public:
    explicit Lobby(std::string code, LobbyManager& manager);

    bool join(std::shared_ptr<ISession> ptr);

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
    std::shared_ptr<Lobby> create();

    std::optional<std::shared_ptr<Lobby>> find(const std::string& code);

    void remove(const std::string& code);

   private:
    static std::string generate_code();

    std::unordered_map<std::string, std::shared_ptr<Lobby>> lobbies_;
    std::mutex mutex_;
};
}  // namespace io_blair
