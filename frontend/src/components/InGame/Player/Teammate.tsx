import type { RapierRigidBody } from "@react-three/rapier";
import { useEffect, useRef, type RefObject } from "react";
import type { Mesh } from "three";
import { Character } from "./Character";
import useConnection from "../../../hooks/useConnection";
import type { GameConnectionListener } from "../../../lib/GameConnection";
import useSpringBodySync from "../../../hooks/useSpringBodySync";

const TEAMMATE_HEIGHT = 0.5;
const TEAMMATE_RADIUS = 0.2;

export function Teammate() {
  const bodyRef = useRef<RapierRigidBody>(null);
  const meshRef = useRef<Mesh>(null);

  useTeammateControls(bodyRef, meshRef);

  return (
    <Character ref={bodyRef} height={TEAMMATE_HEIGHT}>
      <mesh ref={meshRef}>
        <cylinderGeometry
          args={[TEAMMATE_RADIUS, TEAMMATE_RADIUS, TEAMMATE_HEIGHT]}
        />
        <meshStandardMaterial />
      </mesh>
    </Character>
  );
}

function useTeammateControls(
  bodyRef: RefObject<RapierRigidBody>,
  meshRef: RefObject<Mesh>,
) {
  const { moveBody } = useSpringBodySync(bodyRef, meshRef);

  const { addConnectionEventListener, removeConnectionEventListener } =
    useConnection();

  useEffect(
    function listenForOtherMove() {
      const onOtherMove: GameConnectionListener<"characterOtherMove"> = ({
        direction,
        reset,
      }) => {
        moveBody(direction, reset);
      };

      addConnectionEventListener("characterOtherMove", onOtherMove);
      return () =>
        removeConnectionEventListener("characterOtherMove", onOtherMove);
    },
    [moveBody, addConnectionEventListener, removeConnectionEventListener],
  );
}
