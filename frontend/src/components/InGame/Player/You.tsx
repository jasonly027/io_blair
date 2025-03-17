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
import {
  easings,
  SpringRef,
  SpringValue,
  useSpring,
} from "@react-spring/three";
import type Maze from "../../../lib/Maze";
import { DIST_TO_NEXT_CELL, DIST_TO_NEXT_GAP, GROUND_Y, OOB_Y } from "../Map";
import { Character } from "./Character";
import useConnection from "../../../hooks/useConnection";
import type { GameConnectionListener } from "../../../lib/GameConnection";
import { toMapUnits, toNonMapUnits } from "../../../lib/Map";

const YOU_HEIGHT = 0.5;
const YOU_RADIUS = 0.2;
const Y_FALLING_THRESHOLD = -0.001;

export function You() {
  const bodyRef = useRef<RapierRigidBody>(null);
  const meshRef = useRef<Mesh>(null);

  useYouControls(bodyRef, meshRef);

  return (
    <Character ref={bodyRef} height={YOU_HEIGHT}>
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

type SprVals = {
  x: SpringValue<number>;
  z: SpringValue<number>;
  moving: SpringValue<boolean>;
};

type SprCtrl = SpringRef<{
  x: number;
  z: number;
  moving: boolean;
}>;

type SprCoords = [SprVals, SprCtrl];

function useYouControls(
  bodyRef: RefObject<RapierRigidBody>,
  meshRef: RefObject<Mesh>,
) {
  const { map } = useGame();

  const sprCoords = useSpring(() => {
    const [x, z] = toMapUnits(map.start);
    return {
      moving: false,
      x,
      z,

      config: {
        easing: easings.easeOutCubic,
      },
    };
  }, [map.start]);

  const { locked, moveDir } = useMoveSync(sprCoords[0]);

  useEffect(
    function listenForMoveKey() {
      const onKeyDown = ({ key }: KeyboardEvent) => {
        if (
          !(key in controlMap) ||
          locked ||
          sprCoords[0].moving.get() ||
          bodyRef.current === null ||
          bodyRef.current.linvel().y < Y_FALLING_THRESHOLD
        )
          return;

        const dir = controlMap[key as keyof typeof controlMap];
        moveDir(dir);
        moveYou(bodyRef, meshRef, sprCoords, map, dir);
      };

      window.addEventListener("keydown", onKeyDown);
      return () => window.removeEventListener("keydown", onKeyDown);
    },
    [bodyRef, meshRef, sprCoords, map, locked, moveDir],
  );
}

interface useMoveSyncValues {
  locked: boolean;
  moveDir: Dispatch<TraversableKey>;
}

function useMoveSync(sprVals: SprVals): useMoveSyncValues {
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
      const coord = toNonMapUnits([sprVals.x.get(), sprVals.z.get()]);
      characterMove(translate(coord, dir));
      setLocked(true);
    },
    [characterMove, sprVals],
  );

  return useMemo(
    () => ({
      locked,
      moveDir,
    }),
    [locked, moveDir],
  );
}

function moveYou(
  bodyRef: RefObject<RapierRigidBody>,
  meshRef: RefObject<Mesh>,
  [sprVals, sprCtrl]: SprCoords,
  map: Maze,
  dir: TraversableKey,
) {
  const currentCoord: Coordinate = [sprVals.x.get(), sprVals.z.get()];
  const reset = shouldReset(map, currentCoord, dir);

  const [x, z] = translate(
    currentCoord,
    dir,
    reset ? DIST_TO_NEXT_GAP : DIST_TO_NEXT_CELL,
  );
  sprCtrl.start({
    to: {
      x,
      z,
    },

    onChange({ value: { x, z } }) {
      if (bodyRef.current === null) return;
      bodyRef.current.setTranslation(
        {
          x,
          y: bodyRef.current.translation().y,
          z,
        },
        true,
      );
    },

    onStart(_, ctrl) {
      ctrl.set({
        moving: true,
      });
    },

    onRest(_, ctrl) {
      if (!reset) {
        ctrl.set({
          moving: false,
        });
        return;
      }

      setTimeout(() => {
        const [x, z] = toMapUnits(map.start);
        ctrl.start({
          to: {
            moving: false,
            x,
            z,
          },

          onChange({ value: { x, z } }) {
            bodyRef.current?.setTranslation(
              { x, y: GROUND_Y + -OOB_Y, z },
              true,
            );
          },

          onStart() {
            if (meshRef.current === null) return;
            meshRef.current.visible = false;
          },

          onRest() {
            if (meshRef.current === null) return;
            meshRef.current.visible = true;
          },

          config: {
            duration: 0,
          },
        });
      }, 1000);
    },

    config: {
      duration: 350 * (reset ? 0.75 : 1),
    },
  });
}

function shouldReset(
  map: Maze,
  coord: Coordinate,
  dir: TraversableKey,
): boolean {
  const [x, z] = toNonMapUnits(coord);

  if (map.matrix[z]?.[x] !== undefined) {
    return !map.matrix[z][x][dir]("either");
  }
  return true;
}
