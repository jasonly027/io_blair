import { useMemo, useState, type Dispatch } from "react";
import type { GameCharacter } from "../../types/character";
import CharacterCard from "./CharacterCard";
import { a } from "@react-spring/web";
import useDynamicScale from "../../hooks/useDynamicScale";
import useGame from "../../hooks/useGame";
import { playClickSfx } from "../../lib/sounds";

type StrokeKind = "none" | "dashed" | "full";

interface CardStyle {
  character: GameCharacter;
  stroke: StrokeKind;
  strokeColor: string;
}

const characters = ["Io", "Blair"] as const satisfies GameCharacter[];

export default function CharacterSelect() {
  const { you, setYou, teammate } = useGame();

  const cardStyles = useMemo<CardStyle[]>(
    () =>
      characters.map((character) => {
        if (character === you.hover || character === you.confirm) {
          return {
            character,
            stroke: you.confirm !== null ? "full" : "dashed",
            strokeColor: "rgba(255,255,255,1)",
          };
        } else if (
          character === teammate.hover ||
          character === teammate.confirm
        ) {
          return {
            character,
            stroke: teammate.confirm !== null ? "full" : "dashed",
            strokeColor: "rgba(255,255,255,0.5)",
          };
        }
        return {
          character,
          stroke: "none",
          strokeColor: "white",
        };
      }),
    [you, teammate],
  );

  const [confirmed, setConfirmed] = useState(false);

  const onConfirmClick = () => {
    if (you.hover === null && !confirmed) return;
    const ok = setYou({
      type: "confirm",
      character: confirmed ? null : you.hover,
    });
    if (ok) setConfirmed((prev) => !prev);
  };

  return (
    <div className="flex flex-col items-center justify-center space-y-12">
      <div className="flex flex-col max-lg:space-y-8 min-lg:flex-row min-lg:space-x-16">
        {cardStyles.map(({ character, stroke, strokeColor }) => (
          <CharacterCard
            key={character}
            name={character}
            locked={confirmed || character === teammate.confirm}
            stroke={stroke}
            strokeColor={strokeColor}
            onClick={(character) => setYou({ type: "hover", character })}
          />
        ))}
      </div>
      <ConfirmCharacter confirmed={confirmed} onClick={onConfirmClick} />
    </div>
  );
}

interface ConfirmCharacterProps {
  confirmed: boolean;
  onClick?: Dispatch<void>;
}

const noOpOnClick = () => {};

function ConfirmCharacter({
  confirmed,
  onClick = noOpOnClick,
}: ConfirmCharacterProps) {
  const { scale, increaseScale, decreaseScale } = useDynamicScale(1.05);

  return (
    <a.svg
      width="224"
      height="106"
      viewBox="0 0 224 106"
      xmlns="http://www.w3.org/2000/svg"
      style={scale}
    >
      <g
        onClick={() => onClick()}
        onKeyDown={(e) => {
          if (e.key !== "Enter") return;
          e.stopPropagation();
          onClick();
        }}
        onMouseEnter={increaseScale}
        onMouseLeave={decreaseScale}
        onPointerDown={() => {
          playClickSfx();
          increaseScale();
        }}
        onPointerUp={decreaseScale}
        onPointerLeave={decreaseScale}
        tabIndex={0}
        className="cursor-pointer fill-emerald-400 hover:fill-emerald-500 focus:fill-emerald-500 active:fill-green-500"
      >
        <path
          d="M18.8912 15.3784L36.1614 12.3514L54.5579 5L84.9684 12.3514L127.018 15.3784L166.439 5L200.979 15.3784L208.863 37L216.372 55.1622L219 66.8378L208.863 88.027L176.2 92.7838L159.681 101L127.018 92.7838H57.9368L38.5 86L14.7614 88.027L5 55.1622L11.7579 37L18.8912 15.3784Z"
          className="stroke-white stroke-9"
        />
        <text
          x="50%"
          y="50%"
          textAnchor="middle"
          dy=".3em"
          className="fill-white text-3xl select-none"
        >
          {confirmed ? "Unconfirm" : "Confirm"}
        </text>
      </g>
    </a.svg>
  );
}
