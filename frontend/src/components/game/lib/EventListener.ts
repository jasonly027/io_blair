type Listener<T extends Array<unknown>> = (...args: T) => void;

export class EventEmitter<EventMap extends Record<string, Array<unknown>>> {
  private listeners: { [K in keyof EventMap]?: Set<Listener<EventMap[K]>> } =
    {};

  on<K extends keyof EventMap>(eventName: K, listener: Listener<EventMap[K]>) {
    const listeners = this.listeners[eventName] ?? new Set();
    listeners.add(listener);
    this.listeners[eventName] = listeners;
  }

  emit<K extends keyof EventMap>(eventName: K, ...args: EventMap[K]) {
    const listeners = this.listeners[eventName];
    if (!listeners) return;
    for (const listener of listeners) {
      listener(...args);
    }
  }
}
