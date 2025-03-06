import {
  useCallback,
  useEffect,
  useMemo,
  useReducer,
  useState,
  type Dispatch,
} from "react";
import type { GameCharacter, GamePlayer } from "../types/character";
import CharacterCard from "./CharacterCard";
import { useSession } from "../hooks/useSession";
import { a } from "@react-spring/web";
import useDynamicScale from "../hooks/useDynamicScale";
import type { GameConnectionListener } from "../lib/GameConnection";

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

    if (newStyle.You !== "none") {
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

  const { hover, confirm, unConfirm } = useYouCharacterSelecter(changeStyles);

  useTeammateCharacterSelecter(changeStyles);

  const [confirmed, setConfirmed] = useState(false);

  const onCardClick = (character: GameCharacter) => {
    if (confirmed) return;
    hover(character);
  };

  const onConfirmClick = () => {
    if (!confirmed) {
      confirm();
    } else {
      unConfirm();
    }
    setConfirmed((prev) => !prev);
  };

  return (
    <div className="flex flex-col items-center justify-center space-y-12">
      <div className="flex flex-col max-lg:space-y-8 min-lg:flex-row min-lg:space-x-16">
        {styles.map(({ character, stroke, strokeColor }) => (
          <CharacterCard
            key={character}
            name={character}
            locked={confirmed}
            stroke={stroke}
            strokeColor={strokeColor}
            onClick={onCardClick}
          />
        ))}
      </div>
      <ConfirmCharacter confirmed={confirmed} onClick={onConfirmClick} />
    </div>
  );
}

interface youCharacterSelecterValues {
  hover: Dispatch<GameCharacter>;
  confirm: Dispatch<void>;
  unConfirm: Dispatch<void>;
}

function useYouCharacterSelecter(
  changeStyles: Dispatch<StyleAction>,
): youCharacterSelecterValues {
  const [hovered, setHovered] = useState<GameCharacter | null>(null);

  const { you, setYou, teammate, hoverCharacter, confirmCharacter } =
    useSession();

  const hover = useCallback(
    (character: GameCharacter) => {
      if (character === teammate) return;

      hoverCharacter(character);
      setHovered(character);
      changeStyles({ character, You: "dashed" });
    },
    [teammate, hoverCharacter, changeStyles],
  );

  const confirm = useCallback(() => {
    if (hovered === null) return;

    confirmCharacter(hovered);
    setYou(hovered);
    changeStyles({ character: hovered, You: "full" });
  }, [hovered, confirmCharacter, setYou, changeStyles]);

  const unConfirm = useCallback(() => {
    if (you === null) return;

    confirmCharacter(null);
    changeStyles({ character: you, You: "dashed" });
    setYou(null);
  }, [you, confirmCharacter, changeStyles, setYou]);

  return useMemo(
    () => ({
      hover,
      confirm,
      unConfirm,
    }),
    [hover, confirm, unConfirm],
  );
}

function useTeammateCharacterSelecter(changeStyles: Dispatch<StyleAction>) {
  const {
    you,
    setYou,
    setTeammate,
    addConnectionEventListener,
    removeConnectionEventListener,
  } = useSession();

  useEffect(
    function listenForHover() {
      const onHover: GameConnectionListener<"characterHover"> = ({
        character,
      }) => {
        console.log("heard other hover");
        
        changeStyles({ character, Teammate: "dashed" });
      };
      addConnectionEventListener("characterHover", onHover);

      return () => {
        removeConnectionEventListener("characterHover", onHover);
      };
    },
    [addConnectionEventListener, removeConnectionEventListener, changeStyles],
  );

  useEffect(
    function listenForConfirm() {
      const onConfirm: GameConnectionListener<"characterConfirm"> = ({
        character,
      }) => {
        setTeammate(character);
        console.log("heard confirm");

        changeStyles({
          character: character ?? "Io",
          ...(character === you ? { You: "none" } : {}),
          Teammate: character === null ? "none" : "full",
        });

        if (character === you) {
          setYou(null);
        }
      };
      addConnectionEventListener("characterConfirm", onConfirm);

      return () => {
        removeConnectionEventListener("characterConfirm", onConfirm);
      };
    },
    [
      addConnectionEventListener,
      changeStyles,
      removeConnectionEventListener,
      setTeammate,
      setYou,
      you,
    ],
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
