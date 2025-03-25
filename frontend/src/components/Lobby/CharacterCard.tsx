import { Helper, PerspectiveCamera } from "@react-three/drei";
import { Canvas } from "@react-three/fiber";
import { type ReactNode } from "react";
import { PointLightHelper } from "three";
import type { GameCharacter } from "../../types/character";
import CatWaffle from "./CatWaffle";
import { a, useSpring } from "@react-spring/web";

export interface CharacterCardProps {
  name: GameCharacter;
  stroke: StrokeKind;
  strokeColor: string;
  locked: boolean;
  /**
   * Does not call handler if card is locked.
   */
  onClick?: (name: GameCharacter) => void;
}

const noOpOnClick = () => {};

export default function CharacterCard({
  name,
  stroke,
  strokeColor,
  locked,
  onClick = noOpOnClick,
}: CharacterCardProps) {
  return (
    <DashedContainer stroke={stroke} strokeColor={strokeColor}>
      <div
        onClick={() => {
          if (!locked) onClick(name);
        }}
        style={{
          cursor: locked ? "not-allowed" : "pointer",
        }}
        title={`Select ${name}`}
        className="flex size-[20rem] cursor-pointer flex-col rounded-md"
      >
        <h2 className="m-2 text-center select-none">{name}</h2>
        <CharacterScene />
      </div>
    </DashedContainer>
  );
}

export type StrokeKind = "none" | "dashed" | "full";

export interface DashedBorderProps {
  stroke?: StrokeKind;
  strokeColor?: string;
  children: ReactNode;
}

function DashedContainer({
  stroke = "dashed",
  strokeColor = "white",
  children,
}: DashedBorderProps) {
  const dashedBorderSvg = `url("data:image/svg+xml,%3csvg width='100%25' height='100%25' xmlns='http://www.w3.org/2000/svg'%3e%3crect width='100%25' height='100%25' fill='none' stroke='${strokeColor}' stroke-width='15' stroke-dasharray='30' stroke-dashoffset='12' stroke-linecap='square'/%3e%3c/svg%3e")`;
  const outlineClasses = stroke === "full" ? "outline-8 -outline-offset-8" : "";

  const scale = useSpring({
    scale: stroke !== "none" ? 1.02 : 1,
  });
  const invScale = useSpring({
    scale: stroke !== "none" ? 1 / 1.02 : 1,
  });

  return (
    <a.div
      style={{
        ...scale,
        backgroundImage: stroke === "dashed" ? dashedBorderSvg : "",
        outlineColor: strokeColor,
      }}
      className={`rounded-md p-2 ${outlineClasses}`}
    >
      <a.div style={invScale}>{children}</a.div>
    </a.div>
  );
}

function CharacterScene() {
  return (
    <div className="size-full scale-none">
      <Canvas>
        <ambientLight intensity={0.3} />
        <pointLight intensity={3} position={[20, 40, 100]} decay={0}>
          <Helper type={PointLightHelper} />
        </pointLight>
        <PerspectiveCamera makeDefault position={[11, 30, 100]} fov={100} />
        {/* <OrbitControls /> */}
        {/* <axesHelper args={[500]} /> */}
        <CatWaffle />
      </Canvas>
    </div>
  );
}
