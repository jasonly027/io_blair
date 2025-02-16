import { createContext, useContext, useState, type ReactNode } from "react";
import Maze, { type Coordinates, cellFromNumber } from "../lib/Maze";
import useSocket, { type SocketValues } from "./useSocket";

const SessionContext = createContext<SessionValue | undefined>(undefined);

export enum GameStatus {
  Prelobby,
  Lobby,
  InGame,
}

export interface SessionValue extends SocketValues {
  gameStatus: GameStatus;
  maze: Maze;
  startCoords: Coordinates;
}

export function SessionProvider({ children }: { children?: ReactNode }) {
  console.log("SessionProvider render");

  const [gameStatus] = useState(GameStatus.Prelobby);

  const [maze] = useState<Maze>(
    new Maze(
      [
        [0b0011, 0b1111, 0b1101],
        [0b0110, 0b1101, 0b0101],
        [0b0011, 0b1011, 0b1001],
      ].map((row) => row.map(cellFromNumber)),
    ),
  );

  const [startCoords] = useState<Coordinates>([0, 0]);

  const socketValues = useSocket();

  const sessionValue: SessionValue = {
    gameStatus,
    maze,
    startCoords,
    ...socketValues,
  };

  return (
    <SessionContext.Provider value={sessionValue}>
      {children}
    </SessionContext.Provider>
  );
}

export function useSession() {
  const context = useContext(SessionContext);
  if (context === undefined) {
    throw new Error("useSession must be used within a SessionProvider");
  }
  return context;
}
