import useGame from "../../hooks/useGame";
import { CuboidCollider, RigidBody } from "@react-three/rapier";
import { Cell as MazeCell, type Coordinate } from "../../lib/Maze";
import { useEffect, useRef, useState, type ReactNode } from "react";
import { useFrame } from "@react-three/fiber";
import type { Mesh } from "three";
import { SUCCESSFUL_MOVE_DURATION } from "../../hooks/useBody";

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

function Cell({ coordinate, cell }: CellProps) {
  return (
    <>
      <Tile coordinate={coordinate}>
        {cell.right("either") ? <Gap type="right" cell={cell} /> : null}
        {cell.down("either") ? <Gap type="down" cell={cell} /> : null}
      </Tile>
      <Coin coordinate={coordinate} exists={cell.coin()} />
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
      <mesh receiveShadow castShadow>
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
    <mesh
      position={[
        type === "right" ? DIST_TO_NEXT_GAP : 0,
        0,
        type === "down" ? DIST_TO_NEXT_GAP : 0,
      ]}
    >
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
  );
}

function Coin({
  coordinate: [x, z],
  exists,
}: {
  coordinate: Coordinate;
  exists: boolean;
}) {
  const [visible, setVisible] = useState(exists);

  const meshRef = useRef<Mesh>(null);
  useFrame((_, delta) => {
    if (meshRef.current === null) return;
    meshRef.current.rotateZ(delta * 1.5);
  });

  useEffect(() => {
    if (exists) return;

    const timeout = setTimeout(() => {
      setVisible(false);
    }, SUCCESSFUL_MOVE_DURATION);

    return () => clearTimeout(timeout);
  }, [exists]);

  return visible ? (
    <RigidBody
      position={[x, CELL_LEN, z]}
      rotation={[-Math.PI * 1.5, 0, 0]}
      type="fixed"
      colliders={false}
    >
      <CuboidCollider
        sensor
        args={[0.2, 0.2, 0.2]}
        onIntersectionEnter={() => {
          setVisible(false);
        }}
      />
      <mesh castShadow receiveShadow ref={meshRef}>
        <cylinderGeometry args={[0.2, 0.2, 0.05]}></cylinderGeometry>
        <meshStandardMaterial color="yellow" />
      </mesh>
    </RigidBody>
  ) : null;
}
