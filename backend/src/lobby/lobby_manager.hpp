/**
 * @file lobby_manager.hpp
 */
#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

#include "ilobby_manager.hpp"
#include "isession.hpp"
#include "lobby_context.hpp"
#include "lobby_controller.hpp"
#include "string_hash.hpp"

namespace io_blair {
class LobbyManager : public ILobbyManager {
 public:
  LobbyContext create(std::weak_ptr<ISession> session) override;

  std::optional<LobbyContext> join(std::weak_ptr<ISession> session, std::string_view code) override;

  void leave(const std::weak_ptr<ISession>& session, std::string_view code) override;

 private:
  std::string generate_code();

  std::recursive_mutex mutex_;
  std::unordered_map<std::string, LobbyController, StringHash, std::equal_to<>> lobbies_;
};

}  // namespace io_blair
