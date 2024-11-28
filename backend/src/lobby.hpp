#pragma once

#include <memory>
#include <string>

namespace io_blair {
class Session;
class Lobby {
   public:
    explicit Lobby(std::string code, std::shared_ptr<Session> p1 = nullptr,
                   std::shared_ptr<Session> p2 = nullptr);

    void leave(Session* session);

    const std::string code_;

   private:
    std::shared_ptr<Session> p1_;
    std::shared_ptr<Session> p2_;
};
}  // namespace io_blair
