import { a } from "@react-spring/web";
import { Helper } from "@react-three/drei";
import { Canvas } from "@react-three/fiber";
import { Physics } from "@react-three/rapier";
import { useEffect, useState, type ReactNode } from "react";
import { PointLightHelper } from "three";
import Map from "./Map";
import { Teammate, You } from "./Player/Player";
import Chat from "../Chat";
import useGame, { GameStatus } from "../../hooks/useGame";
import useDynamicScale from "../../hooks/useDynamicScale";
import Loading from "../Loading";
import useConnection from "../../hooks/useConnection";
import type { GameConnectionListener } from "../../lib/GameConnection";

export default function InGame() {
  return (
    <div className="z-0 h-screen w-screen">
      <Canvas
        tabIndex={-1}
        shadows
        className="bg-linear-to-t from-gray-200 to-sky-200 focus:outline-0"
      >
        <Background />

        <axesHelper />

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
  const { scale, increaseScale, decreaseScale } = useDynamicScale(1.05);

  return (
    <div
      tabIndex={0}
      onMouseEnter={increaseScale}
      onMouseLeave={decreaseScale}
      onMouseDown={decreaseScale}
      onMouseUp={increaseScale}
      className="group"
    >
      <a.svg
        style={scale}
        xmlns="http://www.w3.org/2000/svg"
        fill="none"
        viewBox="0 0 24 24"
        strokeWidth={2}
        stroke="currentColor"
        className="fixed bottom-5 left-5 z-20 size-12 rounded-lg bg-emerald-400 p-1 outline-3 outline-white group-focus-within:invisible hover:bg-emerald-500 active:bg-green-400 min-md:invisible"
      >
        <path
          strokeLinecap="round"
          strokeLinejoin="round"
          d="M7.5 8.25h9m-9 3H12m-9.75 1.51c0 1.6 1.123 2.994 2.707 3.227 1.129.166 2.27.293 3.423.379.35.026.67.21.865.501L12 21l2.755-4.133a1.14 1.14 0 0 1 .865-.501 48.172 48.172 0 0 0 3.423-.379c1.584-.233 2.707-1.626 2.707-3.228V6.741c0-1.602-1.123-2.995-2.707-3.228A48.394 48.394 0 0 0 12 3c-2.392 0-4.744.175-7.043.513C3.373 3.746 2.25 5.14 2.25 6.741v6.018Z"
        />
      </a.svg>
      <div className="fixed bottom-5 left-5 z-20 flex h-72 w-full max-w-[25rem] items-center justify-center group-focus-within:visible max-[426px]:left-3 max-md:invisible">
        <Chat />
      </div>
    </div>
  );
}

function Hud() {
  return (
    <>
      <section className="fixed top-5 right-5 flex flex-col items-center justify-center space-y-3 select-none min-md:flex-row min-md:space-y-0 min-md:space-x-6">
        <CoinsCounter />
        <MovesCounter />
        <TimeCounter />
      </section>
      <ChatContainer />
      <WinScreen />
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

function WinScreen() {
  const { gameStatus, gameDone } = useGame();

  const { newGame } = useConnection();

  const { scale, increaseScale, decreaseScale } = useDynamicScale(1.05);

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
                onMouseDown={decreaseScale}
                onMouseUp={increaseScale}
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
