export interface Cell {
  up: boolean;
  right: boolean;
  down: boolean;
  left: boolean;
}

export type CellKey = keyof Cell;

export function cellFromNumber(num: number): Cell {
  return {
    up: ((num >> 0) & 1) == 1,
    right: ((num >> 1) & 1) == 1,
    down: ((num >> 2) & 1) == 1,
    left: ((num >> 3) & 1) == 1,
  };
}

export type Coordinates = [x: number, y: number];

export default class Maze {
  readonly rows: number;
  readonly cols: number;
  readonly moves: Cell[][];

  constructor(moves: Cell[][]) {
    if (moves.length <= 0 || moves[0] === undefined || moves[0].length <= 0) {
      throw new Error("rows and columns must be greater than 0");
    }
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

  public inRange([x, y]: Coordinates): boolean {
    return x >= 0 && x < this.cols && y >= 0 && y < this.rows;
  }

  public getCell([x, y]: Coordinates): Cell {
    if (!this.inRange([x, y])) {
      throw new Error("cell out of bounds");
    }
    return this.moves[y]![x]!;
  }

  public setCell([x, y]: Coordinates, cell: Cell): void {
    if (!this.inRange([x, y])) {
      throw new Error("cell out of bounds");
    }
    this.moves[y]![x]! = cell;
  }
}
