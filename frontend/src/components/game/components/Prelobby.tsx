import { type ReactNode } from "react";
import { useSession } from "../hooks/useSession";

export default function Prelobby() {
  return (
    <div className="flex size-full flex-col items-center justify-center space-y-4 bg-blue-400">
      <Title>Io and Blair</Title>
      <Create />
      <span className="text-2xl font-medium text-white">or</span>
      <Join />
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

function Create() {
  const { createLobby } = useSession();

  return <Button onClick={createLobby}>Create Lobby</Button>;
}

function Join() {
  // const { joinLobby } = useSession();

  return (
    <div className="flex flex-row-reverse">
      <button className="peer rounded-r-lg bg-emerald-600 p-2 text-2xl font-semibold text-white hover:bg-emerald-700">
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
        placeholder="Join Code"
        className="w-[12ch] rounded-l-lg border-2 border-emerald-600 p-2 text-center text-2xl font-semibold focus:border-emerald-800 focus:outline-0 peer-hover:border-emerald-700"
        maxLength={8}
      />
    </div>
  );
}

type ButtonProps = React.ButtonHTMLAttributes<HTMLButtonElement> & {
  children?: ReactNode;
};

function Button({ children, ...props }: ButtonProps) {
  return (
    <button
      className="rounded-lg border-2 bg-emerald-600 p-2 text-2xl font-semibold text-white hover:bg-emerald-700"
      {...props}
    >
      {children}
    </button>
  );
}
