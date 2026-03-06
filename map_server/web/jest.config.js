/** @type {import('jest').Config} */
export default {
  testEnvironment: 'jsdom',
  moduleNameMapper: {
    '^(\\.{1,2}/.*)\\.js$': '$1',
  },
  transform: {},
  collectCoverageFrom: [
    'src/**/*.js',
    '!src/**/*.worker.js',
    '!src/**/index.js',
  ],
  coverageThreshold: {
    global: {
      branches: 0,
      functions: 0,
      lines: 0,
      statements: 0,
    },
  },
  testMatch: [
    '**/__tests__/**/*.js',
    '**/*.test.js',
  ],
  setupFilesAfterEnv: ['<rootDir>/jest.setup.js'],
};