/// <reference types="vite/client" />

declare global {
  interface ImportMetaEnv {
    readonly VITE_SERVER_ENDPOINT: string;
  }

  interface ImportMeta {
    readonly env: ImportMetaEnv;
  }
}
