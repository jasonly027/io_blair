import type { GameCharacter } from "../types/character";
import { EventEmitter } from "./EventListener";
import type { Coordinate, MazeMatrix, TraversableKey } from "./Maze";
import { QueuedSocket, SocketState } from "./QueuedSocket";

/**
 * Maps event types of GameConnection with the corresponding data received.
 */
export type GameEventMap = {
  /** Indicates the WebSocket connection was established. */
  open: [];
  /** Indiactes the WebSocket connectionw was terminated. */
  close: [];
  /** Indicates the server responded to a lobby request. */
  lobbyJoin: [
    {
      /** Whether the request was successful. If false, code will be "". */
      success: boolean;
      /** The lobby code. */
      code: string;
      /** The current number of players, including the player. */
      playerCount: number;
      otherConfirm: GameCharacter | "unknown";
    },
  ];
  /** Indicates teammate has joined the lobby */
  lobbyOtherJoin: [];
  /** Indicates teammate has left the lobby */
  lobbyOtherLeave: [];
  /** Indicates the server forwarded a message from another player. */
  chat: [
    {
      /** The message from the other player. */
      msg: string;
    },
  ];
  /** Indicates the character hovered by the other player */
  characterHover: [
    {
      character: GameCharacter;
    },
  ];
  /** Indicates the character confirmed by the other player */
  characterConfirm: [
    {
      character: GameCharacter | null;
    },
  ];
  /** Indicates transition to ingame */
  transitionToInGame: [];
  inGameMaze: [
    {
      maze: MazeMatrix<number>;
      start: Coordinate;
      end: Coordinate;
      cell: number;
    },
  ];
  characterMove: [
    {
      coordinate: Coordinate;
      cell: number;
      reset: boolean;
    },
  ];
  characterOtherMove: [
    {
      direction: TraversableKey;
      reset: boolean;
    },
  ];
  coinTaken: [
    {
      coordinate: Coordinate;
    },
  ];
  transitionToGameDone: [];
};

export type GameEventKey = keyof GameEventMap;

/**
 * Maps types of messages that can be sent to the server and
 * the corresponding data required with them.
 */
export type GameSendMap = {
  ping: [];
  /** Request to create a lobby */
  lobbyCreate: [];
  /** Request to join a lobby */
  lobbyJoin: [
    {
      /** The lobby code of the lobby the player wants to join. */
      code: string;
    },
  ];
  /** Send a message to the lobby chat. */
  chat: [
    {
      /** The message to be sent to the chat. */
      msg: string;
    },
  ];
  /** Send the character being hovered */
  characterHover: [
    {
      character: GameCharacter;
    },
  ];
  /** Send the character confirmed */
  characterConfirm: [
    {
      character: GameCharacter | "unknown";
    },
  ];
  /** Send the new position of the player */
  characterMove: [
    {
      coordinate: Coordinate;
    },
  ];
  /** Request a new game */
  newGame: [];
};

export type GameSendKey = keyof GameSendMap;

export type GameConnectionListener<K extends GameEventKey> = (
  ...args: GameEventMap[K]
) => void;

/**
 * A connection to the game server that allows sending messages
 * and listening to events. close() should be called when connection
 * usage is finished.
 */
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

  /** Forward WebSocket open/close events to the emitter */
  private setupSocket(): void {
    this.addSocketListener("open", () => this.eventEmitter.emit("open"));
    this.addSocketListener("close", () => this.eventEmitter.emit("close"));

    const interval = setInterval(() => {
      this.send("ping");
    }, 10000);
    this.cleanupActions.push(() => {
      clearInterval(interval);
    });
  }

  /** Add an event listener to the WebSocket. The unsubscribe
   * function is stored in cleanupActions.
   */
  private addSocketListener<K extends keyof WebSocketEventMap>(
    eventName: K,
    listener: (ev: WebSocketEventMap[K]) => void,
  ): void {
    this.socket.addEventListener(eventName, listener);
    this.cleanupActions.push(() =>
      this.socket.removeEventListener(eventName, listener),
    );
  }

  /** Forward WebSocket onmessage events to the emitter */
  private setupEventEmitter(): void {
    this.addSocketListener("message", ({ data }) => {
      const obj = JSON.parse(data);

      console.log(obj);

      const gameEvent = toGameEvent(obj);
      if (gameEvent === null) return;

      const [eventName, event] = gameEvent;
      this.eventEmitter.emit(eventName, ...event);
    });
  }

  /** Close the connection to the game server. */
  close(): void {
    this.cleanupActions.forEach((action) => action());

    if (this.socket.state === SocketState.OPEN) {
      this.socket.close();
    }
  }

  get state(): SocketState {
    return this.socket.state;
  }

  /**
   * Listen to a game event.
   * @param eventName The event to listen to.
   * @param listener The callback function when the event is emitted.
   */
  addEventListener<K extends GameEventKey>(
    eventName: K,
    listener: GameConnectionListener<K>,
  ): void {
    this.eventEmitter.addListener(eventName, listener);
  }

  /**
   * Unsubscribe a listener from a game event. Does something if the
   * listener isn't subscribed to the event.
   * @param eventName The event to unsubscribe from.
   * @param listener The listener to unsubscribe.
   */
  removeEventListener<K extends GameEventKey>(
    eventName: K,
    listener: GameConnectionListener<K>,
  ): void {
    this.eventEmitter.removeListener(eventName, listener);
  }

  /**
   * Send a message to the game server.
   * @param messageType The type of message to send.
   * @param args The associated data with that message.
   */
  send<K extends GameSendKey>(messageType: K, ...args: GameSendMap[K]): void {
    const msg = Object.assign({ type: messageType }, ...args);
    this.socket.send(JSON.stringify(msg));
  }
}

// Necessary for toGameEvent() to check if the "type" key
// in a server reply is one of the types in the GameEventMap.
const gameEventMapSchema = {
  open: [],
  close: [],
  lobbyJoin: [
    { success: false, code: "", playerCount: 0, otherConfirm: "unknown" },
  ],
  lobbyOtherJoin: [],
  lobbyOtherLeave: [],
  chat: [{ msg: "" }],
  characterHover: [{ character: "Io" }],
  characterConfirm: [{ character: null }],
  transitionToInGame: [],
  inGameMaze: [
    {
      maze: undefined!,
      start: [0, 0],
      end: [0, 0],
      cell: 0,
    },
  ],
  characterMove: [
    {
      coordinate: [0, 0],
      cell: 0,
      reset: false,
    },
  ],
  characterOtherMove: [
    {
      direction: "up",
      reset: false,
    },
  ],
  coinTaken: [
    {
      coordinate: [0, 0],
    },
  ],
  transitionToGameDone: [],
} as const satisfies GameEventMap;

/** Convert a raw message from the server to a GameEvent */
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
