import { useEffect, useRef, useState, type SetStateAction } from "react";
import { useSession } from "../hooks/useSession";
import type { GameConnectionListener } from "../lib/GameConnection";

interface MessageData {
  who: "You" | "Teammate";
  content: string;
}

export default function Chat() {
  const [history, setHistory] = useState<MessageData[]>([]);

  const { addConnectionEventListener, removeConnectionEventListener } =
    useSession();

  useEffect(() => {
    const updateHistory: GameConnectionListener<"chat"> = ({
      msg: content,
    }) => {
      setHistory((prev) => [...prev, { who: "Teammate", content }]);
    };

    addConnectionEventListener("chat", updateHistory);

    return () => {
      removeConnectionEventListener("chat", updateHistory);
    };
  }, [addConnectionEventListener, removeConnectionEventListener]);

  return (
    <div className="absolute bottom-12 size-full max-h-72 max-w-lg px-8 sm:left-12">
      <div className="flex size-full flex-col rounded-sm bg-black/20">
        <History history={history} />
        <Input setHistory={setHistory} />
      </div>
    </div>
  );
}

function History({ history }: { history: MessageData[] }) {
  return (
    <div className="mt-1 mr-2 h-full overflow-scroll overflow-x-hidden overflow-y-auto px-3 py-2 text-lg text-white">
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
    <div>
      <span className={who === "You" ? "text-emerald-400" : "text-cyan-400"}>
        {who}:{" "}
      </span>
      <span>{content}</span>
    </div>
  );
}

interface InputProps {
  setHistory: React.Dispatch<SetStateAction<MessageData[]>>;
}

function Input({ setHistory }: InputProps) {
  const { message } = useSession();

  const inputRef = useRef<HTMLInputElement>(null!);

  useEffect(
    function registerEnterKeyListner() {
      const toggleFocus = () => {
        if (document.activeElement === inputRef.current) {
          inputRef.current.blur();
          return;
        }
        inputRef.current.focus();
      };

      const submit = () => {
        if (inputRef.current.value === "") return;

        const msg = inputRef.current.value;
        setHistory((prev) => [...prev, { who: "You", content: msg }]);
        message(msg);
        inputRef.current.value = "";
      };

      const onKeyDown = ({ key }: KeyboardEvent) => {
        if (key !== "Enter") return;
        submit();
        toggleFocus();
      };

      window.addEventListener("keydown", onKeyDown);
      return () => window.removeEventListener("keydown", onKeyDown);
    },
    [message, setHistory],
  );

  return (
    <input
      ref={inputRef}
      placeholder="Send Message (Enter)"
      className="m-2 rounded-sm border-2 border-black/25 px-2 text-lg font-semibold placeholder:text-white/65 placeholder:select-none focus:outline-0"
    />
  );
}
