import {
  useReducer,
  type Dispatch,
} from "react";
import type { GameCharacter, GamePlayer } from "../../types/character";
import CharacterCard from "./CharacterCard";
import { a } from "@react-spring/web";
import useDynamicScale from "../../hooks/useDynamicScale";

const characters = ["Io", "Blair"] as const satisfies GameCharacter[];

type StrokeKind = "none" | "dashed" | "full";

type PlayerStrokes = {
  [key in GamePlayer]: StrokeKind;
};

interface CardStyle extends PlayerStrokes {
  character: GameCharacter;
  stroke: StrokeKind;
  strokeColor: string;
}

function createCardStyles(characters: GameCharacter[]): CardStyle[] {
  return characters.map((character) => ({
    character,
    You: "none",
    Teammate: "none",
    stroke: "none",
    strokeColor: "white",
  }));
}

type SomePlayerStrokes = {
  [key in GamePlayer]?: StrokeKind;
};

interface StyleAction extends SomePlayerStrokes {
  character: GameCharacter;
}

function styleReducer(styles: CardStyle[], action: StyleAction): CardStyle[] {
  return styles.map((style) => {
    const isTarget = action.character == style.character;
    const newStyle = { ...style };

    if (action.You !== undefined) {
      newStyle.You = isTarget ? action.You : "none";
    }
    if (action.Teammate !== undefined) {
      newStyle.Teammate = isTarget ? action.Teammate : "none";
    }

    const showYouStyle =
      newStyle.Teammate !== "full" && newStyle.You !== "none";
    if (showYouStyle) {
      newStyle.stroke = newStyle.You;
      newStyle.strokeColor = "rgba(255, 255, 255, 1)";
    } else {
      newStyle.stroke = newStyle.Teammate;
      newStyle.strokeColor = "rgba(255, 255, 255, 0.5)";
    }

    return newStyle;
  });
}

export default function CharacterSelect() {
  const [styles, changeStyles] = useReducer(
    styleReducer,
    characters,
    createCardStyles,
  );

  return (
    <div className="flex flex-col items-center justify-center space-y-12">
      <div className="flex flex-col max-lg:space-y-8 min-lg:flex-row min-lg:space-x-16">
        {styles.map(({ character, stroke, strokeColor }) => (
          <CharacterCard
            key={character}
            name={character}
            locked={false}
            stroke={stroke}
            strokeColor={strokeColor}
          />
        ))}
      </div>
      <ConfirmCharacter confirmed={false} />
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
        onMouseDown={decreaseScale}
        onMouseUp={increaseScale}
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
