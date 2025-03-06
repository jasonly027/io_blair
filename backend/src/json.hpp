/**
 * @file json.hpp
 */
#pragma once

#include <optional>
#include <rfl/json.hpp>
#include <string>
#include <string_view>

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
  using Tag = rfl::Literal<"lobbyCreate">;
};

/**
 * @brief Indicates the client wants to join a lobby.
 */
struct LobbyJoin {
  using Tag = rfl::Literal<"lobbyJoin">;
  /**
   * @brief The code for the lobby the client wants to join.
   */
  std::string code;
};

/**
 * @brief Indicates the client wants to leave their lobby.
 */
struct LobbyLeave {
  using Tag = rfl::Literal<"lobbyLeave">;
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
  using Tag = rfl::Literal<"characterHover">;
  Character character;
};

/**
 * @brief Indicates the champion the client is confirming.
 */
struct CharacterConfirm {
  using Tag = rfl::Literal<"characterConfirm">;
  Character character;
};

/**
 * @brief A union of all possible structs.
 */
using AllJsonTypes = rfl::TaggedUnion<"type", LobbyCreate, LobbyJoin, LobbyLeave, Chat,
                                      CharacterHover, CharacterConfirm>;

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

/**
 * @brief Indicates another session has joined the lobby.
 */
struct lobbyOtherJoin {};

/**
 * @brief Encodes lobbyOtherJoin as a string.
 *
 * @return std::string 
 */
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

/**
 * @brief Contains a message for the other client.
 */
struct chat {
  std::string_view msg;
};

/**
 * @brief Encodes chat as a string.
 * 
 * @param msg The chat message to send.
 * @return std::string 
 */
std::string chat_msg(std::string_view msg);

/**
 * @brief Indicates the character hovered by the other client.
 */
struct characterHover {
  Character character;
};

/**
 * @brief Encodes characterHover as a string.
 * 
 * @param character The character hovered.
 * @return std::string 
 */
std::string character_hover(Character character);

/**
 * @brief Indicates the character confirmed by the other client.
 */
struct characterConfirm {
  std::optional<Character> character;
};

/**
 * @brief Encodes characterConfirm as a string.
 * 
 * @param character The character confirmed.
 * @return std::string 
 */
std::string character_confirm(Character character);

/**
 * @brief Indicates transition to in-game state.
 */
struct transitionToInGame {};

std::string transition_to_ingame();

//NOLINTEND(readability-identifier-naming)
}  // namespace out

}  // namespace io_blair::json
