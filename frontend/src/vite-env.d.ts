/// <reference types="vite/client" />
import { ThreeElements } from "@react-three/fiber";

declare global {
  interface ImportMetaEnv {
    readonly VITE_SERVER_ENDPOINT: string;
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
