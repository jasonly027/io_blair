import * as TH from "three";
import { useSession } from "../../hooks/useSession";
import { CuboidCollider, RigidBody } from "@react-three/rapier";
import type { MapUserData } from "../../lib/Map";

export const TILE_WIDTH = 1;
export const GAP_WIDTH = 0.75;
export const DIST_TO_NEXT_TILE = TILE_WIDTH + GAP_WIDTH;
export const DIST_TO_NEXT_GAP = DIST_TO_NEXT_TILE / 2;

export const GROUND_Y = 0;
export const OOB_Y = -10;

interface CoordinatesProps {
  x: number;
  y: number;
}

function Tile({ x, y }: CoordinatesProps) {
  return (
    <RigidBody position={[x, 0, y]} type="fixed">
      <mesh>
        <boxGeometry args={[TILE_WIDTH, TILE_WIDTH, TILE_WIDTH]} />
        <meshStandardMaterial color="green" side={TH.DoubleSide} />
      </mesh>
    </RigidBody>
  );
}

function Gap({ x, y }: CoordinatesProps) {
  return (
    <RigidBody position={[x, 0, y]} type="fixed">
      <mesh>
        <boxGeometry args={[GAP_WIDTH, TILE_WIDTH, GAP_WIDTH]} />
        <meshStandardMaterial color="lightblue" side={TH.DoubleSide} />
      </mesh>
    </RigidBody>
  );
}

function OobSensor() {
  return (
    <RigidBody userData={{ type: "oobSensor" } satisfies MapUserData}>
      <CuboidCollider args={[10, 0, 10]} position={[0, OOB_Y, 0]} sensor />
    </RigidBody>
  );
}

export default function Map() {
  console.log("Map render");
  const { maze } = useSession();

  return (
    <group position={[0, GROUND_Y - TILE_WIDTH / 2, 0]}>
      {maze.moves.flatMap((row, yIdx) =>
        row.flatMap((col, xIdx) => {
          const x = xIdx + xIdx * GAP_WIDTH;
          const y = yIdx + yIdx * GAP_WIDTH;

          return [
            <Tile key={`${x},${y}`} x={x} y={y} />,
            col.right && (
              <Gap key={`r${x},${y}`} x={x + DIST_TO_NEXT_GAP} y={y} />
            ),
            col.down && (
              <Gap key={`d${x},${y}`} x={x} y={y + DIST_TO_NEXT_GAP} />
            ),
          ];
        }),
      )}
      <OobSensor />
    </group>
  );
}
