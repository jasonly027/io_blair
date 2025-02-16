/// <reference path="../.astro/types.d.ts" />
import { ThreeElements } from "@react-three/fiber";

declare global {
  interface ImportMetaEnv {
    readonly PUBLIC_SERVER_ENDPOINT: string;
  }

  interface ImportMeta {
    readonly env: ImportMetaEnv;
  }

  namespace React {
    namespace JSX {
      interface IntrinsicElements extends ThreeElements {}
    }
  }
}
