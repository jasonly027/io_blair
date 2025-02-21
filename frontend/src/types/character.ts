export type GamePlayer = "You" | "Teammate";

export type GameCharacter = "Io" | "Blair";

/** Get the color associated with a GamePlayer */
export const playerColorMap = {
  You: "oklch(0.596 0.145 163.225)",
  Teammate: "",
} as const satisfies Record<GamePlayer, string>;
