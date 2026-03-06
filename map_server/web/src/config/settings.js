// Application settings and configuration

const CONFIG = {
    // API Configuration
    API_BASE_URL: import.meta.env.VITE_API_BASE_URL || 'http://localhost:8080',

    // Map Configuration
    MAP: {
        defaultCenter: [39.9042, 116.4074], // Beijing coordinates
        defaultZoom: 10,
        minZoom: 3,
        maxZoom: 18,
        maxBounds: [
            [-90, -180],
            [90, 180],
        ], // World bounds
        tileLayer: {
            url: 'https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png',
            attribution:
                '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors',
            maxZoom: 19,
        },
    },

    // Rendering Configuration
    RENDER: {
        defaultWidth: 1024,
        defaultHeight: 768,
        maxWidth: 4096,
        maxHeight: 4096,
        backgroundColor: '#ffffff',
        defaultQuality: 'medium',
        qualityOptions: {
            low: { scale: 0.5, compression: 6 },
            medium: { scale: 1.0, compression: 3 },
            high: { scale: 2.0, compression: 1 },
        },
        defaultFormat: 'png',
        supportedFormats: ['png', 'webp', 'jpeg'],
    },

    // Cache Configuration
    CACHE: {
        defaultStrategy: 'balanced',
        strategies: {
            aggressive: {
                memory: { maxItems: 100, ttl: 300000 }, // 5 minutes
                localStorage: { maxSize: 100 * 1024 * 1024, ttl: 3600000 }, // 1 hour
                skipNetwork: true,
            },
            balanced: {
                memory: { maxItems: 50, ttl: 180000 }, // 3 minutes
                localStorage: { maxSize: 50 * 1024 * 1024, ttl: 1800000 }, // 30 minutes
                skipNetwork: false,
            },
            none: {
                memory: { maxItems: 0, ttl: 0 },
                localStorage: { maxSize: 0, ttl: 0 },
                skipNetwork: false,
            },
        },
    },

    // Performance Configuration
    PERFORMANCE: {
        debounceDelay: 200, // milliseconds
        requestTimeout: 30000, // 30 seconds
        maxRetries: 3,
        retryDelay: 1000, // 1 second
        concurrentRequests: 3,
        preloadAdjacent: true,
        preloadDistance: 1.5, // multiplier of viewport size
    },

    // UI Configuration
    UI: {
        defaultTheme: 'light',
        themes: {
            light: {
                primary: '#3498db',
                secondary: '#2ecc71',
                background: '#f9fafb',
                surface: '#ffffff',
                text: '#111827',
            },
            dark: {
                primary: '#60a5fa',
                secondary: '#34d399',
                background: '#111827',
                surface: '#1f2937',
                text: '#f9fafb',
            },
        },
        animationSpeed: 'normal', // fast, normal, slow
        showScale: true,
        showAttribution: true,
        showCoordinates: true,
    },

    // Layer Configuration
    LAYERS: {
        defaultLayers: [
            {
                id: 'base',
                name: 'Base Map',
                type: 'tile',
                url: 'https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png',
                visible: true,
                opacity: 1.0,
            },
            {
                id: 'roads',
                name: 'Roads',
                type: 'vector',
                source: 'database',
                visible: true,
                opacity: 1.0,
                style: {
                    color: '#ff0000',
                    width: 2,
                },
            },
            {
                id: 'buildings',
                name: 'Buildings',
                type: 'vector',
                source: 'database',
                visible: true,
                opacity: 0.8,
                style: {
                    fill: '#cccccc',
                    stroke: '#999999',
                    strokeWidth: 1,
                },
            },
        ],
        maxLayers: 20,
    },

    // Error Handling Configuration
    ERROR_HANDLING: {
        showToast: true,
        logToConsole: true,
        maxToastDisplay: 5,
        toastDuration: 5000, // milliseconds
        retryOnNetworkError: true,
        fallbackToCached: true,
    },

    // Monitoring Configuration
    MONITORING: {
        enabled: true,
        logPerformance: true,
        logErrors: true,
        logCacheStats: true,
        sampleRate: 0.1, // 10% of requests
        metricsEndpoint: '/api/metrics',
    },

    // Development Configuration
    DEVELOPMENT: {
        debug: import.meta.env.DEV,
        logLevel: import.meta.env.DEV ? 'debug' : 'warn',
        mockResponses: false,
        mockDelay: 1000,
        enableHotReload: true,
    },
};

