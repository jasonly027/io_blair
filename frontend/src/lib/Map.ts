import { DIST_TO_NEXT_CELL } from "../components/InGame/Map";
import type { Coordinate } from "./Maze";

export function toMapUnits([x, z]: Readonly<Coordinate>): Coordinate {
  return [x * DIST_TO_NEXT_CELL, z * DIST_TO_NEXT_CELL];
}

export function toNonMapUnits([x, z]: Readonly<Coordinate>): Coordinate {
  return [x / DIST_TO_NEXT_CELL, z / DIST_TO_NEXT_CELL];
}
