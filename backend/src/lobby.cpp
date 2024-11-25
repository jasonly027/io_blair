#include "lobby.hpp"

#include "session.hpp"

using std::string;

namespace io_blair {
Lobby::Lobby(std::string code, std::shared_ptr<Session> p1,
             std::shared_ptr<Session> p2)
    : code_(std::move(code)), p1_(std::move(p1)), p2_(std::move(p2)) {}
}  // namespace io_blair
