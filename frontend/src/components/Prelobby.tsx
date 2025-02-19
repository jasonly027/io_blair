import { useEffect, useRef } from "react";
import { useSession } from "../hooks/useSession";
import Pregame from "./Pregame";
import { a, useSpring } from "@react-spring/web";
import usePrelobbyStatus, { PrelobbyStatus } from "../hooks/usePrelobbyStatus";
import Loading from "./Loading";

export default function Prelobby() {
  const { status, setToLoading } = usePrelobbyStatus();

  return (
    <Pregame>
      {status === PrelobbyStatus.Loading ? (
        <Loading />
      ) : (
        <div className="flex flex-col items-center space-y-4">
          <Create setToLoading={setToLoading} />
          <span className="select-none">or</span>
          <Join status={status} setToLoading={setToLoading} />
        </div>
      )}
    </Pregame>
  );
}

interface CreateProps {
  setToLoading: () => void;
}

function Create({ setToLoading }: CreateProps) {
  const { createLobby } = useSession();

  const tryCreateLobby = () => {
    setToLoading();
    createLobby();
  };

  const [style, styleApi] = useSpring(() => ({
    from: {
      scale: 1,
    },
  }));
  const increaseScale = () => styleApi.start({ scale: 1.05 });
  const decreaseScale = () => styleApi.start({ scale: 1 });

  return (
    <a.svg
      width="279"
      height="109"
      viewBox="0 0 279 109"
      xmlns="http://www.w3.org/2000/svg"
      style={style}
      tabIndex={0}
      onKeyDown={({ key }) => {
        if (key !== "Enter") return;
        decreaseScale();
      }}
      onKeyUp={({ key }) => {
        if (key !== "Enter") return;
        increaseScale();
        tryCreateLobby();
      }}
      className="group focus:outline-0"
    >
      <g
        onClick={tryCreateLobby}
        onMouseEnter={increaseScale}
        onMouseLeave={decreaseScale}
        onMouseDown={decreaseScale}
        onMouseUp={increaseScale}
        className="fill-emerald-400 group-focus:fill-emerald-500 hover:cursor-pointer hover:fill-emerald-500 active:fill-green-500"
      >
        <path
          d="M12.5464 62.5488L6 37.5976L12.5464 24.7195L34 9.5L66.5 14.6585H108.872L157.97 5L196.781 14.6585L249.152 11.439L273 30.7561L263.18 58.122L273 80.6585L249.152 104L203.5 95L126.641 104L78.5 95L35 99L6 89.1098L12.5464 62.5488Z"
          className="stroke-white stroke-[9]"
        ></path>
        <text
          x="50%"
          y="50%"
          textAnchor="middle"
          dy=".3em"
          className="fill-white text-3xl select-none"
        >
          Create Lobby
        </text>
      </g>
    </a.svg>
  );
}

interface JoinProps {
  status: PrelobbyStatus;
  setToLoading: () => void;
}

function Join({ status, setToLoading }: JoinProps) {
  const { joinLobby } = useSession();

  const inputRef = useRef<HTMLInputElement>(null!);

  const submitJoinCode = () => {
    if (inputRef.current.value === "") return;

    setToLoading();

    const code = inputRef.current.value;
    joinLobby(code);
  };

  const onSubmit = () => {
    inputRef.current.setCustomValidity("");
    submitJoinCode();
  };

  useEffect(
    function CheckIfShouldShowInputError() {
      if (status === PrelobbyStatus.JoinFailed) {
        inputRef.current.setCustomValidity(
          "Lobby with specified code couldn't be found",
        );
        inputRef.current.reportValidity();
      }
    },
    [status],
  );

  const [style, styleApi] = useSpring(() => ({
    from: {
      scale: 1,
    },
  }));
  const increaseScale = () => styleApi.start({ scale: 1.05 });
  const decreaseScale = () => styleApi.start({ scale: 1 });

  return (
    <a.div
      onMouseEnter={increaseScale}
      onMouseLeave={decreaseScale}
      onFocus={increaseScale}
      onBlur={decreaseScale}
      style={style}
      className="relative"
    >
      <svg
        width="300"
        height="115"
        viewBox="0 0 300 115"
        fill="none"
        xmlns="http://www.w3.org/2000/svg"
        className="peer"
      >
        <g
          onClick={submitJoinCode}
          onMouseDown={decreaseScale}
          onMouseUp={increaseScale}
          className="group hover:cursor-pointer"
        >
          <path
            d="M165.099 14.9057L123.305 8.96226L98.7199 13.9151L60.0866 5L20.4444 21.2195L6 66.7453L30.5848 97.783L69.641 104.717C69.641 104.717 101.955 99.7642 103.641 99.7642C105.327 99.7642 130.064 105.519 137.205 107.5L189.444 99.7642L223.4 110L263.438 104.717L280.999 87.217L295 67L278.872 17.5L243.419 14.9057L206.191 5L165.099 14.9057Z"
            className="fill-cyan-500 stroke-white stroke-[9] group-hover:fill-cyan-600 group-active:fill-blue-400"
          />
          <path
            d="M266.5 54.9019C268.5 56.0566 268.5 58.9434 266.5 60.0981L228.25 82.1817C226.25 83.3364 223.75 81.8931 223.75 79.5836L223.75 35.4164C223.75 33.107 226.25 31.6636 228.25 32.8183L266.5 54.9019Z"
            className="fill-white"
          />
        </g>
      </svg>
      <input
        ref={inputRef}
        onKeyDown={({ key }) => {
          if (key !== "Enter") return;
          decreaseScale();
        }}
        onKeyUp={({ key }) => {
          if (key !== "Enter") return;
          increaseScale();
          onSubmit();
        }}
        type="text"
        placeholder="Join Code"
        className="absolute top-1/2 left-[41%] w-[10ch] -translate-x-1/2 -translate-y-1/2 transform rounded-lg bg-cyan-600 py-1 text-center placeholder-white peer-has-hover:bg-cyan-700 peer-has-active:bg-blue-500 placeholder:opacity-75 invalid:placeholder-red-500 focus:outline-0"
        maxLength={8}
      />
    </a.div>
  );
}
