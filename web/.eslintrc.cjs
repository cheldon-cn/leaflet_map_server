module.exports = {
  env: {
    browser: true,
    es2021: true,
    node: true,
  },
  globals: {
    L: true,
  },
  extends: [
    'eslint:recommended',
  ],
  parserOptions: {
    ecmaVersion: 'latest',
    sourceType: 'module',
  },
  rules: {
    'indent': ['error', 4, { 'SwitchCase': 1 }],
    'linebreak-style': ['error', 'windows'],
    'quotes': ['error', 'single'],
    'semi': ['error', 'always'],
    'comma-dangle': ['error', 'always-multiline'],
    'no-unused-vars': ['warn', { 'argsIgnorePattern': '^_' }],
    'no-console': 'warn',
    'no-debugger': 'warn',
  },
  ignorePatterns: ['dist', 'node_modules', '*.worker.js'],
};