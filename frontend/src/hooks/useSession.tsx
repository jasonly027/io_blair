import { createContext, useContext, type SetStateAction } from "react";
import Maze, { type Coordinates } from "../lib/Maze";
import { type ConnectionValues } from "./useConnection";
import type { GameCharacter } from "../types/character";

export const SessionContext = createContext<SessionValue | undefined>(
  undefined,
);

/** The current status of the game */
export enum GameStatus {
  Prelobby,
  Lobby,
  InGame,
}

/** Values in available in the Session context */
export interface SessionValue extends ConnectionValues {
  gameStatus: GameStatus;
  setGameStatus: React.Dispatch<SetStateAction<GameStatus>>;

  lobbyCode: string;
  setLobbyCode: React.Dispatch<SetStateAction<string>>;

  playerCount: number;
  setPlayerCount: React.Dispatch<SetStateAction<number>>;

  you: GameCharacter | null;
  setYou: React.Dispatch<SetStateAction<GameCharacter | null>>;

  teammate: GameCharacter | null;
  setTeammate: React.Dispatch<SetStateAction<GameCharacter | null>>;

  maze: Maze;
  startCoords: Coordinates;
}

/**
 * Returns the Session context. Must be used at the top level of a component that
 * is a child of a SessionProvider.
 */
export function useSession() {
  const context = useContext(SessionContext);
  if (context === undefined) {
    throw new Error("useSession must be used within a SessionProvider");
  }
  return context;
}
