import type { GamePlayer } from "../types/character";
import type { Matrix } from "../types/tuple";

export interface Traversable {
  up: boolean;
  right: boolean;
  down: boolean;
  left: boolean;
}

export class Cell {
  private readonly self: Traversable;
  private readonly other: Traversable;

  constructor(
    self: Traversable = {
      up: false,
      right: false,
      down: false,
      left: false,
    },
    other: Traversable = {
      up: false,
      right: false,
      down: false,
      left: false,
    },
  ) {
    this.self = self;
    this.other = other;
  }

  up(who: GamePlayer | "both" | "either"): boolean {
    switch (who) {
      case "You":
        return this.self.up;
      case "Teammate":
        return this.other.up;
      case "both":
        return this.self.up && this.other.up;
      case "either":
        return this.self.up || this.other.up;
    }
  }

  right(who: GamePlayer | "both" | "either"): boolean {
    switch (who) {
      case "You":
        return this.self.right;
      case "Teammate":
        return this.other.right;
      case "both":
        return this.self.right && this.other.right;
      case "either":
        return this.self.right || this.other.right;
    }
  }

  down(who: GamePlayer | "both" | "either"): boolean {
    switch (who) {
      case "You":
        return this.self.down;
      case "Teammate":
        return this.other.down;
      case "both":
        return this.self.down && this.other.down;
      case "either":
        return this.self.down || this.other.down;
    }
  }

  left(who: GamePlayer | "both" | "either"): boolean {
    switch (who) {
      case "You":
        return this.self.left;
      case "Teammate":
        return this.other.left;
      case "both":
        return this.self.left && this.other.left;
      case "either":
        return this.self.left || this.other.left;
    }
  }

  set_up(who: GamePlayer | "both", value: boolean): void {
    switch (who) {
      case "You":
        this.self.up = value;
        break;
      case "Teammate":
        this.other.up = value;
        break;
      case "both":
        this.self.up = value;
        this.other.up = value;
        break;
    }
  }

  set_right(who: GamePlayer | "both", value: boolean): void {
    switch (who) {
      case "You":
        this.self.right = value;
        break;
      case "Teammate":
        this.other.right = value;
        break;
      case "both":
        this.self.right = value;
        this.other.right = value;
        break;
    }
  }

  set_down(who: GamePlayer | "both", value: boolean): void {
    switch (who) {
      case "You":
        this.self.down = value;
        break;
      case "Teammate":
        this.other.down = value;
        break;
      case "both":
        this.self.down = value;
        this.other.down = value;
        break;
    }
  }

  set_left(who: GamePlayer | "both", value: boolean): void {
    switch (who) {
      case "You":
        this.self.left = value;
        break;
      case "Teammate":
        this.other.left = value;
        break;
      case "both":
        this.self.left = value;
        this.other.left = value;
        break;
    }
  }
}

export type Coordinate = [x: number, y: number];

export default class Maze {
  readonly rows: number;
  readonly cols: number;
  private readonly moves: Matrix<Cell, 8, 8>;

  constructor(moves: Matrix<Cell, 8, 8>) {
    this.rows = moves.length;
    this.cols = moves[0].length;
    this.moves = moves;
  }

  /**
   * Shallow copies internal members.
   * @returns Maze
   */
  public clone(): Maze {
    return new Maze(this.moves);
  }

  public inRange(x: number, y: number): boolean {
    return x >= 0 && x < this.cols && y >= 0 && y < this.rows;
  }

  public at(x: number, y: number): Cell {
    if (!this.inRange(x, y)) {
      throw new Error("cell out of bounds");
    }
    return this.moves[y]![x]!;
  }
}
