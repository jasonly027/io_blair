import { Slide, toast, ToastContainer } from "react-toastify";
import useGame from "../../hooks/useGame";
import Pregame from "../Pregame";
import { a } from "@react-spring/web";
import useDynamicScale from "../../hooks/useDynamicScale";
import CharacterSelect from "./CharacterSelect";
import Chat from "../Chat";
import { playClickSfx } from "../../lib/sounds";

export default function Lobby() {
  return (
    <Pregame>
      <div className="flex w-full flex-col items-center justify-center space-y-8 rounded-lg p-8">
        <StatusBar />
        <CharacterSelect />
        <ChatContainer />
      </div>
    </Pregame>
  );
}

function StatusBar() {
  return (
    <div className="flex w-full max-w-2xl flex-col items-center space-x-4 text-center select-none max-lg:space-y-4 min-lg:flex-row min-lg:justify-between">
      <PlayerCount />
      <LobbyCode />
    </div>
  );
}

function PlayerCount() {
  const { playerCount } = useGame();

  const { scale, increaseScale, decreaseScale } = useDynamicScale(1.05);

  return (
    <div className="flex flex-row flex-wrap items-center justify-center space-x-2">
      <h2>Players: </h2>
      <a.span
        style={scale}
        onMouseEnter={increaseScale}
        onMouseLeave={decreaseScale}
        className="rounded-md border-2 border-white bg-emerald-500 px-1 text-nowrap"
      >
        {`${playerCount}`} out of 2
      </a.span>
    </div>
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
    <div className="flex flex-row flex-wrap items-center justify-center space-x-2">
      <h2>Lobby Code:</h2>
      <a.button
        onClick={saveToClipboard}
        onMouseDown={playClickSfx}
        onMouseEnter={increaseScale}
        onMouseLeave={decreaseScale}
        style={scale}
        title="Copy to clipboard"
        className="cursor-pointer rounded-md border-2 border-white bg-cyan-600 px-1"
      >
        {lobbyCode}
      </a.button>
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

function ChatContainer() {
  return (
    <div className="bottom-5 left-5 flex h-72 w-full max-w-[25rem] items-center justify-center min-2xl:fixed">
      <Chat />
    </div>
  );
}
