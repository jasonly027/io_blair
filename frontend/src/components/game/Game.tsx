import { GameStatus, SessionProvider, useSession } from "./hooks/useSession";
import InGame from "./components/InGame";
import Prelobby from "./components/Prelobby";
import Lobby from "./components/Lobby";

export default function Game() {
  return (
    <SessionProvider>
      <State />
    </SessionProvider>
  );
}

function State() {
  const { gameStatus } = useSession();

  return (
    <div className="h-screen">
      {gameStatus === GameStatus.Prelobby && <Prelobby />}
      {gameStatus === GameStatus.Lobby && <Lobby />}
      {gameStatus === GameStatus.InGame && <InGame />}
    </div>
  );
}
