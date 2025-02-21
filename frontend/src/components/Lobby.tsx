import { Slide, toast, ToastContainer } from "react-toastify";
import { useSession } from "../hooks/useSession";
import Pregame from "./Pregame";
import { useSpring } from "@react-spring/three";
import { a } from "@react-spring/web";
import { Canvas } from "@react-three/fiber";
import { Helper, PerspectiveCamera } from "@react-three/drei";
import CatWaffle from "./CatWaffle";
import { PointLightHelper } from "three";
import { useReducer, useRef, type ReactNode } from "react";
import {
  playerColorMap,
  type GameCharacter,
  type GamePlayer,
} from "../types/character";

export default function Lobby() {
  return (
    <Pregame>
      <StatusBar />
      <CharacterSelect />
    </Pregame>
  );
}

function StatusBar() {
  return (
    <div className="flex w-full max-w-3xl flex-col space-y-4 text-center select-none min-lg:flex-row min-lg:justify-between">
      <PlayerCount />
      <LobbyCode />
    </div>
  );
}

function PlayerCount() {
  const { playerCount } = useSession();

  return (
    <h2>
      Players:{" "}
      <span className="rounded-md border-2 border-white bg-emerald-500 px-1">
        {`${playerCount}`} out of 2
      </span>
    </h2>
  );
}

function LobbyCode() {
  const { lobbyCode } = useSession();

  const saveToClipboard = () => {
    clipboardToast();
    navigator.clipboard.writeText(lobbyCode);
  };

  const [springs, api] = useSpring(() => ({
    from: {
      fontSize: 24,
    },
  }));
  const increaseScale = () => api.start({ to: { fontSize: 25 } });
  const decreaseScale = () => api.start({ to: { fontSize: 24 } });

  return (
    <div className="flex h-fit flex-row items-center justify-center space-x-2">
      <h2>Lobby Code:</h2>
      <a.span
        onClick={saveToClipboard}
        onMouseEnter={increaseScale}
        onMouseLeave={decreaseScale}
        style={springs}
        className="cursor-pointer rounded-md border-2 border-white bg-cyan-600 px-1"
      >
        {lobbyCode}
      </a.span>
      <ToastContainer newestOnTop />
    </div>
  );
}

function clipboardToast() {
  toast(<ClipboardToastComponent />, {
    position: "bottom-center",
    autoClose: 500,
    closeOnClick: true,
    hideProgressBar: true,
    closeButton: false,
    icon: false,
    pauseOnHover: true,
    draggable: false,
    transition: Slide,
  });
}

function ClipboardToastComponent() {
  return (
    <div className="flex flex-row items-center space-x-2 p-2">
      <h1 className="text-center text-lg">Copied to clipboard</h1>
      <svg
        xmlns="http://www.w3.org/2000/svg"
        viewBox="0 0 24 24"
        className="size-5 fill-none stroke-current stroke-2"
      >
        <path
          strokeLinecap="round"
          strokeLinejoin="round"
          d="M15.666 3.888A2.25 2.25 0 0 0 13.5 2.25h-3c-1.03 0-1.9.693-2.166 1.638m7.332 0c.055.194.084.4.084.612v0a.75.75 0 0 1-.75.75H9a.75.75 0 0 1-.75-.75v0c0-.212.03-.418.084-.612m7.332 0c.646.049 1.288.11 1.927.184 1.1.128 1.907 1.077 1.907 2.185V19.5a2.25 2.25 0 0 1-2.25 2.25H6.75A2.25 2.25 0 0 1 4.5 19.5V6.257c0-1.108.806-2.057 1.907-2.185a48.208 48.208 0 0 1 1.927-.184"
        />
      </svg>
    </div>
  );
}

type ContainerAction = "none" | "hover" | "confirm";

const borderStyleMap = {
  none: "none",
  hover: "dashed",
  confirm: "full",
} as const satisfies Record<ContainerAction, StrokeKind>;

type PlayerIntent = {
  [key in GamePlayer]: ContainerAction;
};

