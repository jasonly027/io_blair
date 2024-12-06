#pragma once

#include <memory>
#include <string>

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
    LobbyManager& manager_;
};
}  // namespace io_blair
