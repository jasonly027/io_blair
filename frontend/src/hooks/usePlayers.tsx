import {
  useCallback,
  useEffect,
  useMemo,
  useState,
  type Dispatch,
} from "react";
import type { GameCharacter } from "../types/character";
import useConnection from "./useConnection";
import type { GameConnectionListener } from "../lib/GameConnection";

export interface Player {
  hover: GameCharacter | null;
  confirm: GameCharacter | null;
}

export type ChangePlayerAction =
  | {
      type: "hover";
      character: GameCharacter;
    }
  | {
      type: "confirm";
      character: GameCharacter | null;
    };

export interface usePlayersValues {
  you: Player;
  setYou: (action: ChangePlayerAction) => boolean;
  teammate: Player;
}

/**
 * Returns the characters hovered or confirmed by you
 * and the teammate and a setter for updating you.
 *
 * The setter returns true if the operation was successful or not.
 * The operation may fail if trying to confirm a character already
 * confirmed by teammate.
 *
 * you and teammate may update from teammate changes coming from the server.
 */
export default function usePlayers(): usePlayersValues {
  const [you, setYou] = useState<Player>({ hover: null, confirm: null });
  const teammate = useTeammate(you, setYou);

  const { hoverCharacter, confirmCharacter } = useConnection();

  const changeYou = useCallback(
    ({ type, character }: ChangePlayerAction) => {
      if (type === "hover") {
        hoverCharacter(character);
        setYou((prev) => ({ ...prev, hover: character }));
        return true;
      }

      if (character === teammate.confirm && character !== null) {
        return false;
      }

      confirmCharacter(character);
      setYou((prev) => ({
        ...prev,
        confirm: character,
      }));

      return true;
    },
    [hoverCharacter, confirmCharacter, teammate],
  );

  return useMemo(
    () => ({
      you,
      setYou: changeYou,
      teammate,
    }),
    [changeYou, teammate, you],
  );
}

/**
 * Returns the character hover or confirmed by teammate. Listens
 * for server messages that update it. you hover/confirm choice
 * may be overidden to ensure exclusive character choice.
 */
function useTeammate(
  you: Player,
  setYou: Dispatch<React.SetStateAction<Player>>,
): Player {
  const [teammate, setTeammate] = useState<Player>({
    hover: null,
    confirm: null,
  });

  const { addConnectionEventListener, removeConnectionEventListener } =
    useConnection();

  useEffect(
    function listenForHover() {
      const onHover: GameConnectionListener<"characterHover"> = ({
        character,
      }) => {
        setTeammate((prev) => ({ ...prev, hover: character }));
      };

      addConnectionEventListener("characterHover", onHover);
      return () => removeConnectionEventListener("characterHover", onHover);
    },
    [addConnectionEventListener, removeConnectionEventListener],
  );

  useEffect(
    function listenForConfirm() {
      const onConfirm: GameConnectionListener<"characterConfirm"> = ({
        character,
      }) => {
        if (
          (character === you.hover || character === you.confirm) &&
          character !== null
        ) {
          setYou((prev) => ({
            ...prev,
            ...(character === you.hover ? { hover: null } : {}),
            ...(character === you.confirm ? { confirm: null } : {}),
          }));
        }
        setTeammate((prev) => ({ ...prev, confirm: character }));
      };

      addConnectionEventListener("characterConfirm", onConfirm);
      return () => removeConnectionEventListener("characterConfirm", onConfirm);
    },
    [addConnectionEventListener, removeConnectionEventListener, setYou, you],
  );

  return teammate;
}
