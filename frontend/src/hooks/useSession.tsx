import { createContext, useContext, type SetStateAction } from "react";
import Maze, { type Coordinates } from "../lib/Maze";
import { type ConnectionValues } from "./useConnection";

export const SessionContext = createContext<SessionValue | undefined>(
  undefined,
);

export enum GameStatus {
  Prelobby,
  Lobby,
  InGame,
}

export interface SessionValue extends ConnectionValues {
  gameStatus: GameStatus;
  setGameStatus: React.Dispatch<SetStateAction<GameStatus>>;

  lobbyCode: string;
  setLobbyCode: React.Dispatch<SetStateAction<string>>;

  playerCount: number;
  setPlayerCount: React.Dispatch<SetStateAction<number>>;

  maze: Maze;
  startCoords: Coordinates;
}

export function useSession() {
  const context = useContext(SessionContext);
  if (context === undefined) {
    throw new Error("useSession must be used within a SessionProvider");
  }
  return context;
}
