let _gameFocused = true;

export function gameFocused(): boolean {
  return _gameFocused;
}

export function setGameFocused(value: boolean): void {
  _gameFocused = value;
}

export function setMetaThemeColor(color: string): void {
  const meta = document.querySelector('meta[name="theme-color"]');
  if (meta === null) return;
  meta.setAttribute("content", color);
}
