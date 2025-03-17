import { RapierRigidBody, RigidBody } from "@react-three/rapier";
import useGame from "../../../hooks/useGame";
import { Vector3 } from "three";
import { forwardRef, type ForwardedRef, type ReactNode } from "react";
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
    const [x, z] = toMapUnits(map.start);

    return (
      <RigidBody
        ref={bodyRef}
        position={new Vector3(x, height / 2, z)}
        lockRotations
      >
        {children}
      </RigidBody>
    );
  },
);
