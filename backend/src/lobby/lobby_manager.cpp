#include "lobby_manager.hpp"

#include <algorithm>
#include <cstring>
#include <random>


namespace io_blair {
using std::mt19937;
using std::nullopt;
using std::optional;
using std::random_device;
using std::string;
using std::string_view;
using std::strlen;
using std::uniform_int_distribution;
using std::weak_ptr;
using guard = std::lock_guard<std::recursive_mutex>;

LobbyContext LobbyManager::create(weak_ptr<ISession> session) {
  guard lock(mutex_);

  string code = generate_code();

  auto [it, _] = lobbies_.try_emplace(code, code);
  return *it->second.join(std::move(session));
}

optional<LobbyContext> LobbyManager::join(weak_ptr<ISession> session, string_view code) {
  guard lock(mutex_);

  if (const auto it = lobbies_.find(code); it != lobbies_.end()) {
    return it->second.join(std::move(session));
  }
  return nullopt;
}


void LobbyManager::leave(const std::weak_ptr<ISession>& session, std::string_view code) {
  guard lock(mutex_);

  if (const auto it = lobbies_.find(code); it != lobbies_.end()) {
    it->second.leave(session);

    if (it->second.empty()) {
      lobbies_.erase(it);
    }
  }
}

string LobbyManager::generate_code() {
  static constexpr int kCodeLength          = 6;
  static constexpr const char kCharacters[] = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  thread_local mt19937 gen{random_device{}()};
  thread_local uniform_int_distribution<string::size_type> pick(0, strlen(kCharacters) - 1);

  // Keep generating until the code is unique.
  string res(kCodeLength, '#');
  do {
    std::generate_n(res.begin(), res.size(), [&] { return kCharacters[pick(gen)]; });
  } while (lobbies_.contains(res));

  return res;
}
}  // namespace io_blair
