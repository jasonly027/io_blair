import { useEffect, useState } from "react";

export default function Loading() {
  const [ellipsis, setEllipsis] = useState("");

  useEffect(function periodicallyUpdateEllipsis() {
    const interval = setInterval(() => {
      setEllipsis((prev) => (prev.length < 3 ? prev + "." : "."));
    }, 500);

    return () => clearInterval(interval);
  }, []);

  return <h2>Waiting for server{ellipsis}</h2>;
}
