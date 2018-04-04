import resolve from 'rollup-plugin-node-resolve';
import commonjs from 'rollup-plugin-commonjs';

export default {
  input: 'lib/es6/src/Main.js',
  output: {
    file: 'bin/main.js',
    format: 'cjs',
    banner: '#!/usr/bin/env node'
  },
  plugins: [
    resolve(),
    commonjs()
  ]
};
