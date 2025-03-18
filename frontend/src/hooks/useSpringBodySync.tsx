import { easings, SpringValue, useSpring } from "@react-spring/three";
import useGame from "./useGame";
import { toMapUnits } from "../lib/Map";
import { translate, type Coordinate, type TraversableKey } from "../lib/Maze";
import { useCallback, useMemo, type RefObject } from "react";
import {
  DIST_TO_NEXT_CELL,
  DIST_TO_NEXT_GAP,
  GROUND_Y,
  OOB_Y,
} from "../components/InGame/Map";
import type { RapierRigidBody } from "@react-three/rapier";
import type { Mesh } from "three";

export type SprVals = {
  moving: SpringValue<boolean>;
  x: SpringValue<number>;
  z: SpringValue<number>;
};

export interface useSpringBodySyncValues {
  moveBody: (dir: TraversableKey, reset: boolean) => void;
  sprVals: SprVals;
}

export default function useSpringBodySync(
  bodyRef: RefObject<RapierRigidBody>,
  meshRef: RefObject<Mesh>,
): useSpringBodySyncValues {
  const { map } = useGame();

  const [sprVals, sprCtrl] = useSpring(() => {
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

  const moveBody = useCallback(
    (dir: TraversableKey, reset: boolean) => {
      const currentCoord: Coordinate = [sprVals.x.get(), sprVals.z.get()];
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
    },
    [bodyRef, map.start, meshRef, sprCtrl, sprVals],
  );

  return useMemo(
    () => ({
      moveBody,
      sprVals,
    }),
    [moveBody, sprVals],
  );
}
