import {
  Environment,
  Helper,
  OrbitControls,
  PerspectiveCamera,
} from "@react-three/drei";
import { Canvas } from "@react-three/fiber";
import { Physics } from "@react-three/rapier";
import type { ReactNode } from "react";
import { PointLightHelper } from "three";
import Map from "./Map";
import { Teammate, You } from "./Player/Player";

export default function InGame() {
  return (
    <div className="h-screen w-screen">
      <Canvas className="bg-red-200">
        <Background />
        <Camera />
        <Lights />

        <axesHelper />

        <GamePhysics>
          <Map />
          <You />
          <Teammate />
        </GamePhysics>
      </Canvas>
    </div>
  );
}

function Background() {
  console.log("Background render");

  return <Environment background preset="lobby" />;
}

function Camera() {
  return (
    <>
      <PerspectiveCamera makeDefault position={[0, 5, 5]} />;
      <OrbitControls />
    </>
  );
}

function Lights() {
  return (
    <>
      <pointLight position={[-5, 10, 5]} decay={0}>
        <Helper type={PointLightHelper} />
      </pointLight>
      <ambientLight intensity={0.2} />
    </>
  );
}

function GamePhysics({ children }: { children: ReactNode }) {
  return <Physics debug>{children}</Physics>;
}
