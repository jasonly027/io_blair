/**
 * @file json.hpp
 */
#pragma once

#include <optional>
#include <rfl/json.hpp>
#include <string>

#include "character.hpp"

namespace io_blair {
class IHandler;
}

/**
 * @brief All possible JSON structs used in client-server interaction.
 */
namespace io_blair::json {

/**
 * @brief All possible JSON structs the client may send to the server.
 * 
 * @warning When adding new structs to this namespace,
 * make sure to add them to @ref AllJsonTypes.
 */
namespace in {

/**
 * @brief Indicates the client wants to create a new lobby.
 */
struct LobbyCreate {
  using Tag = rfl::Literal<"lobby_create">;
};

/**
 * @brief Indicates the client wants to join a lobby.
 */
struct LobbyJoin {
  using Tag = rfl::Literal<"lobby_join">;
  /**
   * @brief The code for the lobby the client wants to join.
   */
  std::string code;
};

/**
 * @brief Indicates the client wants to leave their lobby.
 */
struct LobbyLeave {
  using Tag = rfl::Literal<"lobby_leave">;
};

/**
 * @brief Indicates the client wants to send a message.
 */
struct Chat {
  using Tag = rfl::Literal<"chat">;
  /**
   * @brief The message they're brodcasting.
   */
  std::string msg;
};

/**
 * @brief Indicates the champion the client is hovering.
 */
struct CharacterHover {
  using Tag = rfl::Literal<"character_hover">;
  Character character;
};

/**
 * @brief Indicates the champion the client is confirming.
 */
struct CharacterConfirm {
  using Tag = rfl::Literal<"character_confirm">;
  Character character;
};

/**
 * @brief A union of all possible structs.
 */
using AllJsonTypes = rfl::TaggedUnion<"type", LobbyCreate, LobbyJoin>;

}  // namespace in

/**
 * @brief Decodes JSON into one of the objects in json::in and
 * passes into the handler.
 * 
 * @param data The JSON to parse. If \p data wasn't convertible to
 * one of the objects or wasn't valid JSON, the handler isn't called.
 * @param handler The handler that will receive the parsed object.
 */
void decode(const std::string& data, IHandler& handler);

/**
 * @brief All possible JSON structs the server may send to the client.
 */
namespace out {
// NOLINTBEGIN(readability-identifier-naming)

/**
 * @brief In response to the client trying to create/join a lobby.
 */
struct lobbyJoin {
  bool success;
  std::string_view code;
};

/**
 * @brief Encodes lobbyJoin as a string.
 * 
 * @param code The lobby code. Passing nullopt means lobby joining failed.
 * @return std::string
 */
std::string lobby_join(const std::optional<std::string_view>& code);

struct lobbyOtherJoin {};

std::string lobby_other_join();

/**
 * @brief Indicates the other session in the lobby has left.
 */
struct lobbyOtherLeave {};

/**
 * @brief Encodes lobbyOtherLeave as a string.
 * 
 * @return std::string 
 */
std::string lobby_other_leave();

//NOLINTEND(readability-identifier-naming)
}  // namespace out

}  // namespace io_blair::json
