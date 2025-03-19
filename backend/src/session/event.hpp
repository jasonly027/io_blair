/**
 * @file event.hpp
 */
#pragma once

namespace io_blair {
/**
 * @brief Special events that are passed through an ISession to be handled.
 * These aren't received from the client, they come from other parts of the server.
 */
enum class SessionEvent {
  /**
   * @brief Indicates the session is closing.
   */
  kCloseSession,
  /**
   * @brief Indicates Lobby should transition to InGame.
   */
  kTransitionToInGame,
  /**
   * @brief Indicates Lobby should transition to CharacterSelect.
   */
  kTransitionToCharacterSelect,
  /**
   * @brief Indicates Lobby should transition to GameDone.
   */
  kTransitionToGameDone,
};

}  // namespace io_blair
