#include "lobby_manager.hpp"

#include <random>

#include "lobby.hpp"
#include "session.hpp"

using lock = std::lock_guard<std::mutex>;
using std::shared_ptr, std::string;

namespace io_blair {
shared_ptr<Lobby> LobbyManager::create(shared_ptr<Session> p1) {
    lock guard(mutex_);

    string code = generate_code();
    while (lobbies_.contains(code)) {
        code = generate_code();
    }

    auto lobby = std::make_shared<Lobby>(code, std::move(p1));
    lobbies_[code] = lobby;

    return lobby;
}

string LobbyManager::generate_code() {
    static constexpr auto kCodeLength = 6;

    static constexpr auto kCharacters = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    static std::mt19937 gen{std::random_device{}()};
    static std::uniform_int_distribution<std::string::size_type> pick(
        0, std::strlen(kCharacters) - 1);

    string res(kCodeLength, '\0');
    std::generate_n(res.begin(), kCodeLength,
                    [&] { return kCharacters[pick(gen)]; });

    return res;
}
}  // namespace io_blair
