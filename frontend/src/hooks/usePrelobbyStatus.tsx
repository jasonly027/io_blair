import { useCallback, useEffect, useMemo, useRef, useState } from "react";
import type { GameConnectionListener } from "../lib/GameConnection";
import { GameStatus, useSession } from "./useSession";

/** The current status of the prelobby */
export enum PrelobbyStatus {
  Fresh,
  Loading,
  JoinFailed,
}

/** Minimum time to spend on the loading screen */
const MINIMUM_LOADING_TIME = 1500;

export interface PrelobbyStatusValues {
  status: PrelobbyStatus;
  setToLoading: () => void;
}

/**
 * Returns the status of the prelobby. Must be used within a SessionProvider
 * component. Listens for the "lobbyJoin" event from the server and updates
 * the prelobby status to JoinFailed or the game status to Lobby accordingly.
 */
export default function usePrelobbyStatus(): PrelobbyStatusValues {
  const [status, setStatus] = useState<PrelobbyStatus>(PrelobbyStatus.Fresh);

  const {
    setGameStatus,
    setLobbyCode,
    setPlayerCount,
    addConnectionEventListener,
    removeConnectionEventListener,
  } = useSession();

  const timeoutRef = useRef<number | null>(null);

  const onLobbyJoin: GameConnectionListener<"lobbyJoin"> = useCallback(
    ({ success, code, playerCount }) => {
      timeoutRef.current = setTimeout(() => {
        if (!success) {
          setStatus(PrelobbyStatus.JoinFailed);
          return;
        }
        setLobbyCode(code);
        setPlayerCount(playerCount);
        setGameStatus(GameStatus.Lobby);
      }, MINIMUM_LOADING_TIME);
    },
    [setGameStatus, setLobbyCode, setPlayerCount],
  );

  useEffect(
    function registerJoinListener() {
      addConnectionEventListener("lobbyJoin", onLobbyJoin);
      return () => {
        if (timeoutRef.current) {
          clearTimeout(timeoutRef.current);
          timeoutRef.current = null;
        }
        removeConnectionEventListener("lobbyJoin", onLobbyJoin);
      };
    },
    [addConnectionEventListener, removeConnectionEventListener, onLobbyJoin],
  );

  const values: PrelobbyStatusValues = useMemo(
    () => ({
      status,
      setToLoading: () => setStatus(PrelobbyStatus.Loading),
    }),
    [status],
  );

  return values;
}
