#pragma once

#include <memory>
#include <mutex>
#include <optional>
#include <string>

#include "character.hpp"
#include "ilobby_controller.hpp"
#include "isession.hpp"
#include "lobby_context.hpp"
#include "player.hpp"


namespace io_blair {
/**
 * @brief A thread-safe ILobbyController.
 */
class LobbyController : public ILobbyController {
 public:
  /**
   * @brief Construct a new Lobby Controller object.
   * 
   * @param code The lobby's join code.
   */
  explicit LobbyController(std::string code);

  std::optional<LobbyContext> join(std::weak_ptr<ISession> session) override;

  void leave(const std::weak_ptr<ISession>& session) override;

  bool empty() const override;

  void set_character(Player& self, Player& other, Character character) override;

  /**
   * @brief The lobby's join code.
   */
  const std::string code_;

 private:
  mutable std::mutex mutex_;

  Player p1_;
  Player p2_;
};

}  // namespace io_blair
