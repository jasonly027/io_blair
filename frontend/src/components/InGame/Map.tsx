import useGame from "../../hooks/useGame";
import { RigidBody } from "@react-three/rapier";
import { Cell as MazeCell, type Coordinate } from "../../lib/Maze";
import type { ReactNode } from "react";

export const CELL_LEN = 1;
export const DIST_TO_NEXT_CELL = CELL_LEN + CELL_LEN;
export const DIST_TO_NEXT_GAP = DIST_TO_NEXT_CELL / 2;

export const GROUND_Y = 0;
export const OOB_Y = -10;

export default function Map() {
  console.log("Map render");
  const { map } = useGame();

  return (
    <group position={[0, GROUND_Y - CELL_LEN / 2, 0]}>
      {map.matrix.flatMap((row, zIdx) =>
        row.flatMap((cell, xIdx) => {
          const x = xIdx + xIdx * CELL_LEN;
          const z = zIdx + zIdx * CELL_LEN;
          return <Cell key={`${x},${z}`} coordinate={[x, z]} cell={cell} />;
        }),
      )}
    </group>
  );
}

interface CellProps {
  coordinate: Coordinate;
  cell: MazeCell;
}

function Cell({ coordinate: [x, z], cell }: CellProps) {
  return (
    <>
      <Tile coordinate={[x, z]}>
        {cell.right("either") ? <Gap type="right" cell={cell} /> : null}
        {cell.down("either") ? <Gap type="down" cell={cell} /> : null}
      </Tile>
    </>
  );
}

interface TileProps {
  coordinate: Coordinate;
  children?: ReactNode;
}

function Tile({ coordinate: [x, z], children }: TileProps) {
  return (
    <RigidBody position={[x, 0, z]} type="fixed">
      <mesh>
        <boxGeometry args={[CELL_LEN, CELL_LEN, CELL_LEN]} />
        <meshStandardMaterial color="darkgreen" />
      </mesh>
      {children}
    </RigidBody>
  );
}

interface GapProps {
  type: "right" | "down";
  cell: MazeCell;
}

const Z_FIGHTING_BUFFER = 0.001;

function Gap({ type, cell }: GapProps) {
  const cellDir = cell[type].bind(cell);

  return (
    <RigidBody
      position={[
        type === "right" ? DIST_TO_NEXT_GAP : 0,
        0,
        type === "down" ? DIST_TO_NEXT_GAP : 0,
      ]}
      type="fixed"
    >
      <mesh>
        <boxGeometry
          args={[
            CELL_LEN - Z_FIGHTING_BUFFER,
            CELL_LEN,
            CELL_LEN - Z_FIGHTING_BUFFER,
          ]}
        />
        <meshStandardMaterial
          color={
            cellDir("both") ? "white" : cellDir("You") ? "dodgerblue" : "pink"
          }
          transparent={!cellDir("both")}
          opacity={0.5}
        />
      </mesh>
    </RigidBody>
  );
}
