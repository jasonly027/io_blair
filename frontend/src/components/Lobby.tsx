import { useSession } from "../hooks/useSession";
import Pregame from "./Pregame";

export default function Lobby() {
  return (
    <Pregame>
      <StatusBar />
    </Pregame>
  );
}

function StatusBar() {
  const { lobbyCode, playerCount } = useSession();

  const onLobbyCodeClick = () => {
    navigator.clipboard.writeText(lobbyCode);
  };

  return (
    <div className="flex w-full max-w-xl flex-col space-y-4 text-center select-none min-lg:flex-row min-lg:justify-between">
      <h2>
        Players:{" "}
        <span className="rounded-md border-2 border-white bg-emerald-500 px-1">
          {`${playerCount}`} out of 2
        </span>
      </h2>
      <h2 onClick={onLobbyCodeClick} className="cursor-pointer">
        Lobby Code: {lobbyCode}
      </h2>
    </div>
  );
}
