import { useEffect, useMemo, useRef, useState, type ReactNode } from "react";
import Maze, { type Coordinates, cellFromNumber } from "../../lib/Maze";
import {
  GameStatus,
  GameContext,
  type GameValue as GameValues,
} from "../../hooks/useGame";
import usePlayers from "../../hooks/usePlayers";
import useConnection from "../../hooks/useConnection";
import type { GameConnectionListener } from "../../lib/GameConnection";

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

  const gameStatus = useGameStatus();

  const lobbyCode = useLobbyCode();

  const playerCount = usePlayerCount();

  const { you, setYou, teammate } = usePlayers();

  const [maze] = useState<Maze>(getDefaultMaze);
  const [startCoords] = useState<Coordinates>([0, 0]);

  const gameValues: GameValues = useMemo(
    () => ({
      gameStatus,

      lobbyCode,

      playerCount,

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

const MINIMUM_LOADING_TIME = 1000;

function useGameStatus(): GameStatus {
  const [gameStatus, setGameStatus] = useState(GameStatus.Prelobby);

  const { addConnectionEventListener, removeConnectionEventListener } =
    useConnection();

  const timeoutRef = useRef<number | null>(null);

  useEffect(
    function listenForLobbyJoin() {
      const onLobbyJoin: GameConnectionListener<"lobbyJoin"> = ({
        success,
      }) => {
        if (!success) return;

        timeoutRef.current = setTimeout(() => {
          setGameStatus(GameStatus.Lobby);
        }, MINIMUM_LOADING_TIME);
      };

      addConnectionEventListener("lobbyJoin", onLobbyJoin);
      return () => {
        if (timeoutRef.current !== null) {
          clearTimeout(timeoutRef.current);
          timeoutRef.current = null;
        }
        removeConnectionEventListener("lobbyJoin", onLobbyJoin);
      };
    },
    [addConnectionEventListener, removeConnectionEventListener],
  );

  useEffect(
    function listenForTransitionToInGame() {
      const onTransition: GameConnectionListener<"transitionToInGame"> = () => {
        setGameStatus(GameStatus.InGame);
      };

      addConnectionEventListener("transitionToInGame", onTransition);
      return () =>
        removeConnectionEventListener("transitionToInGame", onTransition);
    },
    [addConnectionEventListener, removeConnectionEventListener],
  );

  return gameStatus;
}

function useLobbyCode(): string {
  const [lobbyCode, setLobbyCode] = useState("00000000");

  const { addConnectionEventListener, removeConnectionEventListener } =
    useConnection();

  const onLobbyJoin: GameConnectionListener<"lobbyJoin"> = ({ code }) => {
    setLobbyCode(code);
  };

  useEffect(
    function listenForLobbyJoin() {
      addConnectionEventListener("lobbyJoin", onLobbyJoin);
      return () => removeConnectionEventListener("lobbyJoin", onLobbyJoin);
    },
    [addConnectionEventListener, removeConnectionEventListener],
  );

  return lobbyCode;
}

function usePlayerCount(): number {
  const [playerCount, setPlayerCount] = useState(0);

  const { addConnectionEventListener, removeConnectionEventListener } =
    useConnection();

  const onLobbyJoin: GameConnectionListener<"lobbyJoin"> = ({
    playerCount,
  }) => {
    setPlayerCount(playerCount);
  };

  useEffect(
    function listenForLobbyJoin() {
      addConnectionEventListener("lobbyJoin", onLobbyJoin);
      return () => removeConnectionEventListener("lobbyJoin", onLobbyJoin);
    },
    [addConnectionEventListener, removeConnectionEventListener],
  );

  return playerCount;
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
