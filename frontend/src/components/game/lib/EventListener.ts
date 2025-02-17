type Listener<T extends Array<unknown>> = (...args: T) => void;

export class EventEmitter<EventMap extends Record<string, Array<unknown>>> {
  private listeners: { [K in keyof EventMap]?: Set<Listener<EventMap[K]>> } =
    {};

  addListener<K extends keyof EventMap>(
    eventName: K,
    listener: Listener<EventMap[K]>,
  ): void {
    const listeners = this.listeners[eventName] ?? new Set();
    listeners.add(listener);
    this.listeners[eventName] = listeners;
  }

  removeListener<K extends keyof EventMap>(
    eventName: K,
    listener: Listener<EventMap[K]>,
  ): void {
    const listeners = this.listeners[eventName];
    if (!listeners) return;
    listeners.delete(listener);
  }

  emit<K extends keyof EventMap>(eventName: K, ...args: EventMap[K]): void {
    const listeners = this.listeners[eventName];
    if (!listeners) return;
    for (const listener of listeners) {
      listener(...args);
    }
  }
}
