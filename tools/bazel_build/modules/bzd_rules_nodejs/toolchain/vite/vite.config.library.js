import { defineConfig } from "vite";
import path from "path";

const root = path.resolve("%root%");
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
		target: "es6",
		assetsDir: "assets",
		emptyOutDir: true,
		rollupOptions: {
			output: {
				// Ensures everything is bundled into one file.
				inlineDynamicImports: true,
			},
		},
	},
	css: {
		preprocessorOptions: {
			scss: {
				api: "modern-compiler",
			},
		},
	},
});
