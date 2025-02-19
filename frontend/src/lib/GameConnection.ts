import { EventEmitter } from "./EventListener";
import { QueuedSocket, SocketState } from "./QueuedSocket";

export type GameEventMap = {
  open: [];
  close: [];
  lobbyJoin: [{ success: boolean; code: string; playerCount: number }];
  chat: [{ msg: string }];
};

export type GameEventKey = keyof GameEventMap;

export type GameSendMap = {
  lobbyCreate: [];
  lobbyJoin: [{ code: string }];
  chat: [{ msg: string }];
};

export type GameSendKey = keyof GameSendMap;

export type GameConnectionListener<K extends GameEventKey> = (
  ...args: GameEventMap[K]
) => void;

export class GameConnection {
  private socket: QueuedSocket;
  private eventEmitter: EventEmitter<GameEventMap>;
  private cleanupActions: Array<() => void> = [];

  constructor(url: string, protocols?: string | string[]) {
    this.socket = new QueuedSocket(url, protocols);
    this.setupSocket();

    this.eventEmitter = new EventEmitter();
    this.setupEventEmitter();
  }

  private setupSocket(): void {
    this.addSocketListener("open", () => this.eventEmitter.emit("open"));
    this.addSocketListener("close", () => this.eventEmitter.emit("close"));
  }

  private addSocketListener<K extends keyof WebSocketEventMap>(
    eventName: K,
    listener: (ev: WebSocketEventMap[K]) => void,
  ): void {
    this.socket.addEventListener(eventName, listener);
    this.cleanupActions.push(() =>
      this.socket.removeEventListener(eventName, listener),
    );
  }

  private setupEventEmitter(): void {
    this.addSocketListener("message", ({ data }) => {
      const obj = JSON.parse(data);
      const gameEvent = toGameEvent(obj);
      if (gameEvent === null) return;

      const [eventName, event] = gameEvent;
      this.eventEmitter.emit(eventName, ...event);
    });
  }

  close(): void {
    this.cleanupActions.forEach((action) => action());

    if (this.socket.state === SocketState.OPEN) {
      this.socket.close();
    }
  }

  get state(): SocketState {
    return this.socket.state;
  }

  addEventListener<K extends GameEventKey>(
    eventName: K,
    listener: GameConnectionListener<K>,
  ): void {
    this.eventEmitter.addListener(eventName, listener);
  }

  removeEventListener<K extends GameEventKey>(
    eventName: K,
    listener: GameConnectionListener<K>,
  ): void {
    this.eventEmitter.removeListener(eventName, listener);
  }

  send<K extends GameSendKey>(messageType: K, ...args: GameSendMap[K]): void {
    const msg = Object.assign({ type: messageType }, ...args);
    this.socket.send(JSON.stringify(msg));
  }
}

const gameEventMapSchema: GameEventMap = {
  open: [],
  close: [],
  lobbyJoin: [{ success: false, code: "", playerCount: 0 }],
  chat: [{ msg: "" }],
};

function toGameEvent(
  data: unknown,
): [type: GameEventKey, data: GameEventMap[GameEventKey]] | null {
  if (
    typeof data !== "object" ||
    data === null ||
    !("type" in data) ||
    typeof data.type !== "string" ||
    !(data.type in gameEventMapSchema)
  ) {
    return null;
  }

  const eventName = data.type as GameEventKey;
  const event = [data as unknown] as GameEventMap[GameEventKey];

  return [eventName, event];
}
