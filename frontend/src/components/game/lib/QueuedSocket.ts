export const enum SocketState {
  CONNECTING = 0,
  OPEN = 1,
  CLOSING = 2,
  CLOSED = 3,
}

type Message = string | ArrayBufferLike | Blob | ArrayBufferView;

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
  }

  override send(data: Message): void {
    if (this.readyState === SocketState.CONNECTING) {
      this.msgQueue.push(data);
      return;
    }
    super.send(data);
  }

  public get state(): SocketState {
    return this.readyState;
  }
}
