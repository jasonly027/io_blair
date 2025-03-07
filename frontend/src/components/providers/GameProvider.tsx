import { useMemo, useState, type ReactNode } from "react";
import Maze, { type Coordinates, cellFromNumber } from "../../lib/Maze";
import {
  GameStatus,
  GameContext,
  type GameValue as GameValues,
} from "../../hooks/useGame";
import usePlayers from "../../hooks/usePlayers";

/**
 * A provider for useGame.
 *
 * If gameStatus is not Lobby or InGame:
 * - lobbyCode will be "00000000"
 * - playerCount will be 1
 *
 * If gameStatus is not InGame:
 * - maze will be a 3x3
 * - startCoords will be [0, 0]
 */
export function GameProvider({ children }: { children?: ReactNode }) {
  console.log("GameProvider render");

  const [gameStatus, setGameStatus] = useState(GameStatus.Prelobby);

  const [lobbyCode, setLobbyCode] = useState("00000000");

  const [playerCount, setPlayerCount] = useState(1);

  const { you, setYou, teammate } = usePlayers();

  const [maze] = useState<Maze>(getDefaultMaze);
  const [startCoords] = useState<Coordinates>([0, 0]);

  const gameValues: GameValues = useMemo(
    () => ({
      gameStatus,
      setGameStatus,

      lobbyCode,
      setLobbyCode,

      playerCount,
      setPlayerCount,

      you,
      setYou,

      teammate,

      maze,
      startCoords,
    }),
    [
      gameStatus,
      lobbyCode,
      playerCount,
      you,
      setYou,
      teammate,
      maze,
      startCoords,
    ],
  );

  return (
    <GameContext.Provider value={gameValues}>{children}</GameContext.Provider>
  );
}

function getDefaultMaze(): Maze {
  return new Maze(
    [
      [0b0011, 0b1111, 0b1101],
      [0b0110, 0b1101, 0b0101],
      [0b0011, 0b1011, 0b1001],
    ].map((row) => row.map(cellFromNumber)),
  );
}
