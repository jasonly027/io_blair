import { Helper } from "@react-three/drei";
import { Canvas } from "@react-three/fiber";
import { Physics } from "@react-three/rapier";
import { type ReactNode } from "react";
import { PointLightHelper } from "three";
import Map from "./Map";
import { Teammate, You } from "./Player/Player";
import Chat from "../Chat";

export default function InGame() {
  return (
    <div className="h-screen w-screen">
      <Canvas shadows className="bg-linear-to-t from-gray-200 to-sky-200">
        <Background />

        <axesHelper />

        <GamePhysics>
          <Map />
          <You />
          <Teammate />
        </GamePhysics>
      </Canvas>
      <ChatContainer />
    </div>
  );
}

function Background() {
  console.log("Background render");

  return (
    <>
      <pointLight castShadow position={[10, 7, 10]} intensity={3} decay={0}>
        <Helper type={PointLightHelper} />
      </pointLight>
      <ambientLight intensity={1} />
    </>
  );
}

function GamePhysics({ children }: { children: ReactNode }) {
  return <Physics debug>{children}</Physics>;
}

function ChatContainer() {
  return (
    <div tabIndex={0} className="group">
      <svg
        xmlns="http://www.w3.org/2000/svg"
        fill="none"
        viewBox="0 0 24 24"
        strokeWidth={2}
        stroke="currentColor"
        className="fixed bottom-5 left-5 size-12 rounded-lg bg-emerald-400 p-1 group-focus-within:invisible min-md:invisible"
      >
        <path
          strokeLinecap="round"
          strokeLinejoin="round"
          d="M7.5 8.25h9m-9 3H12m-9.75 1.51c0 1.6 1.123 2.994 2.707 3.227 1.129.166 2.27.293 3.423.379.35.026.67.21.865.501L12 21l2.755-4.133a1.14 1.14 0 0 1 .865-.501 48.172 48.172 0 0 0 3.423-.379c1.584-.233 2.707-1.626 2.707-3.228V6.741c0-1.602-1.123-2.995-2.707-3.228A48.394 48.394 0 0 0 12 3c-2.392 0-4.744.175-7.043.513C3.373 3.746 2.25 5.14 2.25 6.741v6.018Z"
        />
      </svg>
      <div className="fixed bottom-5 left-5 flex h-72 w-full max-w-[25rem] items-center justify-center group-focus-within:visible max-[426px]:left-3 max-md:invisible">
        <Chat />
      </div>
    </div>
  );
}
