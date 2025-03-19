import { RapierRigidBody } from "@react-three/rapier";
import useGame from "../../../hooks/useGame";
import {
  translate,
  type Coordinate,
  type TraversableKey,
} from "../../../lib/Maze";
import { Mesh } from "three";
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

const YOU_HEIGHT = 0.5;
const YOU_RADIUS = 0.2;
const Y_FALLING_THRESHOLD = -0.001;

export function You() {
  const { youCoord } = useGame();
  const initCoord = useRef(youCoord);

  const bodyRef = useRef<RapierRigidBody>(null);
  const meshRef = useRef<Mesh>(null);

  useYouControls(bodyRef, meshRef);

  return (
    <Character
      ref={bodyRef}
      height={YOU_HEIGHT}
      initialCoord={initCoord.current}
    >
      <mesh ref={meshRef}>
        <cylinderGeometry args={[YOU_RADIUS, YOU_RADIUS, YOU_HEIGHT]} />
        <meshStandardMaterial />
      </mesh>
    </Character>
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
  const { map, youCoord } = useGame();
  const initCoord = useRef(youCoord);

  const { moveBody, moving } = useBody(initCoord.current, bodyRef, meshRef);
  const { locked, moveDir } = useBodyLock(youCoord);

  useEffect(
    function listenForMoveKey() {
      const onKeyDown = ({ key }: KeyboardEvent) => {
        if (
          !(key in controlMap) ||
          locked ||
          moving() ||
          bodyRef.current === null ||
          bodyRef.current.linvel().y < Y_FALLING_THRESHOLD
        )
          return;

        const dir = controlMap[key as keyof typeof controlMap];
        moveDir(dir);

        const reset = shouldReset(map, youCoord, dir);
        moveBody(dir, reset);
      };

      window.addEventListener("keydown", onKeyDown);
      return () => window.removeEventListener("keydown", onKeyDown);
    },
    [bodyRef, locked, map, moveBody, moveDir, moving, youCoord],
  );
}

interface useMoveSyncValues {
  locked: boolean;
  moveDir: Dispatch<TraversableKey>;
}

function useBodyLock(youCoord: Readonly<Coordinate>): useMoveSyncValues {
  const [locked, setLocked] = useState(false);

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
