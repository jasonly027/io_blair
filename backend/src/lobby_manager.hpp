#pragma once

#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

#include "lobby.hpp"

namespace io_blair {

class ISession;

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
