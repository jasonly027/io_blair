import { RapierRigidBody, RigidBody } from "@react-three/rapier";
import useGame from "../../../hooks/useGame";
import { Vector3 } from "three";
import { forwardRef, useMemo, type ForwardedRef, type ReactNode } from "react";
import { toMapUnits } from "../../../lib/Map";

interface CharacterProps {
  children: ReactNode;
  height: number;
}

export const Character = forwardRef(
  (
    { children, height }: CharacterProps,
    bodyRef: ForwardedRef<RapierRigidBody>,
  ) => {
    const { map } = useGame();

    const position = useMemo(() => {
      const [x, z] = toMapUnits(map.start);
      return new Vector3(x, height / 2, z);
    }, [map.start, height]);

    return (
      <RigidBody
        ref={bodyRef}
        position={position}
        lockRotations
        collisionGroups={(0x2 << 16) | 1}
      >
        {children}
      </RigidBody>
    );
  },
);
