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
        {cell.right("either") && <Gap type="right" cell={cell} />}
        {cell.down("either") && <Gap type="down" cell={cell} />}
        <Coin exists={cell.coin()} />
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
    <group position={[x, 0, z]}>
      <RigidBody type="fixed">
        <mesh receiveShadow castShadow>
          <boxGeometry args={[CELL_LEN, CELL_LEN, CELL_LEN]} />
          <meshStandardMaterial color="darkgreen" />
        </mesh>
      </RigidBody>
      {children}
    </group>
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
            cellDir("both") ? "white" : cellDir("You") ? "lightgreen" : "pink"
          }
          opacity={cellDir("both") ? 1 : 0.5}
          transparent
        />
      </mesh>
    </RigidBody>
  );
}

function Coin({ exists }: { exists: boolean }) {
  const [prevExists, setPrevExists] = useState(exists);

  const meshRef = useRef<Mesh>(null);
  useFrame((_, delta) => {
    if (meshRef.current === null) return;
    meshRef.current.rotateZ(delta * 1.5);
  });

  const timeoutRef = useRef<number | null>(null);

  if (exists !== prevExists) {
    if (exists) {
      setPrevExists(true);
      if (timeoutRef.current !== null) clearTimeout(timeoutRef.current);
    } else {
      timeoutRef.current = setTimeout(() => {
        setPrevExists(false);
      }, SUCCESSFUL_MOVE_DURATION);
    }
  }

  useEffect(function clearTimeoutOnDismount() {
    return () => {
      if (timeoutRef.current === null) return;
      clearTimeout(timeoutRef.current);
    };
  }, []);

  return (
    prevExists && (
      <RigidBody
        position={[0, CELL_LEN, 0]}
        rotation={[-Math.PI * 1.5, 0, 0]}
        type="fixed"
        colliders={false}
      >
        <CuboidCollider
          sensor
          args={[0.2, 0.2, 0.2]}
          onIntersectionEnter={() => {
            setPrevExists(false);
          }}
        />
        <mesh castShadow receiveShadow ref={meshRef}>
          <cylinderGeometry args={[0.2, 0.2, 0.05]}></cylinderGeometry>
          <meshStandardMaterial color="yellow" />
        </mesh>
      </RigidBody>
    )
  );
}
