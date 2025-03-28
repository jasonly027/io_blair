import useGame, { GameStatus } from "../hooks/useGame";
import InGame from "./InGame/InGame";
import Lobby from "./Lobby/Lobby";
import Prelobby from "./Prelobby";
import Sound from "./Sound";

export default function GameState() {
  console.log("GameState render");

  const { gameStatus } = useGame();

  return (
    <div className="flex min-h-screen flex-col items-center bg-linear-to-b from-blue-400 from-70% to-blue-500">
      {gameStatus === GameStatus.Prelobby && <Prelobby />}
      {gameStatus === GameStatus.Lobby && <Lobby />}
      {(gameStatus === GameStatus.InGame ||
        gameStatus === GameStatus.GameDone) && <InGame />}
      <Sound />
    </div>
  );
}
