import { RapierRigidBody } from "@react-three/rapier";
import useGame from "../../../hooks/useGame";
import {
  translate,
  type Coordinate,
  type TraversableKey,
} from "../../../lib/Maze";
import { Mesh, PerspectiveCamera as PCamera } from "three";
import {
  useCallback,
  useEffect,
  useMemo,
  useRef,
  useState,
  type Dispatch,
  type RefObject,
} from "react";
import type Maze from "../../../lib/Maze";
import { Character } from "./Character";
import useConnection from "../../../hooks/useConnection";
import type { GameConnectionListener } from "../../../lib/GameConnection";
import useBody from "../../../hooks/useBody";
import { PerspectiveCamera } from "@react-three/drei";
import { useFrame } from "@react-three/fiber";
import { gameFocused } from "../../../lib/game";

const YOU_HEIGHT = 0.5;
const YOU_RADIUS = 0.2;

export function You() {
  const { youCoord } = useGame();
  const initCoord = useRef(youCoord);

  const bodyRef = useRef<RapierRigidBody>(null);
  const meshRef = useRef<Mesh>(null);
  const cameraRef = useRef<PCamera>(null);

  useYouControls(bodyRef, meshRef);

  useFrame(() => {
    if (bodyRef.current === null || cameraRef.current === null) return;
    const { x, z } = bodyRef.current.translation();
    cameraRef.current.position.set(x, 5, z + 7);
    cameraRef.current.lookAt(x, 0, z);
  });

  return (
    <>
      <Character
        ref={bodyRef}
        height={YOU_HEIGHT}
        initialCoord={initCoord.current}
      >
        <mesh castShadow receiveShadow ref={meshRef}>
          <cylinderGeometry args={[YOU_RADIUS, YOU_RADIUS, YOU_HEIGHT]} />
          <meshStandardMaterial />
        </mesh>
      </Character>
      <PerspectiveCamera ref={cameraRef} makeDefault />
    </>
  );
}

const controlMap = {
  w: "up",
  d: "right",
  s: "down",
  a: "left",
  ArrowUp: "up",
  ArrowRight: "right",
  ArrowDown: "down",
  ArrowLeft: "left",
} as const satisfies Record<string, TraversableKey>;

function useYouControls(
  bodyRef: RefObject<RapierRigidBody>,
  meshRef: RefObject<Mesh>,
) {
  const { map, youCoord, incrementMovesCount, gameDone } = useGame();
  const { moveBody, moving } = useBody(youCoord, bodyRef, meshRef);
  const { locked, moveDir } = useBodyLock();

  useEffect(
    function listenForMoveKey() {
      const onKeyDown = ({ key }: KeyboardEvent) => {
        if (
          !(key in controlMap) ||
          !gameFocused() ||
          gameDone ||
          locked ||
          moving()
        )
          return;

        incrementMovesCount();

        const dir = controlMap[key as keyof typeof controlMap];
        moveDir(dir);

        const reset = shouldReset(map, youCoord, dir);
        moveBody(dir, reset);
      };

      window.addEventListener("keydown", onKeyDown);
      return () => window.removeEventListener("keydown", onKeyDown);
    },
    [gameDone, incrementMovesCount, locked, map, moveBody, moveDir, moving, youCoord],
  );
}

interface useBodyLockValues {
  locked: boolean;
  moveDir: Dispatch<TraversableKey>;
}

function useBodyLock(): useBodyLockValues {
  const [locked, setLocked] = useState(false);

  const { youCoord } = useGame();

  const {
    addConnectionEventListener,
    removeConnectionEventListener,
    characterMove,
  } = useConnection();

  useEffect(
    function listenForCharacterMove() {
      const onCharacterMove: GameConnectionListener<"characterMove"> = () => {
        setLocked(false);
      };

      addConnectionEventListener("characterMove", onCharacterMove);
      return () =>
        removeConnectionEventListener("characterMove", onCharacterMove);
    },
    [addConnectionEventListener, removeConnectionEventListener],
  );

  const moveDir = useCallback(
    (dir: TraversableKey) => {
      characterMove(translate(youCoord, dir));
      setLocked(true);
    },
    [characterMove, youCoord],
  );

  return useMemo(
    () => ({
      locked,
      moveDir,
    }),
    [locked, moveDir],
  );
}

function shouldReset(
  map: Maze,
  [x, z]: Readonly<Coordinate>,
  dir: TraversableKey,
): boolean {
  if (map.matrix[z]?.[x] !== undefined) {
    return !map.matrix[z][x][dir]("either");
  }
  return true;
}
