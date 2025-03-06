import { GameStatus, useSession } from "../hooks/useSession";
import Chat from "./Chat";
import InGame from "./InGame";
import Lobby from "./Lobby";
import Prelobby from "./Prelobby";

export default function GameState() {
  console.log("GameState render");

  const { gameStatus } = useSession();

  return (
    <div className="flex min-h-screen flex-col items-center bg-linear-to-b from-blue-400 from-70% to-blue-500">
      {gameStatus === GameStatus.Prelobby && <Prelobby />}

      {(gameStatus === GameStatus.Lobby ||
        gameStatus === GameStatus.InGame) && <Chat />}

      {gameStatus === GameStatus.Lobby && <Lobby />}
      {gameStatus === GameStatus.InGame && <InGame />}
    </div>
  );
}
