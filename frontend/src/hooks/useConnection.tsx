import { createContext, useContext } from "react";
import { SocketState } from "../lib/QueuedSocket";
import {
  type GameConnectionListener,
  type GameEventKey,
} from "../lib/GameConnection";
import type { GameCharacter } from "../types/character";
import type { Coordinate } from "../lib/Maze";

export const ConnectionContext = createContext<ConnectionValues | undefined>(
  undefined,
);

export interface ConnectionValues extends ConnectionActions {
  state: SocketState;
}

export interface ConnectionActions {
  addConnectionEventListener: <K extends GameEventKey>(
    eventName: K,
    listener: GameConnectionListener<K>,
  ) => void;

  removeConnectionEventListener: <K extends GameEventKey>(
    eventName: K,
    listener: GameConnectionListener<K>,
  ) => void;

  createLobby: () => void;

  joinLobby: (code: string) => void;

  message: (msg: string) => void;

  hoverCharacter: (character: GameCharacter) => void;

  confirmCharacter: (character: GameCharacter | null) => void;

  characterMove: (coordinate: Coordinate) => void;

  newGame: () => void;
}

/**
 * Returns the Connection context. Must be used at the top level of a component that
 * is a child of a ConnectionProvider.
 */
export default function useConnection() {
  const context = useContext(ConnectionContext);
  if (context === undefined) {
    throw new Error("useConnection must be used within a ConnectionProvider");
  }
  return context;
}