interface CharCardStyle extends PlayerIntent {
  character: GameCharacter;
  stroke: StrokeKind;
  strokeColor: string;
}

type CharCardStyles = CharCardStyle[];

interface CharCardStylesAction {
  intent: ContainerAction;
  caller: GamePlayer;
  character: GameCharacter;
}

function characterReducer(
  state: CharCardStyles,
  action: CharCardStylesAction,
): CharCardStyles {
  return state.map((s) => {
    const style = { ...s };

    // If this is the character the caller directly
    // wanted to update the intent on, change it to the new intent.
    // Otherwise, clear that caller's intent on this card because
    // each caller can only have up to one non-"none" intent on a
    // card.
    if (action.character === s.character) {
      style[action.caller] = action.intent;
    } else {
      style[action.caller] = "none";
    }

    // Determine whether to show the user's intent visualization
    // (dashed border for hover / full border for confirm) or their
    // teammates. Will show the user's unless theirs is "none".
    const dominantPlayer: GamePlayer =
      style.You !== "none" ? "You" : "Teammate";
    style.stroke = borderStyleMap[style[dominantPlayer]];
    style.strokeColor = playerColorMap[dominantPlayer];

    return style;
  });
}

const gameCharacters = ["Io", "Blair"] as const satisfies GameCharacter[];

function createDefaultCharacterStyles(
  characters: GameCharacter[],
): CharCardStyles {
  return characters.map((name) => ({
    character: name,
    stroke: "none",
    strokeColor: "",
    You: "none",
    Teammate: "none",
  }));
}

function CharacterSelect() {
  const [characterStyles, changeCharacterStyles] = useReducer(
    characterReducer,
    gameCharacters,
    createDefaultCharacterStyles,
  );

  return (
    <>
      <div className="m-4 flex flex-col max-lg:space-y-8 lg:flex-row lg:space-x-16">
        {characterStyles.map(({ character, stroke, strokeColor }) => (
          <CharacterCard
            key={character}
            name={character}
            stroke={stroke}
            strokeColor={strokeColor}
            onClick={(character) =>
              changeCharacterStyles({
                caller: "You",
                character,
                intent: "hover",
              })
            }
          />
        ))}
      </div>
    </>
  );
}

interface CharacterCardProps {
  name: GameCharacter;
  stroke: StrokeKind;
  strokeColor: string;
  onClick?: (name: GameCharacter) => void;
}

const noOpOnClick = () => {};

function CharacterCard({
  name,
  stroke,
  strokeColor,
  onClick = noOpOnClick,
}: CharacterCardProps) {
  return (
    <DashedContainer stroke={stroke} strokeColor={strokeColor}>
      <div
        onClick={() => onClick(name)}
        className="flex size-[20rem] flex-col rounded-md bg-orange-400"
      >
        <h2 className="m-2 text-center select-none">{name}</h2>
        <CharacterScene />
      </div>
    </DashedContainer>
  );
}

type StrokeKind = "none" | "dashed" | "full";

interface DashedBorderProps {
  stroke?: StrokeKind;
  strokeColor?: string;
  children: ReactNode;
}

function DashedContainer({
  stroke = "dashed",
  strokeColor = "white",
  children,
}: DashedBorderProps) {
  const divRef = useRef<HTMLDivElement>(null!);

  const dashedBorderSvg = `url("data:image/svg+xml,%3csvg width='100%25' height='100%25' xmlns='http://www.w3.org/2000/svg'%3e%3crect width='100%25' height='100%25' fill='none' stroke='${strokeColor}' stroke-width='15' stroke-dasharray='30' stroke-dashoffset='12' stroke-linecap='square'/%3e%3c/svg%3e")`;
  const outlineClasses = stroke === "full" ? "outline-8 -outline-offset-8" : "";

  return (
    <div
      ref={divRef}
      style={{
        backgroundImage: stroke === "dashed" ? dashedBorderSvg : "",
        outlineColor: strokeColor,
      }}
      className={`rounded-md p-2 ${outlineClasses}`}
    >
      {children}
    </div>
  );
}

function CharacterScene() {
  return (
    <div className="size-full">
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
