import { useState } from "react";
import { a } from "@react-spring/web";
import useDynamicScale from "../hooks/useDynamicScale";
import { playClickSfx, setAllAudioMute } from "../lib/sounds";

export default function Sound() {
  const [muted, setMuted] = useState(true);

  const onClick = () => {
    setMuted((prev) => {
      setAllAudioMute(!prev);
      return !prev;
    });
  };

  const { scale, increaseScale, decreaseScale } = useDynamicScale(1.05);

  return (
    <a.button
      onClick={() => {
        playClickSfx();
        onClick();
      }}
      style={scale}
      onMouseEnter={increaseScale}
      onMouseLeave={decreaseScale}
      onPointerDown={increaseScale}
      onPointerUp={decreaseScale}
      onPointerLeave={decreaseScale}
      type="button"
      className="ml-safe-l mt-safe-t fixed top-3 left-3 cursor-pointer rounded-lg border-3 border-white bg-emerald-400 p-1 hover:bg-emerald-500 focus:outline-0 active:bg-green-500 [&>svg]:size-9"
    >
      {muted ? (
        <svg
          xmlns="http://www.w3.org/2000/svg"
          fill="none"
          viewBox="0 0 24 24"
          strokeWidth={2.5}
          stroke="currentColor"
        >
          <title>Unmute</title>
          <path
            strokeLinecap="round"
            strokeLinejoin="round"
            d="M17.25 9.75 19.5 12m0 0 2.25 2.25M19.5 12l2.25-2.25M19.5 12l-2.25 2.25m-10.5-6 4.72-4.72a.75.75 0 0 1 1.28.53v15.88a.75.75 0 0 1-1.28.53l-4.72-4.72H4.51c-.88 0-1.704-.507-1.938-1.354A9.009 9.009 0 0 1 2.25 12c0-.83.112-1.633.322-2.396C2.806 8.756 3.63 8.25 4.51 8.25H6.75Z"
          />
        </svg>
      ) : (
        <svg
          xmlns="http://www.w3.org/2000/svg"
          fill="none"
          viewBox="0 0 24 24"
          strokeWidth={2.5}
          stroke="currentColor"
        >
          <title>Mute</title>
          <path
            strokeLinecap="round"
            strokeLinejoin="round"
            d="M19.114 5.636a9 9 0 0 1 0 12.728M16.463 8.288a5.25 5.25 0 0 1 0 7.424M6.75 8.25l4.72-4.72a.75.75 0 0 1 1.28.53v15.88a.75.75 0 0 1-1.28.53l-4.72-4.72H4.51c-.88 0-1.704-.507-1.938-1.354A9.009 9.009 0 0 1 2.25 12c0-.83.112-1.633.322-2.396C2.806 8.756 3.63 8.25 4.51 8.25H6.75Z"
          />
        </svg>
      )}
    </a.button>
  );
}
