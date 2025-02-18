import { StrictMode } from "react";
import { SessionProvider } from "../components/SessionProvider";
import State from "../components/State";

export default function App() {
  return (
    <SessionProvider>
      <StrictMode>
        <State />
      </StrictMode>
    </SessionProvider>
  );
}
