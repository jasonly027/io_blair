#include "lobby_data.hpp"

#include <optional>
#include <string_view>
#include <utility>

#include "lobby_context.hpp"

namespace io_blair {
using std::nullopt;
using std::optional;
using std::string;
using std::string_view;
using std::weak_ptr;
using guard = std::lock_guard<std::recursive_mutex>;

LobbyData::LobbyData(string code)
    : code_(std::move(code)) {}

optional<LobbyContext> LobbyData::join(weak_ptr<ISession> session) {
  {
    guard lock(mu1_);

    if (p1_.expired()) {
      p1_ = std::move(session);
      return LobbyContext(code_, p2_, mu1_);
    }
  }

  {
    guard lock(mu2_);

    if (p2_.expired()) {
      p1_ = std::move(session);
      return LobbyContext(code_, p1_, mu2_);
    }
  }

  return nullopt;
}
}  // namespace io_blair
