import {
  createContext,
  useContext,
  useMemo,
  useState,
  type ReactNode,
  type SetStateAction,
} from "react";
import Maze, { type Coordinates, cellFromNumber } from "../lib/Maze";
import useConnection, { type ConnectionValues } from "./useConnection";

const SessionContext = createContext<SessionValue | undefined>(undefined);

export enum GameStatus {
  Prelobby,
  Lobby,
  InGame,
}

export interface SessionValue extends ConnectionValues {
  gameStatus: GameStatus;
  setGameStatus: React.Dispatch<SetStateAction<GameStatus>>;
  maze: Maze;
  startCoords: Coordinates;
}

export function SessionProvider({ children }: { children?: ReactNode }) {
  console.log("SessionProvider render");

  const [gameStatus, setGameStatus] = useState(GameStatus.Prelobby);

  const [maze] = useState<Maze>(getDefaultMaze);

  const [startCoords] = useState<Coordinates>([0, 0]);

  const connection = useConnection();

  const sessionValue: SessionValue = useMemo(
    () => ({
      gameStatus,
      setGameStatus,
      maze,
      startCoords,
      ...connection,
    }),
    [gameStatus, maze, startCoords, connection],
  );

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

function getDefaultMaze(): Maze {
  return new Maze(
    [
      [0b0011, 0b1111, 0b1101],
      [0b0110, 0b1101, 0b0101],
      [0b0011, 0b1011, 0b1001],
    ].map((row) => row.map(cellFromNumber)),
  );
}
