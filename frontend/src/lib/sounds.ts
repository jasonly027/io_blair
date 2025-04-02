import CLICK from "/audio/click.mp3";
import COIN from "/audio/coin.mp3";
import POP from "/audio/pop.mp3";
import WIN from "/audio/win.mp3";
import WIN_REVEAL from "/audio/win_reveal.mp3";
import FALL from "/audio/fall.mp3";
import BGM from "/audio/BGM.mp3";
import { Howl } from "howler";
import type { DispatchWithoutAction } from "react";

export const BGM_VOLUME = 0.05;
export const SFX_VOLUME = BGM_VOLUME * 1.1;

type HowlSrc = string | string[];

function newSfx(src: HowlSrc): [Howl, DispatchWithoutAction] {
  const howl = new Howl({
    src,
    volume: SFX_VOLUME,
    mute: true,
  });
  return [
    howl,
    () => {
      howl.seek(0);
      howl.play();
    },
  ];
}

const [clickSfx, _playClickSfx] = newSfx(CLICK);
export const playClickSfx = () => _playClickSfx();

const [winRevealSfx, _playWinRevealSfx] = newSfx(WIN_REVEAL);
export const playWinRevealSfx = () => _playWinRevealSfx();

const [winSfx, _playWinSfx] = newSfx(WIN);
export const playWinSfx = () => _playWinSfx();

const [popSfx, _playPopSfx] = newSfx(POP);
export const playPopSfx = () => _playPopSfx();

const [coinSfx, _playCoinSfx] = newSfx(COIN);
export const playCoinSfx = () => _playCoinSfx();

const [fallSfx, _playFallSfx] = newSfx(FALL);
export const playFallSfx = () => _playFallSfx();

const bgmMusic = new Howl({
  src: [BGM],
  volume: SFX_VOLUME,
  loop: true,
});

export function playBgmMusic() {
  bgmMusic.play();
}

export function pauseBgmMusic() {
  bgmMusic.pause();
}

export function setAllAudioMute(mute: boolean) {
  [clickSfx, winRevealSfx, winSfx, popSfx, coinSfx, fallSfx, bgmMusic].forEach(
    (sound) => sound.mute(mute),
  );

  if (!mute && !bgmMusic.playing()) {
    bgmMusic.play();
  }
}
