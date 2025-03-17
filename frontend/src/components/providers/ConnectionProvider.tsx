import { useEffect, useMemo, useRef, useState, type ReactNode } from "react";
import { GameConnection } from "../../lib/GameConnection";
import { SocketState } from "../../lib/QueuedSocket";
import {
  ConnectionContext,
  type ConnectionActions,
} from "../../hooks/useConnection";

/**
 * A provider for useConnection.
 *
 * Exposes the state of the server connection,
 * event listening capabilities,
 * and send actions.
 */
export default function ConnectionProvider({
  children,
}: {
  children?: ReactNode;
}) {
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

  return (
    <ConnectionContext.Provider value={connectionValues}>
      {children}
    </ConnectionContext.Provider>
  );
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

    hoverCharacter(character) {
      connection.send("characterHover", { character });
    },

    confirmCharacter(character) {
      connection.send("characterConfirm", {
        character: character ?? "unknown",
      });
    },

    characterMove(coordinate) {
      connection.send("characterMove", {
        coordinate,
      });
    },
  };
}
