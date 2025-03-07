import { Slide, toast, ToastContainer } from "react-toastify";
import useGame from "../../hooks/useGame";
import Pregame from "../Pregame";
import { a } from "@react-spring/web";
import useDynamicScale from "../../hooks/useDynamicScale";
import CharacterSelect from "./CharacterSelect";

export default function Lobby() {
  return (
    <Pregame>
      <div className="space-y-8 rounded-lg p-8">
        <StatusBar />
        <CharacterSelect />
        <ChatSpaceOffset />
      </div>
    </Pregame>
  );
}

function StatusBar() {
  return (
    <div className="flex w-full max-w-3xl flex-col items-center space-x-4 text-center select-none max-lg:space-y-4 min-lg:flex-row min-lg:justify-between">
      <PlayerCount />
      <LobbyCode />
    </div>
  );
}

function PlayerCount() {
  const { playerCount } = useGame();

  return (
    <h2>
      Players:{" "}
      <span className="rounded-md border-2 border-white bg-emerald-500 px-1 text-nowrap">
        {`${playerCount}`} out of 2
      </span>
    </h2>
  );
}

function LobbyCode() {
  const { lobbyCode } = useGame();

  const saveToClipboard = () => {
    clipboardToast();
    navigator.clipboard.writeText(lobbyCode);
  };

  const { scale, increaseScale, decreaseScale } = useDynamicScale(1.05);

  return (
    <div className="flex h-fit flex-row flex-wrap items-center justify-center space-x-2">
      <h2>Lobby Code:</h2>
      <a.span
        onClick={saveToClipboard}
        onMouseEnter={increaseScale}
        onMouseLeave={decreaseScale}
        style={scale}
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

function ChatSpaceOffset() {
  return <div className="h-72 min-lg:h-0"></div>;
}
