import eslint from "@eslint/js";
import tseslint from "typescript-eslint"
import eslintPluginAstro from "eslint-plugin-astro";
import eslintPluginReact from "eslint-plugin-react"

export default tseslint.config(
  eslint.configs.recommended,
  tseslint.configs.recommended,
  eslintPluginAstro.configs.recommended,
  eslintPluginReact.configs.flat["jsx-runtime"]!
);
