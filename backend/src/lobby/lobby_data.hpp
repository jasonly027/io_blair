#pragma once

#include <memory>
#include <mutex>
#include <optional>
#include <string>

#include "isession.hpp"
#include "lobby_context.hpp"

namespace io_blair {
class LobbyData {
 public:
  explicit LobbyData(std::string code);

  std::optional<LobbyContext> join(std::weak_ptr<ISession>);

  const std::string code_;

 private:
  std::recursive_mutex mu1_;
  std::weak_ptr<ISession> p1_;

  std::recursive_mutex mu2_;
  std::weak_ptr<ISession> p2_;
};

}  // namespace io_blair
