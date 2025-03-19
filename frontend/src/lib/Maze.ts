import type { GamePlayer } from "../types/character";
import type { Matrix } from "../types/tuple";

export interface Traversable {
  up: boolean;
  right: boolean;
  down: boolean;
  left: boolean;
}

export type TraversableKey = keyof Traversable;

type CellGetDirectionParam = GamePlayer | "both" | "either";

export class Cell {
  private readonly self: Traversable;
  private readonly other: Traversable;
  private _coin: boolean;

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
    coin: boolean = false,
  ) {
    this.self = self;
    this.other = other;
    this._coin = coin;
  }

  up(who: CellGetDirectionParam): boolean {
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

  right(who: CellGetDirectionParam): boolean {
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

  down(who: CellGetDirectionParam): boolean {
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

  left(who: CellGetDirectionParam): boolean {
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

  coin(): boolean {
    return this._coin;
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

  set_coin(value: boolean): void {
    this._coin = value;
  }
}

export type Coordinate = [x: number, y: number];

export type MazeMatrix<T> = Matrix<T, 6, 6>;

export default class Maze {
  readonly matrix: MazeMatrix<Cell>;
  readonly rows: number;
  readonly cols: number;
  readonly start: Readonly<Coordinate>;
  readonly end: Readonly<Coordinate>;

  constructor(
    matrix: MazeMatrix<Cell>,
    start: Readonly<Coordinate>,
    end: Readonly<Coordinate>,
  ) {
    this.matrix = matrix;
    this.rows = matrix.length;
    this.cols = matrix[0].length;
    this.start = start;
    this.end = end;
  }

  /**
   * Shallow copies internal members.
   * @returns Maze
   */
  public clone(): Maze {
    return new Maze(this.matrix, this.start, this.end);
  }

  public inRange(x: number, y: number): boolean {
    return x >= 0 && x < this.cols && y >= 0 && y < this.rows;
  }

  public bridge(
    who: GamePlayer | "both",
    [x, y]: Coordinate,
    dir: TraversableKey,
    value: boolean,
  ): void {
    if (!this.inRange(x, y)) return;
    this.matrix[y]![x]![`set_${dir}`](who, value);

    [x, y] = translate([x, y], dir);
    if (!this.inRange(x, y)) return;
    this.matrix[y]![x]![`set_${opposite(dir)}`](who, value);
  }
}

export const moveMap = {
  up: [0, -1],
  right: [1, 0],
  down: [0, 1],
  left: [-1, 0],
} as const satisfies Record<TraversableKey, Coordinate>;

export function translate(
  [x, z]: Readonly<Coordinate>,
  dir: TraversableKey,
  value: number = 1,
): Coordinate {
  const [dX, dZ] = moveMap[dir];
  return [x + dX * value, z + dZ * value];
}

export function opposite(dir: TraversableKey): TraversableKey {
  switch (dir) {
    case "up":
      return "down";
    case "right":
      return "left";
    case "down":
      return "up";
    case "left":
      return "right";
  }
}
