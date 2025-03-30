import {
  useCallback,
  useEffect,
  useMemo,
  useRef,
  useState,
  type DispatchWithoutAction,
  type ReactNode,
} from "react";
import Maze, {
  Cell,
  coordEqual,
  translate,
  type Coordinate,
  type MazeMatrix,
} from "../../lib/Maze";
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
  const gameStatus = useGameStatus();

  const lobbyCode = useLobbyCode();

  const playerCount = usePlayerCount();

  const { you, setYou, teammate } = usePlayers();

  const { map, currentCoins, totalCoins } = useMap();

  const youCoord = useYouCoord(map);

  const teammateCoord = useTeammateCoord(map);

  const { movesCount, incrementMovesCount } = useMovesCount();

  const gameDone = useGameDone(
    youCoord,
    teammateCoord,
    map,
    currentCoins,
    totalCoins,
  );

  const gameValues: GameValues = useMemo(
    () => ({
      gameStatus,

      lobbyCode,

      playerCount,

      you,
      setYou,

      teammate,

      map,
      currentCoins,
      totalCoins,

      youCoord,
      teammateCoord,

      movesCount,
      incrementMovesCount,

      gameDone,
    }),
    [
      gameStatus,
      lobbyCode,
      playerCount,
      you,
      setYou,
      teammate,
      map,
      currentCoins,
      totalCoins,
      youCoord,
      teammateCoord,
      movesCount,
      incrementMovesCount,
      gameDone,
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

  useEffect(function listenForLobbyOtherLeave() {
    const onLobbyOtherLeave: GameConnectionListener<"lobbyOtherLeave"> = () => {
      setGameStatus(GameStatus.Lobby);
    };

    addConnectionEventListener("lobbyOtherLeave", onLobbyOtherLeave);
    return () =>
      removeConnectionEventListener("lobbyOtherLeave", onLobbyOtherLeave);
  });

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

  useEffect(
    function listenForTransitionToGameDone() {
      const onTransition: GameConnectionListener<
        "transitionToGameDone"
      > = () => {
        setGameStatus(GameStatus.GameDone);
      };

      addConnectionEventListener("transitionToGameDone", onTransition);
      return () =>
        removeConnectionEventListener("transitionToGameDone", onTransition);
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

  const onLobbyOtherJoin: GameConnectionListener<"lobbyOtherJoin"> = () => {
    setPlayerCount((prev) => prev + 1);
  };

  const onLobbyOtherLeave: GameConnectionListener<"lobbyOtherLeave"> = () => {
    setPlayerCount((prev) => prev - 1);
  };

  useEffect(
    function listenForLobbyJoin() {
      addConnectionEventListener("lobbyJoin", onLobbyJoin);
      addConnectionEventListener("lobbyOtherJoin", onLobbyOtherJoin);
      addConnectionEventListener("lobbyOtherLeave", onLobbyOtherLeave);

      return () => {
        removeConnectionEventListener("lobbyJoin", onLobbyJoin);
        removeConnectionEventListener("lobbyOtherJoin", onLobbyOtherJoin);
        removeConnectionEventListener("lobbyOtherLeave", onLobbyOtherLeave);
      };
    },
    [addConnectionEventListener, removeConnectionEventListener],
  );

  return playerCount;
}

interface useMapValues {
  map: Maze;
  currentCoins: number;
  totalCoins: number;
}

function useMap(): useMapValues {
  const [maze, setMaze] = useState<Maze>(getDefaultMaze);

  const [currentCoins, setCurrentCoins] = useState(0);
  const [totalCoins, setTotalCoins] = useState(1);

  const { addConnectionEventListener, removeConnectionEventListener } =
    useConnection();

  useEffect(
    function listenForInGameMaze() {
      const onInGameMaze: GameConnectionListener<"inGameMaze"> = ({
        maze,
        start,
        end,
        cell,
      }) => {
        const map = new Maze(deserializeMatrix(maze), start, end);
        updateCellWithOther(map, start, cell);

        setMaze(map);
        setCurrentCoins(0);
        setTotalCoins(
          map.matrix.reduce(
            (sum, row) =>
              sum + row.reduce((sum, cell) => sum + (cell.coin() ? 1 : 0), 0),
            0,
          ),
        );
      };

      addConnectionEventListener("inGameMaze", onInGameMaze);
      return () => removeConnectionEventListener("inGameMaze", onInGameMaze);
    },
    [addConnectionEventListener, removeConnectionEventListener],
  );

  useEffect(
    function listenForCharacterMove() {
      const onCharacterMove: GameConnectionListener<"characterMove"> = ({
        coordinate,
        cell,
        reset,
      }) => {
        if (reset) return;

        setMaze((prev) => {
          const maze = prev.clone();
          updateCellWithOther(maze, coordinate, cell);
          return maze;
        });
      };

      addConnectionEventListener("characterMove", onCharacterMove);
      return () =>
        removeConnectionEventListener("characterMove", onCharacterMove);
    },
    [addConnectionEventListener, removeConnectionEventListener],
  );

  useEffect(
    function listenForCoinTaken() {
      const onCoinTaken: GameConnectionListener<"coinTaken"> = ({
        coordinate,
      }) => {
        setMaze((prev) => {
          const maze = prev.clone();
          maze.take_coin(coordinate);
          return maze;
        });
        setCurrentCoins((prev) => prev + 1);
      };

      addConnectionEventListener("coinTaken", onCoinTaken);
      return () => removeConnectionEventListener("coinTaken", onCoinTaken);
    },
    [addConnectionEventListener, removeConnectionEventListener],
  );

  return useMemo(
    () => ({
      map: maze,
      currentCoins,
      totalCoins,
    }),
    [currentCoins, maze, totalCoins],
  );
}

function updateCellWithOther(
  maze: Maze,
  coordinate: Coordinate,
  other: number,
) {
  const nth_bit = (n: number): boolean => ((other >> n) & 1) == 1;
  maze.bridge("Teammate", coordinate, "up", nth_bit(4));
  maze.bridge("Teammate", coordinate, "right", nth_bit(5));
  maze.bridge("Teammate", coordinate, "down", nth_bit(6));
  maze.bridge("Teammate", coordinate, "left", nth_bit(7));
}

function getDefaultMaze(): Maze {
  const cells = [
    [64, 32, 486, 168, 160, 196],
    [84, 102, 153, 68, 34, 153],
    [85, 85, 100, 145, 34, 200],
    [51, 153, 85, 34, 136, 84],
    [64, 34, 187, 170, 170, 153],
    [50, 170, 136, 34, 170, 136],
  ].map((row) => row.map(deserializeCell)) as MazeMatrix<Cell>;

  return new Maze(cells, [1, 4], [4, 1]);
}

function deserializeMatrix(maze: MazeMatrix<number>): MazeMatrix<Cell> {
  const cells = maze.map((row) => {
    return row.map((num) => deserializeCell(num));
  }) as MazeMatrix<Cell>;

  return cells;
}

function deserializeCell(num: number): Cell {
  // Determines if nth bit of num is on
  const nth_bit = (n: number): boolean => ((num >> n) & 1) == 1;

  const cell = new Cell();

  cell.set_up("both", nth_bit(0));
  cell.set_right("both", nth_bit(1));
  cell.set_down("both", nth_bit(2));
  cell.set_left("both", nth_bit(3));

  cell.set_up("You", nth_bit(4));
  cell.set_right("You", nth_bit(5));
  cell.set_down("You", nth_bit(6));
  cell.set_left("You", nth_bit(7));

  cell.set_coin(nth_bit(8));

  return cell;
}

function useYouCoord(map: Maze): Readonly<Coordinate> {
  const [coord, setCoord] = useState(map.start);

  const { addConnectionEventListener, removeConnectionEventListener } =
    useConnection();

  useEffect(
    function listenForInGameMaze() {
      const onInGameMaze: GameConnectionListener<"inGameMaze"> = ({
        start,
      }) => {
        setCoord(start);
      };

      addConnectionEventListener("inGameMaze", onInGameMaze);
      return () => removeConnectionEventListener("inGameMaze", onInGameMaze);
    },
    [addConnectionEventListener, removeConnectionEventListener],
  );

  useEffect(
    function listenForCharacterMove() {
      const onCharacterMove: GameConnectionListener<"characterMove"> = ({
        coordinate,
        reset,
      }) => {
        if (reset) {
          setCoord(map.start);
          return;
        }
        setCoord(coordinate);
      };

      addConnectionEventListener("characterMove", onCharacterMove);
      return () =>
        removeConnectionEventListener("characterMove", onCharacterMove);
    },
    [addConnectionEventListener, map.start, removeConnectionEventListener],
  );

  return coord;
}

function useTeammateCoord(map: Maze): Readonly<Coordinate> {
  const [coord, setCoord] = useState(map.start);

  const { addConnectionEventListener, removeConnectionEventListener } =
    useConnection();

  useEffect(
    function listenForInGameMaze() {
      const onInGameMaze: GameConnectionListener<"inGameMaze"> = ({
        start,
      }) => {
        setCoord(start);
      };

      addConnectionEventListener("inGameMaze", onInGameMaze);
      return () => removeConnectionEventListener("inGameMaze", onInGameMaze);
    },
    [addConnectionEventListener, removeConnectionEventListener],
  );

  useEffect(
    function listenForCharacterMove() {
      const onCharacterMove: GameConnectionListener<"characterOtherMove"> = ({
        direction,
        reset,
      }) => {
        if (reset) {
          setCoord(map.start);
          return;
        }
        setCoord((prev) => translate(prev, direction));
      };

      addConnectionEventListener("characterOtherMove", onCharacterMove);
      return () =>
        removeConnectionEventListener("characterOtherMove", onCharacterMove);
    },
    [addConnectionEventListener, map.start, removeConnectionEventListener],
  );

  return coord;
}

interface useMovesCountValues {
  movesCount: number;
  incrementMovesCount: DispatchWithoutAction;
}

function useMovesCount(): useMovesCountValues {
  const [movesCount, setMovesCount] = useState(0);

  const { addConnectionEventListener, removeConnectionEventListener } =
    useConnection();

  useEffect(function listenForInGameMaze() {
    const onInGameMaze: GameConnectionListener<"inGameMaze"> = () => {
      setMovesCount(0);
    };

    addConnectionEventListener("inGameMaze", onInGameMaze);
    return () => removeConnectionEventListener("inGameMaze", onInGameMaze);
  });

  const incrementMovesCount = useCallback(
    () => setMovesCount((prev) => prev + 1),
    [],
  );

  return useMemo(
    () => ({
      movesCount,
      incrementMovesCount,
    }),
    [incrementMovesCount, movesCount],
  );
}

function useGameDone(
  youCoord: Readonly<Coordinate>,
  teammateCoord: Readonly<Coordinate>,
  map: Maze,
  currentCoins: number,
  totalCoins: number,
): boolean {
  return useMemo(
    () =>
      coordEqual(youCoord, map.end) &&
      coordEqual(teammateCoord, map.end) &&
      currentCoins === totalCoins,
    [currentCoins, map.end, teammateCoord, totalCoins, youCoord],
  );
}
