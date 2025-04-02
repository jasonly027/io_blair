/** The current state of the WebSocket */
export const enum SocketState {
  CONNECTING = 0,
  OPEN = 1,
  CLOSING = 2,
  CLOSED = 3,
}

type Message = string | ArrayBufferLike | Blob | ArrayBufferView;

/**
 * A WebSocket that allows immediate message sending on instantiation.
 * If the underlying WebSocket is still connecting, messages will be queued.
 */
export class QueuedSocket extends WebSocket {
  private msgQueue: Message[];

  constructor(url: string, protocols?: string | string[]) {
    super(url, protocols);
    this.msgQueue = [];

    this.addEventListener("open", () => {
      while (this.msgQueue.length > 0) {
        this.send(this.msgQueue.shift()!);
      }
    });

    this.addEventListener("close", (err) => {
      console.log("Close error", err);
    })
    this.addEventListener("error", (err) => {
      console.log("Error", err);
    })
  }

  /**
   * Sends a message through the WebSocket. The message will be queued
   * if the WebSocket is still connecting.
   * @param data The message to be sent.
   */
  override send(data: Message): void {
    if (this.readyState === SocketState.CONNECTING) {
      this.msgQueue.push(data);
      return;
    }
    super.send(data);
  }

  /** The current state of the WebSocket */
  public get state(): SocketState {
    return this.readyState;
  }
}
