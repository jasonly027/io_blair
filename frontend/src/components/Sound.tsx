import { useState } from "react";
import { a } from "@react-spring/web";
import useDynamicScale from "../hooks/useDynamicScale";
import { BGM_VOLUME, playClickSfx } from "../lib/sounds";
import BGM from "/audio/BGM.mp3"

const bgm = new Audio(BGM);
bgm.loop = true;
bgm.volume = BGM_VOLUME;

export default function Sound() {
  const [muted, setMuted] = useState(true);

  const onClick = () => {
    if (muted) {
      setMuted(false);
      bgm.play();
    } else {
      setMuted(true);
      bgm.pause();
    }
  };

  const { scale, increaseScale, decreaseScale } = useDynamicScale(1.05);

  return (
    <a.button
      onClick={onClick}
      style={scale}
      onMouseEnter={increaseScale}
      onMouseLeave={decreaseScale}
      onMouseDown={() => {
        playClickSfx();
        decreaseScale();
      }}
      onMouseUp={increaseScale}
      type="button"
      className="fixed top-5 left-5 cursor-pointer focus:outline-0 [&>svg]:size-12 [&>svg]:rounded-lg [&>svg]:bg-emerald-400 [&>svg]:p-1 [&>svg]:outline-3 [&>svg]:outline-white [&>svg]:hover:bg-emerald-500 [&>svg]:active:bg-green-500"
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
