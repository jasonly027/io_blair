import { useEffect, useRef, useState, type ReactNode } from "react";
import { GameStatus, useSession } from "../hooks/useSession";
import type { GameConnectionListener } from "../lib/GameConnection";

export default function Prelobby() {
  const [loading, setLoading] = useState(false);

  const {
    setGameStatus,
    addConnectionEventListener,
    removeConnectionEventListener,
  } = useSession();

  const onLobbyJoin: GameConnectionListener<"lobbyJoin"> = ({ success }) => {
    setTimeout(() => {
      if (!success) {
        setLoading(false);
        return;
      }
      setGameStatus(GameStatus.Lobby);
    }, 2000);
  };

  useEffect(() => {
    addConnectionEventListener("lobbyJoin", onLobbyJoin);
    return () => {
      removeConnectionEventListener("lobbyJoin", onLobbyJoin);
    };
  }, []);

  return (
    <div className="flex size-full flex-col items-center justify-center space-y-4 bg-blue-400">
      <Title>Io and Blair</Title>
      <div className="flex h-3/4 flex-col items-center justify-center space-y-4">
        {loading ? (
          <Loading />
        ) : (
          <>
            <Create setLoading={setLoading} />
            <span className="text-2xl font-medium text-white">or</span>
            <Join setLoading={setLoading} />
          </>
        )}
      </div>
    </div>
  );
}

function Title({ children }: { children: ReactNode }) {
  return (
    <h1 className="m-4 block text-6xl font-bold text-white drop-shadow-md">
      {children}
    </h1>
  );
}

interface SetLoadingProp {
  setLoading: React.Dispatch<React.SetStateAction<boolean>>;
}

function Create({ setLoading }: SetLoadingProp) {
  const { createLobby } = useSession();

  const onClick = () => {
    setLoading(true);
    createLobby();
  };

  return <Button onClick={onClick}>Create Lobby</Button>;
}

function Join({ setLoading }: SetLoadingProp) {
  const inputRef = useRef<HTMLInputElement>(null!);

  const { joinLobby } = useSession();

  const onClick = () => {
    setLoading(true);

    const code = inputRef.current.value;
    joinLobby(code);
  };

  return (
    <div className="flex flex-row-reverse">
      <button
        type="button"
        onClick={onClick}
        className="peer rounded-r-lg bg-emerald-600 p-2 text-2xl font-semibold text-white hover:bg-emerald-700"
      >
        <svg
          xmlns="http://www.w3.org/2000/svg"
          fill="none"
          viewBox="0 0 24 24"
          strokeWidth="1.5"
          stroke="currentColor"
          className="size-6"
        >
          <path
            strokeLinecap="round"
            strokeLinejoin="round"
            d="m8.25 4.5 7.5 7.5-7.5 7.5"
          />
        </svg>
      </button>
      <input
        ref={inputRef}
        placeholder="Join Code"
        className="w-[12ch] rounded-l-lg border-2 border-emerald-600 p-2 text-center text-2xl font-semibold focus:border-emerald-800 focus:outline-0 peer-hover:border-emerald-700"
        maxLength={8}
      />
    </div>
  );
}

function Loading() {
  const [ellipsis, setEllipsis] = useState("");

  useEffect(() => {
    const interval = setInterval(() => {
      setEllipsis((prev) => (prev.length < 3 ? prev + "." : "."));
    }, 500);

    return () => clearInterval(interval);
  }, []);

  return (
    <h2 className="text-2xl font-semibold text-white">
      Waiting for server{ellipsis}
    </h2>
  );
}

type ButtonProps = React.ButtonHTMLAttributes<HTMLButtonElement> & {
  children?: ReactNode;
};

function Button({ children, ...props }: ButtonProps) {
  return (
    <button
      type="button"
      className="rounded-lg border-2 bg-emerald-600 p-2 text-2xl font-semibold text-white hover:bg-emerald-700"
      {...props}
    >
      {children}
    </button>
  );
}
