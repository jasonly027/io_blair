import type { RapierRigidBody } from "@react-three/rapier";
import { useEffect, useRef, type RefObject } from "react";
import type { Mesh } from "three";
import { Character } from "./Character";
import useConnection from "../../../hooks/useConnection";
import type { GameConnectionListener } from "../../../lib/GameConnection";
import useBody from "../../../hooks/useBody";
import useGame from "../../../hooks/useGame";

const TEAMMATE_HEIGHT = 0.5;
const TEAMMATE_RADIUS = 0.2;

export function Teammate() {
  const { teammateCoord } = useGame();
  const initCoord = useRef(teammateCoord());

  const bodyRef = useRef<RapierRigidBody>(null);
  const meshRef = useRef<Mesh>(null);

  useTeammateControls(bodyRef, meshRef);

  return (
    <Character
      ref={bodyRef}
      height={TEAMMATE_HEIGHT}
      initialCoord={initCoord.current}
    >
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
  const { teammateCoord } = useGame();
  const initCoord = useRef(teammateCoord());

  const { moveBody } = useBody(initCoord.current, bodyRef, meshRef);

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
