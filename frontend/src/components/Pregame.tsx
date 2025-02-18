import type { ReactNode } from "react";

export default function Pregame({ children }: { children?: ReactNode }) {
  return (
    <>
      <Title>Io and Blair</Title>
      {children}
    </>
  );
}

function Title({ children }: { children?: ReactNode }) {
  return (
    <h1 className="my-16 text-center text-6xl font-bold drop-shadow-md select-none">
      {children}
    </h1>
  );
}
