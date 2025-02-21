import { StrictMode } from "react";
import { SessionProvider } from "../components/SessionProvider";
import GameState from "../components/GameState";

// Fix react-spring/web and react-spring/three conflict:
// https://github.com/pmndrs/react-spring/issues/1586
import { Globals } from "@react-spring/shared";
Globals.assign({
  frameLoop: "always",
});
const originalWarn = console.warn;
console.warn = (message) => {
  if (
    message.includes(
      "Cannot call the manual advancement of rafz whilst frameLoop is not set as demand",
    )
  ) {
    return;
  }
  originalWarn(message);
};

export default function App() {
  return (
    <SessionProvider>
      <StrictMode>
        <GameState />
      </StrictMode>
    </SessionProvider>
  );
}
