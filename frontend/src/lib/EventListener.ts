type Listener<T extends Array<unknown>> = (...args: T) => void;

/** An event emitter */
export class EventEmitter<EventMap extends Record<string, Array<unknown>>> {
  private listeners: { [K in keyof EventMap]?: Set<Listener<EventMap[K]>> } =
    {};

  /**
   * Subscribe an event listener to an event.
   * @param eventName The event to listen for.
   * @param listener The callback function when the event is emitted.
   */
  addListener<K extends keyof EventMap>(
    eventName: K,
    listener: Listener<EventMap[K]>,
  ): void {
    const listeners = this.listeners[eventName] ?? new Set();
    listeners.add(listener);
    this.listeners[eventName] = listeners;
  }

  /**
   * Unsubscribe an event listener from an event.
   * @param eventName The event to unsubscribe from.
   * @param listener The listener to unsubscribe.
   */
  removeListener<K extends keyof EventMap>(
    eventName: K,
    listener: Listener<EventMap[K]>,
  ): void {
    const listeners = this.listeners[eventName];
    if (!listeners) return;
    listeners.delete(listener);
  }

  /**
   * Emit an event.
   * @param eventName The type of event to emit.
   * @param args Data associated with that event passed to listeners.
   */
  emit<K extends keyof EventMap>(eventName: K, ...args: EventMap[K]): void {
    const listeners = this.listeners[eventName];
    if (!listeners) return;
    for (const listener of listeners) {
      listener(...args);
    }
  }
}
