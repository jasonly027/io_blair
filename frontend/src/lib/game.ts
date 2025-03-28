let _gameFocused = true;

export function gameFocused(): boolean {
  return _gameFocused;
}

export function setGameFocused(value: boolean): void {
  _gameFocused = value;
}
