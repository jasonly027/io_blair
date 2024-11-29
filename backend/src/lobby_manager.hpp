#pragma once

#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

namespace io_blair {
template <typename Lobby>
class BasicLobbyManager {
   public:
    static BasicLobbyManager& get();

    std::shared_ptr<Lobby> create();

    std::optional<std::shared_ptr<Lobby>> find(const std::string& code);

    void remove(const std::string& code);

   private:
    BasicLobbyManager() = default;

    static std::string generate_code();

    std::unordered_map<std::string, std::shared_ptr<Lobby>> lobbies_;
    std::mutex mutex_;
};

}  // namespace io_blair
