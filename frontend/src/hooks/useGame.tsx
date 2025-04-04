import { createContext, useContext, type DispatchWithoutAction } from "react";
import Maze, { type Coordinate } from "../lib/Maze";
import type { ChangePlayerAction, Player } from "./usePlayers";

export const GameContext = createContext<GameValue | undefined>(undefined);

/** The current status of the game */
export enum GameStatus {
  Prelobby,
  Lobby,
  InGame,
  GameDone,
}

/** Values available in the Game context */
export interface GameValue {
  gameStatus: GameStatus;

  lobbyCode: string;

  playerCount: number;

  you: Player;
  setYou: (action: ChangePlayerAction) => boolean;

  teammate: Player;

  map: Maze;
  currentCoins: number;
  totalCoins: number;

  youCoord: Readonly<Coordinate>;
  teammateCoord: Readonly<Coordinate>;

  movesCount: number;
  incrementMovesCount: DispatchWithoutAction;

  gameDone: boolean
}

/**
 * Returns the Game context. Must be used at the top level of a component that
 * is a child of a GameProvider.
 */
export default function useGame() {
  const context = useContext(GameContext);
  if (context === undefined) {
    throw new Error("useGame must be used within a GameProvider");
  }
  return context;
}
