import { useEffect, useRef, useState } from "react";

export default function Client() {
  const socketRef = useRef<WebSocket>(null);

  const [history, setHistory] = useState<Array<String>>([]);
  const msgRef = useRef<HTMLInputElement>(null);

  useEffect(() => {
    socketRef.current = new WebSocket("ws://localhost:8080");
    const sock = socketRef.current;

    sock.onopen = () => {
      console.log("Connection established");
    };

    sock.onmessage = (msg) => {
      console.log(msg.data);

      const obj = JSON.parse(msg.data);
      switch (obj.type) {
        case "chat":
          setHistory((prev) => [...prev, `Other: ${obj.msg}`]);
          break;
        case "lobbyJoin":
          if (obj.success) {
            navigator.clipboard.writeText(obj.code);
          }
      }
    };

    sock.onclose = () => {
      console.log("closed");
    };

    function onEnterPress(event: KeyboardEvent) {
      if (event.key === "Enter") {
        sendMsg();
      }
    }
    window.addEventListener("keydown", onEnterPress);

    return () => {
      sock.close();
      window.removeEventListener("keydown", onEnterPress);
    };
  }, []);

  useEffect(() => {
    console.log("Rerender");
  });

  const codeRef = useRef<HTMLInputElement>(null);

  function sendMsg() {
    if (!msgRef.current?.value) {
      return;
    }

    const val = msgRef.current!.value;
    msgRef.current!.value = "";

    setHistory((prev) => [...prev, `Self: ${val}`]);

    socketRef.current?.send(
      JSON.stringify({
        type: "chat",
        msg: val,
      }),
    );
  }

  return (
    <>
      <button
        onClick={() =>
          socketRef.current?.send(JSON.stringify({ type: "lobbyCreate" }))
        }
      >
        Create Lobby
      </button>
      <button
        onClick={() =>
          socketRef.current?.send(JSON.stringify({ type: "lobbyLeave" }))
        }
      >
        Leave lobby
      </button>

      <br />

      <input type="text" ref={codeRef} placeholder="Join Code" />
      <button
        onClick={() => {
          if (!codeRef.current!.value) {
            return;
          }

          socketRef.current?.send(
            JSON.stringify({
              type: "lobbyJoin",
              code: codeRef.current!.value,
            }),
          );
        }}
      >
        Join Lobby
      </button>

      <br />
      <br />

      <input type="text" ref={msgRef} placeholder="Chat" />
      <button onClick={sendMsg}>Send</button>

      {history.map((h, i) => (
        <p key={i}>{h}</p>
      ))}
    </>
  );
}
