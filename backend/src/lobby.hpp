#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string_view>

namespace io_blair {

class ISession;
class LobbyContext;

class ILobbyManager {
 public:
  virtual ~ILobbyManager()                                      = default;
  virtual LobbyContext& create(std::weak_ptr<ISession> session) = 0;
  virtual std::optional<std::reference_wrapper<LobbyContext>> join(std::weak_ptr<ISession> session,
                                                                   std::string_view code)
      = 0;
};

class LobbyManager : public ILobbyManager {
 public:
  LobbyContext& create(std::weak_ptr<ISession> session) override;

  std::optional<std::reference_wrapper<LobbyContext>> join(std::weak_ptr<ISession> session,
                                                           std::string_view code) override;

 private:
};

}  // namespace io_blair
