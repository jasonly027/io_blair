import {
  CoefficientCombineRule,
  RapierRigidBody,
  RigidBody,
} from "@react-three/rapier";
import { Vector3 } from "three";
import { forwardRef, useMemo, type ForwardedRef, type ReactNode } from "react";
import { toMapUnits } from "../../../lib/map";
import type { Coordinate } from "../../../lib/Maze";

interface CharacterProps {
  children: ReactNode;
  height: number;
  initialCoord: Readonly<Coordinate>;
}

export const Character = forwardRef(
  (
    { children, height, initialCoord }: CharacterProps,
    bodyRef: ForwardedRef<RapierRigidBody>,
  ) => {
    const pos = useMemo(() => {
      const [x, z] = toMapUnits(initialCoord);
      return new Vector3(x, height / 2, z);
    }, [initialCoord, height]);

    return (
      <RigidBody
        ref={bodyRef}
        position={pos}
        lockRotations
        collisionGroups={(0x2 << 16) | (0xffff & ~0x2)}
        friction={0}
        frictionCombineRule={CoefficientCombineRule.Min}
      >
        {children}
      </RigidBody>
    );
  },
);
