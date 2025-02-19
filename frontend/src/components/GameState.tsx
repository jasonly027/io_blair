import { GameStatus, useSession } from "../hooks/useSession";
import Chat from "./Chat";
import InGame from "./InGame";
import Lobby from "./Lobby";
import Prelobby from "./Prelobby";

export default function GameState() {
  console.log("GameState render");

  const { gameStatus } = useSession();

  return (
    <div className="flex h-screen flex-col items-center bg-blue-400">
      {gameStatus === GameStatus.Prelobby && <Prelobby />}

      {(gameStatus === GameStatus.Lobby ||
        gameStatus === GameStatus.InGame) && <Chat />}

      {gameStatus === GameStatus.Lobby && <Lobby />}
      {gameStatus === GameStatus.InGame && <InGame />}
    </div>
  );
}
