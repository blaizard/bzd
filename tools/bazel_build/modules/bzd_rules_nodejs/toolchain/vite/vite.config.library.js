import { defineConfig } from "vite";
import path from "path";

const root = import.meta.dirname;
const isProduction = process.env.NODE_ENV == "production";

// https://vitejs.dev/config/
export default defineConfig({
	root: root,
	resolve: {
		alias: {
			"@": root,
		},
		preserveSymlinks: true,
	},
	build: {
		lib: {
			entry: ["%main%"],
			name: "%name%",
			fileName: "library",
			formats: ["umd"],
		},
		minify: isProduction ? "terser" : false,
		assetsDir: "assets",
		emptyOutDir: true,
		rollupOptions: {
			output: {
				// Ensures everything is bundled into one file.
				inlineDynamicImports: true,
			},
		},
		outDir: path.dirname(path.resolve(process.cwd(), "%output%")),
	},
});
