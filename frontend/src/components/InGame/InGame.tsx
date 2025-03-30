import { a } from "@react-spring/web";
import { Helper } from "@react-three/drei";
import { Canvas } from "@react-three/fiber";
import { Physics } from "@react-three/rapier";
import {
  useEffect,
  useState,
  type DispatchWithoutAction,
  type ReactNode,
} from "react";
import { PointLightHelper } from "three";
import Map from "./Map";
import { Teammate, You } from "./Player/Player";
import Chat from "../Chat";
import useGame, { GameStatus } from "../../hooks/useGame";
import useDynamicScale from "../../hooks/useDynamicScale";
import Loading from "../Loading";
import useConnection from "../../hooks/useConnection";
import type { GameConnectionListener } from "../../lib/GameConnection";
import { playClickSfx, SFX_VOLUME } from "../../lib/sounds";
import WIN from "/audio/win.mp3";

export default function InGame() {
  return (
    <div className="z-0 h-[100dvh] w-[100dvw]">
      <Canvas
        tabIndex={-1}
        shadows
        className="bg-linear-to-t from-gray-200 to-sky-200 focus:outline-0"
      >
        <Background />

        <GamePhysics>
          <Map />
          <You />
          <Teammate />
        </GamePhysics>
      </Canvas>
      <Hud />
    </div>
  );
}

function Background() {
  return (
    <>
      <pointLight
        castShadow
        position={[10, 7, 10]}
        intensity={2}
        decay={0}
        shadow-normalBias={0.02}
        shadow-mapSize-width={1024}
        shadow-mapSize-height={1024}
      >
        <Helper type={PointLightHelper} />
      </pointLight>
      <ambientLight intensity={0.8} />
    </>
  );
}

function GamePhysics({ children }: { children: ReactNode }) {
  return <Physics>{children}</Physics>;
}

function ChatContainer() {
  const { scale, increaseScale, decreaseScale } = useDynamicScale(1.05);

  return (
    <div tabIndex={0} className="group">
      <div className="fixed bottom-3 left-3 z-20 rounded-lg border-3 border-white bg-emerald-400 p-1 group-focus-within:invisible hover:bg-emerald-500 active:bg-green-400 min-md:invisible">
        <a.svg
          style={scale}
          onMouseEnter={increaseScale}
          onMouseLeave={decreaseScale}
          onPointerDown={() => {
            playClickSfx();
            increaseScale();
          }}
          onPointerUp={decreaseScale}
          onPointerLeave={decreaseScale}
          xmlns="http://www.w3.org/2000/svg"
          fill="none"
          viewBox="0 0 24 24"
          strokeWidth={2}
          stroke="currentColor"
          className="size-9"
        >
          <path
            strokeLinecap="round"
            strokeLinejoin="round"
            d="M7.5 8.25h9m-9 3H12m-9.75 1.51c0 1.6 1.123 2.994 2.707 3.227 1.129.166 2.27.293 3.423.379.35.026.67.21.865.501L12 21l2.755-4.133a1.14 1.14 0 0 1 .865-.501 48.172 48.172 0 0 0 3.423-.379c1.584-.233 2.707-1.626 2.707-3.228V6.741c0-1.602-1.123-2.995-2.707-3.228A48.394 48.394 0 0 0 12 3c-2.392 0-4.744.175-7.043.513C3.373 3.746 2.25 5.14 2.25 6.741v6.018Z"
          />
        </a.svg>
      </div>
      <div className="fixed bottom-3 left-3 z-20 flex h-72 w-full max-w-[min(25rem,92vw)] items-center justify-center group-focus-within:visible max-[426px]:left-3 max-md:invisible">
        <Chat />
      </div>
    </div>
  );
}

function Hud() {
  return (
    <>
      <section className="absolute top-3 right-3 flex flex-col items-center justify-center space-y-3 select-none min-md:flex-row min-md:space-y-0 min-md:space-x-6 [&>svg]:h-[8vh] [&>svg]:min-h-12 [&>svg]:w-fit">
        <CoinsCounter />
        <MovesCounter />
        <TimeCounter />
      </section>
      <ChatContainer />
      <WinScreen />
      <Controls />
    </>
  );
}

