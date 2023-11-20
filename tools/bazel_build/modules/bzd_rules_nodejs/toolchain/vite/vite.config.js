import { defineConfig } from "vite";
import path from "path";
import vue from "@vitejs/plugin-vue";

const root = path.resolve(process.env.BZD_ROOT_DIR);
const isProduction = process.env.NODE_ENV == "production";

// https://vitejs.dev/config/
export default defineConfig({
	plugins: [vue()],
	root: root,
	resolve: {
		alias: {
			"#bzd": root,
			"@": root,
		},
		preserveSymlinks: true,
	},
	build: {
		minify: isProduction ? "esbuild" : false,
		assetsDir: "assets",
		emptyOutDir: true,
		rollupOptions: {
			input: {
				index: "/index.html",
			},
		},
	},
});
