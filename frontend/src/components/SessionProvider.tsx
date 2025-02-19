import { useMemo, useState, type ReactNode } from "react";
import Maze, { type Coordinates, cellFromNumber } from "../lib/Maze";
import useConnection from "../hooks/useConnection";
import {
  GameStatus,
  SessionContext,
  type SessionValue,
} from "../hooks/useSession";

export function SessionProvider({ children }: { children?: ReactNode }) {
  console.log("SessionProvider render");

  const [gameStatus, setGameStatus] = useState(GameStatus.Lobby);

  const [lobbyCode, setLobbyCode] = useState("00000000");

  const [playerCount, setPlayerCount] = useState(1);

  const [maze] = useState<Maze>(getDefaultMaze);

  const [startCoords] = useState<Coordinates>([0, 0]);

  const connection = useConnection();

  const sessionValue: SessionValue = useMemo(
    () => ({
      gameStatus,
      setGameStatus,

      lobbyCode,
      setLobbyCode,

      playerCount,
      setPlayerCount,

      maze,
      startCoords,
      ...connection,
    }),
    [gameStatus, lobbyCode, playerCount, maze, startCoords, connection],
  );

  return (
    <SessionContext.Provider value={sessionValue}>
      {children}
    </SessionContext.Provider>
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
