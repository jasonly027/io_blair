/**
 * @file json.hpp
 */
#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <rfl/json.hpp>
#include <string>
#include <string_view>

#include "character.hpp"
#include "lobby/lobby_controller.hpp"
#include "maze.hpp"

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
 * @brief Indicates the coordinate the client wants to move to.
 */
struct CharacterMove {
  using Tag = rfl::Literal<"characterMove">;
  std::array<int, 2> coordinate;
};

/**
 * @brief Indicates the client wants to know if the game is finished.
 */
struct CheckWin {
  using Tag = rfl::Literal<"checkWin">;
};

struct NewGame {
  using Tag = rfl::Literal<"newGame">;
};

/**
 * @brief A union of all possible structs.
 */
using AllJsonTypes
    = rfl::TaggedUnion<"type", LobbyCreate, LobbyJoin, LobbyLeave, Chat, CharacterHover,
                       CharacterConfirm, CharacterMove, CheckWin, NewGame>;

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
  int player_count;
  Character other_confirm;
};

/**
 * @brief Encodes lobbyJoin as a string.
 * 
 * @param code The lobby code. Passing nullopt means lobby joining failed.
 * @return std::string
 */
std::string lobby_join(const std::optional<std::string_view>& code, std::optional<int> player_count,
                       std::optional<Character> other_confirm);

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

/**
 * @brief Encodes transitionToInGame as a string.
 * 
 * @return std::string 
 */
std::string transition_to_ingame();

/**
 * @brief Contains the serialized maze and start/end coordinates.
 */
struct inGameMaze {
  using M = LobbyController::Maze;

  std::array<std::array<int16_t, M::cols()>, M::rows()> maze;
  coordinate_arr start;
  coordinate_arr end;
  /**
   * @brief The possible paths from the other client's perspective
   * for the start position.
   */
  int16_t cell;
};

/**
 * @brief Encodes inGameMaze as a string.
 * 
 * @param maze
 * @param character The character to serialize maze for.
 * @return std::string 
 */
std::string ingame_maze(LobbyController::Maze maze, Character self, Character other);

enum class Direction { up, right, down, left };

/**
 * @brief Provides the client with possible paths
 * from the other client's perspective, OR whether
 * the client needs to reset.
 */
struct characterMove {
  // [0, 0] if reset is true
  coordinate_arr coordinate;
  // 0 if reset is true
  int16_t cell;
  // The client moved to a bad position and needs to
  // go to the start
  bool reset;
};

/**
 * @brief Encodes characterMove as a string.
 * 
 * @param coordinate 
 * @param cell 
 * @return std::string 
 */
std::string character_move(coordinate coordinate, int16_t cell);

/**
 * @brief Indicates the client needs to go back to
 * the start of the maze.
 * 
 * @return std::string 
 */
std::string character_reset();

/**
 * @brief Indicates where the other client has moved.
 */
struct characterOtherMove {
  /**
   * @brief The direction they moved.
   */
  Direction direction;
  /**
   * @brief Whether they moved to their death.
   */
  bool reset;
};

/**
 * @brief Encodes characterOtherMove as a string.
 * 
 * @param direction
 * @param reset 
 * @return std::string 
 */
std::string character_other_move(Direction direction, bool reset);

/**
 * @brief Indicates a coin has been taken.
 */
struct coinTaken {
  /**
   * @brief The coordinate of the coin taken.
   */
  coordinate_arr coordinate;
};

/**
 * @brief Encodes coinTaken as a string.
 * 
 * @param coordinate The coordinate of the coin taken.
 * @return std::string 
 */
std::string coin_taken(coordinate coordinate);

/**
 * @brief Indicates the game has finished.
 */
struct inGameWin {};

/**
 * @brief Encodes inGameWin as a string.
 *
 * @return std::string 
 */
std::string ingame_win();

//NOLINTEND(readability-identifier-naming)
}  // namespace out

}  // namespace io_blair::json
