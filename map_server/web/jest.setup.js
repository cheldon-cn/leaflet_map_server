// Jest setup file
import '@testing-library/jest-dom';

// Mock global objects
global.L = {
  map: jest.fn(() => ({
    setView: jest.fn(),
    on: jest.fn(),
    remove: jest.fn(),
    getBounds: jest.fn(),
    getCenter: jest.fn(),
    getZoom: jest.fn(),
    invalidateSize: jest.fn(),
    zoomIn: jest.fn(),
    zoomOut: jest.fn(),
  })),
  tileLayer: jest.fn(),
  canvas: jest.fn(),
};

// Mock localStorage
const localStorageMock = {
  getItem: jest.fn(),
  setItem: jest.fn(),
  removeItem: jest.fn(),
  clear: jest.fn(),
};
global.localStorage = localStorageMock;

// Mock fetch
global.fetch = jest.fn();

// Mock console methods for cleaner test output
global.console = {
  ...console,
  log: jest.fn(),
  error: jest.fn(),
  warn: jest.fn(),
  info: jest.fn(),
  debug: jest.fn(),
};