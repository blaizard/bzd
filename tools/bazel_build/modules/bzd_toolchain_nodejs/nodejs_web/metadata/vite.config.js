import { defineConfig } from 'vite';
import path from "path";
import vue from '@vitejs/plugin-vue2'

const root = path.resolve(process.env.BZD_ROOT_DIR);

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [vue()],
  resolve: {
    alias: {
      '#bzd': root,
    },
    preserveSymlinks: true
  },
  build: {
      rollupOptions: {
          input: {
              main: path.join(root, 'index.html'),
          },
      },
  }
})
