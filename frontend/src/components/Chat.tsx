import { useEffect, useRef, useState, type SetStateAction } from "react";
import type { GameConnectionListener } from "../lib/GameConnection";
import type { GamePlayer } from "../types/character";
import useConnection from "../hooks/useConnection";
import { setGameFocused } from "../lib/game";
import { SFX_VOLUME } from "../lib/sounds";

interface MessageData {
  who: GamePlayer;
  content: string;
}

const popSfx = new Audio("/audio/pop.mp3");
popSfx.volume = SFX_VOLUME;

export default function Chat() {
  const [history, setHistory] = useState<MessageData[]>([]);

  const { addConnectionEventListener, removeConnectionEventListener } =
    useConnection();

  useEffect(
    function listenForChatMessages() {
      const updateHistory: GameConnectionListener<"chat"> = ({
        msg: content,
      }) => {
        popSfx.currentTime = 0;
        popSfx.play();
        setHistory((prev) => [...prev, { who: "Teammate", content }]);
      };

      addConnectionEventListener("chat", updateHistory);

      return () => removeConnectionEventListener("chat", updateHistory);
    },
    [addConnectionEventListener, removeConnectionEventListener],
  );

  return (
    <div className="flex size-full flex-col rounded-sm bg-black/30">
      <History history={history} />
      <Input setHistory={setHistory} />
    </div>
  );
}

function History({ history }: { history: MessageData[] }) {
  const divRef = useRef<HTMLDivElement>(null!);

  useEffect(function scrollToBottomOnUpdate() {
    divRef.current.scrollTo({
      top: divRef.current.scrollHeight,
      behavior: "smooth",
    });
  });

  return (
    <div
      ref={divRef}
      className="mt-1 mr-2 h-full overflow-scroll overflow-x-hidden overflow-y-auto px-3 pt-2 text-lg text-white"
    >
      {history.map((msg, i) => (
        <Message key={i} messageData={msg} />
      ))}
    </div>
  );
}

function Message({
  messageData: { who, content },
}: {
  messageData: MessageData;
}) {
  return (
    <div className="text-wrap break-all">
      <span className={who === "You" ? "text-emerald-300" : "text-cyan-300"}>
        {who}:{" "}
      </span>
      {content}
    </div>
  );
}

interface InputProps {
  setHistory: React.Dispatch<SetStateAction<MessageData[]>>;
}

function Input({ setHistory }: InputProps) {
  const { message } = useConnection();

  const inputRef = useRef<HTMLInputElement>(null!);

  useEffect(
    function registerEnterKeyListner() {
      const submit = () => {
        if (inputRef.current.value === "") return;

        const msg = inputRef.current.value;
        setHistory((prev) => [...prev, { who: "You", content: msg }]);
        message(msg);
        inputRef.current.value = "";
      };

      const onEnter = () => {
        if (document.activeElement === inputRef.current) {
          submit();
          inputRef.current.blur();
        } else {
          inputRef.current.focus();
        }
      };

      const onEscape = () => inputRef.current.blur();

      const onKeyDown = ({ key }: KeyboardEvent) => {
        switch (key) {
          case "Enter":
            onEnter();
            break;
          case "Escape":
            onEscape();
            break;
        }
      };

      window.addEventListener("keydown", onKeyDown);
      return () => window.removeEventListener("keydown", onKeyDown);
    },
    [message, setHistory],
  );

  return (
    <input
      ref={inputRef}
      onFocus={() => setGameFocused(false)}
      onBlur={() => setGameFocused(true)}
      placeholder="Send Message (Enter)"
      className="m-1 rounded-sm border-2 border-black/25 px-2 text-lg font-semibold placeholder:text-white/65 placeholder:select-none focus:outline-0"
    />
  );
}
