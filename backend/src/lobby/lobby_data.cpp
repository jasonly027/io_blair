#include "lobby_data.hpp"

#include <memory>
#include <optional>
#include <string_view>
#include <utility>

#include "isession.hpp"
#include "json.hpp"
#include "lobby_context.hpp"


namespace io_blair {
using std::nullopt;
using std::optional;
using std::string;
using std::string_view;
using std::weak_ptr;
namespace jout = json::out;

LobbyData::LobbyData(string code)
    : code_(std::move(code)) {}

optional<LobbyContext> LobbyData::join(weak_ptr<ISession> session) {
  if (p1_.try_set(session)) {
    p2_.async_send(jout::lobby_other_join());
    return LobbyContext{code_, p2_};
  }

  if (p2_.try_set(std::move(session))) {
    p1_.async_send(jout::lobby_other_join());
    return LobbyContext{code_, p1_};
  }

  return nullopt;
}

void LobbyData::leave(const weak_ptr<ISession>& session) {
  auto sess = session.lock();

  if (sess == p1_) {
    p1_.reset();
    p2_.async_send(jout::lobby_other_leave());
  } else if (sess == p2_) {
    p2_.reset();
    p1_.async_send(jout::lobby_other_leave());
  }
}

}  // namespace io_blair
