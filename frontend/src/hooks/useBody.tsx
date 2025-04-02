import { easings, SpringValue, useSpring } from "@react-spring/three";
import useGame from "./useGame";
import { toMapUnits } from "../lib/map";
import { translate, type Coordinate, type TraversableKey } from "../lib/Maze";
import { useCallback, useEffect, useMemo, useRef, type RefObject } from "react";
import {
  DIST_TO_NEXT_CELL,
  DIST_TO_NEXT_GAP,
  GROUND_Y,
  OOB_Y,
} from "../components/InGame/Map";
import type { RapierRigidBody } from "@react-three/rapier";
import type { Mesh } from "three";
import useConnection from "./useConnection";
import type { GameConnectionListener } from "../lib/GameConnection";
import { playFallSfx } from "../lib/sounds";

export const SUCCESSFUL_MOVE_DURATION = 350;
export const UNSUCCESSFUL_MOVE_DURATION = SUCCESSFUL_MOVE_DURATION * 0.75;

const Y_FALLING_THRESHOLD = -0.001;

export type SprVals = {
  moving: SpringValue<boolean>;
  x: SpringValue<number>;
  z: SpringValue<number>;
};

export interface useSpringBodySyncValues {
  moveBody: (dir: TraversableKey, reset: boolean) => void;
  moving: () => boolean;
}

/**
 * Provides a controller for moving the character body
 * on the map.
 *
 * @param initialCoord
 * @param bodyRef
 * @param meshRef
 * @returns
 */
export default function useBody(
  initialCoord: Readonly<Coordinate>,
  bodyRef: RefObject<RapierRigidBody>,
  meshRef: RefObject<Mesh>,
): useSpringBodySyncValues {
  const { map } = useGame();

  const { addConnectionEventListener, removeConnectionEventListener } =
    useConnection();

  const currentCoord = useRef(toMapUnits(initialCoord));
  const [sprVals, sprCtrl] = useSpring(() => {
    const [x, z] = currentCoord.current;
    return {
      moving: false,
      x,
      z,

      config: {
        easing: easings.easeOutCubic,
      },
    };
  }, [currentCoord]);

  useEffect(
    function listenForInGameMaze() {
      const onInGameMaze: GameConnectionListener<"inGameMaze"> = ({
        start,
      }) => {
        currentCoord.current = toMapUnits(start);
        const [x, z] = currentCoord.current;
        sprCtrl.start({
          to: {
            x,
            z,
          },
          onRest() {
            bodyRef.current?.setTranslation(
              {
                x,
                y: bodyRef.current.translation().y,
                z,
              },
              true,
            );
          },
          immediate: true,
        });
      };

      addConnectionEventListener("inGameMaze", onInGameMaze);
      return () => removeConnectionEventListener("inGameMaze", onInGameMaze);
    },
    [
      addConnectionEventListener,
      removeConnectionEventListener,
      sprCtrl,
      bodyRef,
    ],
  );

  const moveBody = useCallback(
    (dir: TraversableKey, reset: boolean) => {
      const newCoord = translate(
        currentCoord.current,
        dir,
        reset ? DIST_TO_NEXT_GAP : DIST_TO_NEXT_CELL,
      );

      sprCtrl.start({
        from: {
          x: currentCoord.current[0],
          z: currentCoord.current[1],
        },
        to: {
          x: newCoord[0],
          z: newCoord[1],
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

          playFallSfx();
          setTimeout(() => {
            if (meshRef.current !== null) {
              meshRef.current.visible = false;
            }

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
          duration: reset
            ? UNSUCCESSFUL_MOVE_DURATION
            : SUCCESSFUL_MOVE_DURATION,
        },
      });

      currentCoord.current = reset ? toMapUnits(map.start) : newCoord;
    },
    [bodyRef, map.start, meshRef, sprCtrl],
  );

  const moving = useCallback(() => {
    return (
      sprVals.moving.get() ||
      bodyRef.current === null ||
      bodyRef.current.linvel().y < Y_FALLING_THRESHOLD
    );
  }, [sprVals, bodyRef]);

  return useMemo(
    () => ({
      moveBody,
      moving,
    }),
    [moveBody, moving],
  );
}
