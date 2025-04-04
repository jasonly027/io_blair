import useGame, { GameStatus } from "../hooks/useGame";
import InGame from "./InGame/InGame";
import Lobby from "./Lobby/Lobby";
import Prelobby from "./Prelobby";
import Sound from "./Sound";

export default function GameState() {
  const { gameStatus } = useGame();

  return (
    <div className="flex min-h-[100dvh] flex-col items-center ">
      {gameStatus === GameStatus.Prelobby && <Prelobby />}
      {gameStatus === GameStatus.Lobby && <Lobby />}
      {(gameStatus === GameStatus.InGame ||
        gameStatus === GameStatus.GameDone) && <InGame />}
      <Sound />
    </div>
  );
}
