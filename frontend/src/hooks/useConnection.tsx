import { useEffect, useMemo, useRef, useState } from "react";
import { SocketState } from "../lib/QueuedSocket";
import {
  GameConnection,
  type GameConnectionListener,
  type GameEventKey,
} from "../lib/GameConnection";

export interface ConnectionValues extends ConnectionActions {
  state: SocketState;
}

/**
 * Initializes a GameConnection and exposes actions on it.
 * @returns The state of the connection, event listening capabilities,
 * and possible send actions.
 */
export default function useConnection(): ConnectionValues {
  const connection = useRef<GameConnection>(null!);
  if (connection.current === null) {
    connection.current = new GameConnection(
      `ws://${import.meta.env.VITE_SERVER_ENDPOINT}`,
    );
  }

  const closeConnection = () => connection.current.close();

  const [state, setState] = useState<SocketState>(SocketState.CONNECTING);

  const updateConnectionState = () => setState(connection.current.state);

  useEffect(() => {
    connection.current.addEventListener("open", updateConnectionState);
    connection.current.addEventListener("close", updateConnectionState);
    window.addEventListener("beforeunload", closeConnection);

    return () => {
      connection.current.removeEventListener("open", updateConnectionState);
      connection.current.removeEventListener("close", updateConnectionState);
      window.removeEventListener("beforeunload", closeConnection);

      closeConnection();
    };
  }, []);

  const connectionValues = useMemo(() => {
    return {
      state,
      ...createActions(connection.current),
    };
  }, [state]);

  return connectionValues;
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
}

function createActions(connection: GameConnection): ConnectionActions {
  return {
    addConnectionEventListener(eventName, listener) {
      connection.addEventListener(eventName, listener);
    },

    removeConnectionEventListener(eventName, listener) {
      connection.removeEventListener(eventName, listener);
    },

    createLobby() {
      connection.send("lobbyCreate");
    },

    joinLobby(code) {
      connection.send("lobbyJoin", { code });
    },

    message(msg) {
      connection.send("chat", { msg });
    },
  };
}
