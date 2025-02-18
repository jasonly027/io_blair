import {
  useSpring,
  SpringRef,
  SpringValue,
  easings,
} from "@react-spring/three";
import { useEffect, useRef, type RefObject } from "react";
import { useSession } from "../hooks/useSession";
import type Maze from "../lib/Maze";
import { DIST_TO_NEXT_GAP, DIST_TO_NEXT_TILE, GROUND_Y, OOB_Y } from "./Map";
import type { CellKey, Coordinates } from "../lib/Maze";
import {
  CoefficientCombineRule,
  RapierRigidBody,
  RigidBody,
  type CollisionPayload,
} from "@react-three/rapier";
import { Mesh } from "three";
import { isMapUserData } from "../lib/Map";

export const PLAYER_HEIGHT = 0.5;

type Triplet = [x: number, y: number, z: number];

function toMapPosition([x, y]: Coordinates): Triplet {
  return [
    y * DIST_TO_NEXT_TILE,
    GROUND_Y + PLAYER_HEIGHT / 2,
    x * DIST_TO_NEXT_TILE,
  ];
}

function toCoordinates(x: number, z: number): Coordinates {
  return [x / DIST_TO_NEXT_TILE, z / DIST_TO_NEXT_TILE];
}

interface SprValues {
  moving: SpringValue<boolean>;
  x: SpringValue<number>;
  z: SpringValue<number>;
}
type SprController = SpringRef<{
  moving: boolean;
  x: number;
  z: number;
}>;

interface PlayerData {
  vals: SprValues;
  ctrl: SprController;
  mesh: Mesh;
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
} as const satisfies Record<string, CellKey>;

type ControlKey = keyof typeof controlMap;

const isControlKey = (v: unknown): v is ControlKey =>
  Object.keys(controlMap).includes(v as ControlKey);

export default function Player() {
  console.log("Player render");
  const { startCoords } = useSession();
  const startPos = toMapPosition(startCoords);

  const body = useRef<RapierRigidBody>(null!);
  const mesh = useRef<Mesh>(null!);

  const moveDirection = useControls(startPos, body, mesh);

  useEffect(() => {
    const onKeyPress = ({ key }: KeyboardEvent) => {
      if (!isControlKey(key)) return;
      moveDirection(controlMap[key]);
    };
    window.addEventListener("keypress", onKeyPress);

    return () => window.removeEventListener("keypress", onKeyPress);
  }, [moveDirection]);

  return (
    <RigidBody
      ref={body}
      userData={
        {
          vals: null!,
          ctrl: null!,
          mesh: null!,
        } satisfies PlayerData
      }
      onIntersectionEnter={(ev) => onIntersectionEnter(ev, body, startPos)}
      position={startPos}
      lockRotations
      friction={0}
      frictionCombineRule={CoefficientCombineRule.Min}
    >
      <mesh ref={mesh}>
        <cylinderGeometry args={[0.2, 0.2, 0.5]} />
        <meshStandardMaterial />
      </mesh>
    </RigidBody>
  );
}

function onIntersectionEnter(
  { other: { rigidBody: other } }: CollisionPayload,
  body: RefObject<RapierRigidBody>,
  startPos: Triplet,
) {
  if (!isMapUserData(other?.userData) || other.userData.type !== "oobSensor") {
    return;
  }

  const { ctrl, mesh } = body.current.userData as PlayerData;
  const [x, y, z] = startPos;

  ctrl.start({
    to: {
      x,
      z,
    },
    onStart() {
      mesh.visible = false;
    },
    onResolve() {
      mesh.visible = true;
    },
    onChange() {
      body.current.setTranslation({ x, y: y + -OOB_Y, z }, true);
    },
    config: {
      duration: 0,
    },
  });
}

function useControls(
  startPos: Triplet,
  body: RefObject<RapierRigidBody>,
  mesh: RefObject<Mesh>,
) {
  const spring = useSpring(
    () => ({
      from: {
        moving: false,
        x: startPos[0],
        z: startPos[2],
      },

      onStart() {
        spring[1].set({
          moving: true,
        });
      },

      onRest() {
        spring[1].set({
          moving: false,
        });
      },

      config: {
        easing: easings.easeOutCubic,
      },
    }),
    [startPos],
  );

  useEffect(() => {
    const player = body.current.userData as PlayerData;
    player.mesh = mesh.current;
    player.vals = spring[0];
    player.ctrl = spring[1];
  }, [body, mesh, spring]);

  const { maze } = useSession();

  return (direction: CellKey) => moveBody(body, maze, direction);
}

type MoveUnit = -1 | 0 | 1;

interface Magnitudes {
  x: MoveUnit;
  z: MoveUnit;
}

const moveMap = {
  up: { x: 0, z: -1 },
  right: { x: 1, z: 0 },
  down: { x: 0, z: 1 },
  left: { x: -1, z: 0 },
} as const satisfies Record<CellKey, Magnitudes>;

const Y_FALLING_THRESHOLD = -0.001;

function moveBody(
  body: RefObject<RapierRigidBody>,
  maze: Maze,
  direction: CellKey,
) {
  const { vals, ctrl } = body.current.userData as PlayerData;
  if (vals.moving.get() || body.current.linvel().y < Y_FALLING_THRESHOLD) {
    return;
  }

  const coords = toCoordinates(vals.x.get(), vals.z.get());
  const safe = maze.getCell(coords)[direction];

  const distance = safe ? DIST_TO_NEXT_TILE : DIST_TO_NEXT_GAP;
  const { x: moveX, z: moveZ } = moveMap[direction];

  ctrl.start({
    to: {
      x: vals.x.get() + moveX * distance,
      z: vals.z.get() + moveZ * distance,
    },

    onChange({ value: { x, z } }) {
      body.current.setTranslation(
        {
          x,
          y: body.current.translation().y,
          z,
        },
        true,
      );
    },

    config: {
      duration: 350 * (safe ? 1 : 0.75),
    },
  });
}
