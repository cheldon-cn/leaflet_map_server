import { defineConfig } from 'vite';
import { resolve } from 'path';

export default defineConfig({
  root: 'src',
  publicDir: '../public',
  build: {
    outDir: '../dist',
    assetsDir: 'assets',
    sourcemap: true,
    rollupOptions: {
      input: {
        main: resolve(__dirname, 'src/index.html')
      },
      output: {
        manualChunks: {
          vendor: ['leaflet', 'axios'],
          utils: ['lru-cache', 'localforage', 'debounce']
        }
      }
    }
  },
  server: {
    port: 3000,
    proxy: {
      '/api': {
        target: 'http://localhost:8080',
        changeOrigin: true,
        rewrite: (path) => path.replace(/^\/api/, '')
      }
    },
    open: false,
    cors: true
  },
  css: {
    devSourcemap: true
  },
  esbuild: {
    target: 'es2020'
  },
  optimizeDeps: {
    include: ['leaflet', 'axios']
  }
});