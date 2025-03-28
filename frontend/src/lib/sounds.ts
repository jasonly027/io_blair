import CLICK from "/audio/click.mp3"

export const BGM_VOLUME = 0.05;
export const SFX_VOLUME = BGM_VOLUME * 1.1;

const clickSfx = new Audio(CLICK);
clickSfx.volume = SFX_VOLUME;

export function playClickSfx() {
  clickSfx.currentTime = 0;
  clickSfx.play();
}
