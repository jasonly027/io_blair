import { useEffect, useMemo, useRef, useState } from "react";
import { QueuedSocket, SocketState } from "../lib/QueuedSocket";

export interface SocketValues extends SocketActions {
  state: SocketState;
}

export default function useSocket(): SocketValues {
  console.log("useSocket render");

  const socket = useRef<QueuedSocket>(null!);
  if (socket.current === null) {
    socket.current = new QueuedSocket(`ws://${import.meta.env.PUBLIC_SERVER_ENDPOINT}`);
    console.log(import.meta.env.PUBLIC_SERVER_ENDPOINT);
    
  }

  const [state, setState] = useState<SocketState>(SocketState.CONNECTING);

  const updateSocketState = () => setState(socket.current.state);

  const closeSocket = () => socket.current.close();

  useEffect(() => {
    socket.current.addEventListener("open", updateSocketState);
    socket.current.addEventListener("close", updateSocketState);

    socket.current.onmessage = ({ data }) => {
      console.log(data);
    };

    window.addEventListener("beforeunload", closeSocket);
    return () => window.removeEventListener("beforeunload", closeSocket);
  }, []);

  const actions = useMemo(() => {
    return createActions(socket.current);
  }, []);

  const socketValues = useMemo(() => {
    return {
      state,
      ...actions,
    };
  }, []);

  return socketValues;
}

export interface SocketActions {
  createLobby: () => void;
  joinLobby: (code: string) => void;
  // onLobbyJoin: ()
}

function createActions(socket: WebSocket): SocketActions {
  const sender = createJsonSender(socket);
  socket.addEventListener("message", () => {});

  return {
    createLobby: createLobby(sender),
    joinLobby: joinLobby(sender),
  };
}

interface Message {
  type: string;
  [key: string]: unknown;
}

type MessageSender = (msg: Message) => void;

function createJsonSender(socket: WebSocket): MessageSender {
  return (msg) => socket.send(JSON.stringify(msg));
}

function createLobby(sender: MessageSender) {
  return () => {
    sender({
      type: "lobbyCreate",
    });
  };
}

function joinLobby(sender: MessageSender) {
  return (code: string) =>
    sender({
      type: "lobbyJoin",
      code,
    });
}
