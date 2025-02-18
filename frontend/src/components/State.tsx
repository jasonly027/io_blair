import { GameStatus, useSession } from "../hooks/useSession";
import InGame from "./InGame";
import Lobby from "./Lobby";
import Prelobby from "./Prelobby";

export default function State() {
  console.log("State render");

  const { gameStatus } = useSession();

  return (
    <div className="flex h-screen flex-col items-center bg-blue-400">
      {gameStatus === GameStatus.Prelobby && <Prelobby />}
      {gameStatus === GameStatus.Lobby && <Lobby />}
      {gameStatus === GameStatus.InGame && <InGame />}
    </div>
  );
}
