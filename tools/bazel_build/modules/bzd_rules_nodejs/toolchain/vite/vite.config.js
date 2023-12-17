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
		// Note, there is a but in javascript core that affects Safari only, this doesn't seem to be fixed in esbuild,
		// but in terser yes, see: https://bugs.webkit.org/show_bug.cgi?id=223533
		minify: isProduction ? "terser" : false,
		assetsDir: "assets",
		emptyOutDir: true,
		rollupOptions: {
			input: {
				index: "/index.html",
			},
		},
	}
});