function MovesCounter() {
  const { movesCount } = useGame();

  const { scale, increaseScale, decreaseScale } = useDynamicScale(1.05);

  return (
    <a.svg
      style={scale}
      onMouseEnter={increaseScale}
      onMouseLeave={decreaseScale}
      onPointerDown={increaseScale}
      onPointerUp={decreaseScale}
      onPointerLeave={decreaseScale}
      width="150"
      height="61"
      viewBox="0 0 150 61"
      fill="none"
      xmlns="http://www.w3.org/2000/svg"
    >
      <title>Moves</title>
      <path
        d="M30.5 4L56.5611 8.84615L69 11L98 8L122.5 8.84615L141 14.5L146 38.5L138 50L119.5 54L85 52.5L66 50L56.5611 52.5L31.5 57.5L18.5 54L4 43.5L6.54676 13.0192L30.5 4Z"
        stroke="white"
        strokeWidth="6"
        strokeLinecap="round"
        className="fill-sky-400"
      />
      <path
        d="M21 30.5H44.5"
        stroke="white"
        strokeWidth="6"
        strokeLinecap="round"
      />
      <path
        d="M31.8333 42L44.5 30.5"
        stroke="white"
        strokeWidth="6"
        strokeLinecap="round"
      />
      <path
        d="M31.8333 19L44.5 30.5"
        stroke="white"
        strokeWidth="6"
        strokeLinecap="round"
      />
      <text
        x="63%"
        y="50%"
        textAnchor="middle"
        dy=".3em"
        className="fill-white font-[Courier_New] text-3xl select-none"
      >
        {movesCount.toString().padStart(4, "0")}
      </text>
    </a.svg>
  );
}

function TimeCounter() {
  const [time, setTime] = useState(0);

  const { addConnectionEventListener, removeConnectionEventListener } =
    useConnection();

  const { scale, increaseScale, decreaseScale } = useDynamicScale(1.05);

  useEffect(function periodicallyUpdateTime() {
    const interval = setInterval(() => setTime((prev) => prev + 1), 1000);
    return () => clearInterval(interval);
  }, []);

  useEffect(
    function listenForInGameMaze() {
      const onInGameMaze: GameConnectionListener<"inGameMaze"> = () => {
        setTime(0);
      };

      addConnectionEventListener("inGameMaze", onInGameMaze);
      return () => {
        removeConnectionEventListener("inGameMaze", onInGameMaze);
      };
    },
    [addConnectionEventListener, removeConnectionEventListener],
  );

  return (
    <a.svg
      style={scale}
      onMouseEnter={increaseScale}
      onMouseLeave={decreaseScale}
      onPointerDown={increaseScale}
      onPointerUp={decreaseScale}
      onPointerLeave={decreaseScale}
      width="150"
      height="66"
      viewBox="0 0 150 66"
      fill="none"
      xmlns="http://www.w3.org/2000/svg"
    >
      <title>Time</title>
      <path
        d="M31 3.5L56.5611 10.3462L69 12.5L98 9.5L122.5 10.3462L141 16L146 40L138 51.5L119.5 55.5L85 54H66L58.5 57.5L38.5 62L16.5 60.5L4 45L6.54676 14.5192L31 3.5Z"
        fill="#2BD39E"
        stroke="white"
        strokeWidth="6"
        strokeLinecap="round"
      />
      <path
        d="M34 27.5V33.5L38 37.5"
        stroke="white"
        strokeWidth="5"
        strokeLinecap="round"
      />
      <circle cx="34" cy="33" r="13.5" stroke="white" strokeWidth="5" />
      <text
        x="63%"
        y="50%"
        textAnchor="middle"
        dy=".3em"
        className="fill-white font-[Courier_New] text-3xl select-none"
      >
        {time.toString().padStart(4, "0")}
      </text>
    </a.svg>
  );
}

function CoinsCounter() {
  const { currentCoins, totalCoins } = useGame();

  const { scale, increaseScale, decreaseScale } = useDynamicScale(1.05);

  return (
    <a.svg
      style={scale}
      onMouseEnter={increaseScale}
      onMouseLeave={decreaseScale}
      onPointerDown={increaseScale}
      onPointerUp={decreaseScale}
      onPointerLeave={decreaseScale}
      width="168"
      height="67"
      viewBox="0 0 168 67"
      fill="none"
      xmlns="http://www.w3.org/2000/svg"
    >
      <title>Coins</title>
      <path
        d="M33.662 4L56.5 11.5L75.4085 13.0769L107.268 10.0513L137 8.5L159 15L164.5 44L151 56.4444L130.5 56.4444L102 58.5L72.1127 54.9316L57.5 56.4444L41.9014 63L17.7324 61.4872L4 45.8547L6.79785 15.1134L33.662 4Z"
        fill="#D3BD3C"
        stroke="white"
        strokeWidth="6"
        strokeLinecap="round"
      />
      <path
        d="M34 29V35V39.5"
        stroke="white"
        strokeWidth="5"
        strokeLinecap="round"
      />
      <circle cx="34" cy="34" r="13.5" stroke="white" strokeWidth="5" />
      <text
        x="63%"
        y="50%"
        textAnchor="middle"
        dy=".3em"
        className="fill-white font-[Courier_New] text-3xl select-none"
      >
        {currentCoins.toString().padStart(2, "0")}/
        {totalCoins.toString().padStart(2, "0")}
      </text>
    </a.svg>
  );
}

const winSfx = new Audio(WIN);
winSfx.volume = SFX_VOLUME;

