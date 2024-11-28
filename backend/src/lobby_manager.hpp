#pragma once

#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

#include "lobby.hpp"

namespace io_blair {
class Session;
class LobbyManager {
   public:
    static LobbyManager& get();

    std::shared_ptr<Lobby> create(std::shared_ptr<Session> p1);

    std::optional<std::shared_ptr<Lobby>> find(const std::string& code);

    void remove(const std::string& code);

   private:
    LobbyManager() = default;

    static std::string generate_code();

    std::unordered_map<std::string, std::shared_ptr<Lobby>> lobbies_;
    std::mutex mutex_;
};

}  // namespace io_blair
