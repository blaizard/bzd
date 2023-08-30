import { defineConfig } from "vite";
import path from "path";
import vue from "@vitejs/plugin-vue2";

const root = path.resolve(process.env.BZD_ROOT_DIR);

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
		assetsDir: "assets",
		emptyOutDir: true,
		rollupOptions: {
			input: {
				index: "/index.html",
			},
		},
	},
});