function WinScreen() {
  const { gameStatus, gameDone } = useGame();

  const { newGame, addConnectionEventListener, removeConnectionEventListener } =
    useConnection();

  const { scale, increaseScale, decreaseScale } = useDynamicScale(1.05);

  useEffect(
    function listenForTransitionToGameDone() {
      const onGameDone: GameConnectionListener<"transitionToGameDone"> = () => {
        winSfx.play();
      };

      addConnectionEventListener("transitionToGameDone", onGameDone);
      return () =>
        removeConnectionEventListener("transitionToGameDone", onGameDone);
    },
    [addConnectionEventListener, removeConnectionEventListener],
  );

  return (
    gameDone && (
      <div className="fixed top-0 left-0 z-10 flex size-full flex-col items-center justify-center space-y-6 bg-black/50">
        {gameStatus === GameStatus.InGame && <Loading />}
        {gameStatus === GameStatus.GameDone && (
          <>
            <p className="text-center text-4xl select-none">You Won!</p>
            <a.svg
              width="279"
              height="109"
              viewBox="0 0 279 109"
              xmlns="http://www.w3.org/2000/svg"
              style={scale}
              className="w-60"
            >
              <g
                onClick={newGame}
                onMouseEnter={increaseScale}
                onMouseLeave={decreaseScale}
                onPointerDown={() => {
                  playClickSfx();
                  increaseScale();
                }}
                onPointerUp={decreaseScale}
                onPointerLeave={decreaseScale}
                className="cursor-pointer fill-emerald-400 hover:fill-emerald-500 focus:fill-emerald-500 active:fill-green-500"
              >
                <path
                  d="M12.5464 62.5488L6 37.5976L12.5464 24.7195L34 9.5L66.5 14.6585H108.872L157.97 5L196.781 14.6585L249.152 11.439L273 30.7561L263.18 58.122L273 80.6585L249.152 104L203.5 95L126.641 104L78.5 95L35 99L6 89.1098L12.5464 62.5488Z"
                  className="stroke-white stroke-9"
                ></path>
                <text
                  x="50%"
                  y="50%"
                  textAnchor="middle"
                  dy=".3em"
                  className="fill-white text-3xl select-none"
                >
                  Play Again
                </text>
              </g>
            </a.svg>
          </>
        )}
      </div>
    )
  );
}

function dispatchKeyEvent(key: string) {
  document.dispatchEvent(new KeyboardEvent("keydown", { key, bubbles: true }));
}

function Controls() {
  const onUp = () => dispatchKeyEvent("w");
  const onRight = () => dispatchKeyEvent("d");
  const onDown = () => dispatchKeyEvent("s");
  const onLeft = () => dispatchKeyEvent("a");

  return (
    <>
      <svg
        width="160"
        height="158"
        viewBox="0 0 160 158"
        fill="none"
        xmlns="http://www.w3.org/2000/svg"
        className="fixed right-3 bottom-3 size-[min(30vh,50vw)] max-h-40 max-w-40"
      >
        <Arrow
          onClick={onUp}
          origin="50% 20%"
          d="M82.6651 8.75L102.151 42.5C103.113 44.1667 101.91 46.25 99.9856 46.25H61.0144C59.0899 46.25 57.8871 44.1667 58.8494 42.5L78.3349 8.75001C79.2972 7.08333 81.7028 7.08334 82.6651 8.75Z"
        />
        <Arrow
          onClick={onRight}
          origin="80% 50%"
          d="M151.25 80.6651L117.5 100.151C115.833 101.113 113.75 99.9101 113.75 97.9856L113.75 59.0144C113.75 57.0899 115.833 55.8871 117.5 56.8494L151.25 76.3349C152.917 77.2972 152.917 79.7028 151.25 80.6651Z"
        />
        <Arrow
          onClick={onDown}
          origin="50% 80%"
          d="M78.3349 149.25L58.8494 115.5C57.8871 113.833 59.0899 111.75 61.0144 111.75L99.9856 111.75C101.91 111.75 103.113 113.833 102.151 115.5L82.6651 149.25C81.7028 150.917 79.2972 150.917 78.3349 149.25Z"
        />
        <Arrow
          onClick={onLeft}
          origin="20% 50%"
          d="M8.75 76.3349L42.5 56.8494C44.1667 55.8871 46.25 57.0899 46.25 59.0144L46.25 97.9856C46.25 99.9101 44.1667 101.113 42.5 100.151L8.75 80.6651C7.08333 79.7028 7.08334 77.2972 8.75 76.3349Z"
        />
      </svg>
    </>
  );
}

function Arrow({
  d,
  onClick,
  origin,
}: {
  d: string;
  onClick: DispatchWithoutAction;
  origin: string;
}) {
  const { scale, increaseScale, decreaseScale } = useDynamicScale(1.1);

  return (
    <a.path
      onClick={onClick}
      onMouseEnter={increaseScale}
      onMouseLeave={decreaseScale}
      onPointerDown={increaseScale}
      onPointerUp={decreaseScale}
      onPointerLeave={decreaseScale}
      style={{ ...scale, transformOrigin: origin }}
      d={d}
      stroke="white"
      strokeWidth="5"
      className="cursor-pointer fill-emerald-400 hover:fill-emerald-500 active:fill-green-500"
    />
  );
}
