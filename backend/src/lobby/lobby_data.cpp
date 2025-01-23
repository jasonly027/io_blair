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
  if (auto opt = join(session, p1_, p2_)) {
    return std::move(opt);
  }
  return join(std::move(session), p2_, p1_);
}

optional<LobbyContext> LobbyData::join(weak_ptr<ISession> session, SessionView& a, SessionView& b) {
  if (a.try_set(std::move(session))) {
    b.async_send(jout::lobby_other_join());
    return LobbyContext{code_, b};
  }
  return nullopt;
}

void LobbyData::leave(const weak_ptr<ISession>& session) {
  if (leave(session, p1_, p2_)) {
    return;
  }
  leave(session, p2_, p1_);
}

bool LobbyData::leave(const weak_ptr<ISession>& session, SessionView& a, SessionView& b) {
  if (session.lock() == a) {
    a.reset();
    b.async_send(jout::lobby_other_leave());
    return true;
  }
  return false;
}
}  // namespace io_blair