// Environment-specific overrides
const environment = import.meta.env.MODE || 'development';

const environmentConfigs = {
    development: {
        // 使用Vite代理，前端通过端口3000访问后端，避免CORS问题
        API_BASE_URL: 'http://localhost:3000/api',
        DEVELOPMENT: {
            ...CONFIG.DEVELOPMENT,
            debug: true,
            logLevel: 'debug',
            mockResponses: false,
        },
    },
    staging: {
        API_BASE_URL: 'https://staging-api.example.com',
        DEVELOPMENT: {
            ...CONFIG.DEVELOPMENT,
            debug: false,
            logLevel: 'info',
        },
    },
    production: {
        API_BASE_URL: 'https://api.example.com',
        DEVELOPMENT: {
            ...CONFIG.DEVELOPMENT,
            debug: false,
            logLevel: 'error',
        },
        CACHE: {
            ...CONFIG.CACHE,
            strategies: {
                ...CONFIG.CACHE.strategies,
                aggressive: {
                    ...CONFIG.CACHE.strategies.aggressive,
                    memory: { maxItems: 200, ttl: 600000 }, // 10 minutes
                },
            },
        },
    },
};

// Merge environment-specific config
const envConfig = environmentConfigs[environment] || {};

// Deep merge function
function deepMerge(target, source) {
    const result = { ...target };

    for (const key in source) {
        if (source[key] && typeof source[key] === 'object' && !Array.isArray(source[key])) {
            result[key] = deepMerge(target[key] || {}, source[key]);
        } else {
            result[key] = source[key];
        }
    }

    return result;
}

// Create final config
const settings = deepMerge(CONFIG, envConfig);

// Freeze config to prevent accidental modifications
Object.freeze(settings);

// Export configuration
export { settings };

// Utility functions for config access
export function getConfig(path, defaultValue = null) {
    const parts = path.split('.');
    let current = settings;

    for (const part of parts) {
        if (current && typeof current === 'object' && part in current) {
            current = current[part];
        } else {
            return defaultValue;
        }
    }

    return current;
}

export function updateConfig(path, value) {
    console.warn(
        'Config updates are not allowed in production. This is a development-only operation.'
    );

    if (import.meta.env.DEV) {
        const parts = path.split('.');
        let current = settings;

        for (let i = 0; i < parts.length - 1; i++) {
            const part = parts[i];
            if (!(part in current)) {
                current[part] = {};
            }
            current = current[part];
        }

        current[parts[parts.length - 1]] = value;
        return true;
    }

    return false;
}

export function getApiUrl(endpoint) {
    const baseUrl = settings.API_BASE_URL.replace(/\/$/, '');
    const cleanEndpoint = endpoint.replace(/^\//, '');
    return `${baseUrl}/${cleanEndpoint}`;
}

export function getQualitySettings(quality) {
    return settings.RENDER.qualityOptions[quality] || settings.RENDER.qualityOptions.medium;
}

export function isFeatureEnabled(feature) {
    const featureFlags = {
        cache: true,
        preload: settings.PERFORMANCE.preloadAdjacent,
        offline: 'serviceWorker' in navigator,
        webp: (() => {
            const canvas = document.createElement('canvas');
            return canvas.toDataURL('image/webp').indexOf('data:image/webp') === 0;
        })(),
    };

    return featureFlags[feature] || false;
}

// Export default
export default settings;
