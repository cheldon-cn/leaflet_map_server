// API endpoints configuration

const ENDPOINTS = {
    // Map rendering endpoints
    GENERATE_MAP: {
        method: 'POST',
        path: '/generate',
        description: 'Generate a map image for the specified viewport',
        timeout: 30000, // 30 seconds
        retries: 3,
        cacheable: true,
        requestSchema: {
            bbox: {
                type: 'array',
                items: { type: 'number' },
                minItems: 4,
                maxItems: 4,
                description: 'Bounding box [west, south, east, north] in WGS84 coordinates',
            },
            width: {
                type: 'integer',
                minimum: 1,
                maximum: 4096,
                default: 1024,
                description: 'Output image width in pixels',
            },
            height: {
                type: 'integer',
                minimum: 1,
                maximum: 4096,
                default: 768,
                description: 'Output image height in pixels',
            },
            layers: {
                type: 'array',
                items: { type: 'string' },
                default: [],
                description: 'List of layer IDs to include in the rendering',
            },
            styles: {
                type: 'object',
                additionalProperties: {
                    type: 'object',
                    properties: {
                        color: { type: 'string' },
                        width: { type: 'number' },
                        fill: { type: 'string' },
                        stroke: { type: 'string' },
                        strokeWidth: { type: 'number' },
                    },
                },
                default: {},
                description: 'Layer-specific style overrides',
            },
            backgroundColor: {
                type: 'string',
                pattern: '^#[0-9a-fA-F]{6}$',
                default: '#ffffff',
                description: 'Background color in hex format',
            },
            filter: {
                type: 'string',
                default: '',
                description: 'SQL WHERE clause for filtering features',
            },
            format: {
                type: 'string',
                enum: ['png', 'webp', 'jpeg'],
                default: 'png',
                description: 'Output image format',
            },
            quality: {
                type: 'integer',
                minimum: 0,
                maximum: 100,
                default: 90,
                description: 'Image quality (0-100) for lossy formats',
            },
        },
        responseSchema: {
            type: 'object',
            properties: {
                image: {
                    type: 'string',
                    format: 'binary',
                    description: 'Image binary data',
                },
                metadata: {
                    type: 'object',
                    properties: {
                        renderTime: {
                            type: 'number',
                            description: 'Rendering time in milliseconds',
                        },
                        cacheHit: {
                            type: 'boolean',
                            description: 'Whether the response was served from cache',
                        },
                        featuresCount: {
                            type: 'integer',
                            description: 'Number of features rendered',
                        },
                        bbox: {
                            type: 'array',
                            items: { type: 'number' },
                            description: 'Actual bounding box rendered',
                        },
                        timestamp: {
                            type: 'string',
                            format: 'date-time',
                            description: 'When the image was rendered',
                        },
                    },
                    required: ['renderTime', 'cacheHit', 'timestamp'],
                },
            },
        },
    },

    // Tile service endpoints
    GET_TILE: {
        method: 'GET',
        path: '/tile/{z}/{x}/{y}.png',
        description: 'Get a map tile for the specified coordinates and zoom level',
        timeout: 10000, // 10 seconds
        retries: 2,
        cacheable: true,
        pathParams: {
            z: {
                type: 'integer',
                minimum: 0,
                maximum: 20,
                description: 'Zoom level',
            },
            x: {
                type: 'integer',
                minimum: 0,
                description: 'Tile X coordinate',
            },
            y: {
                type: 'integer',
                minimum: 0,
                description: 'Tile Y coordinate',
            },
        },
        queryParams: {
            style: {
                type: 'string',
                description: 'Tile style identifier',
            },
        },
    },

    // Layer management endpoints
    LIST_LAYERS: {
        method: 'GET',
        path: '/layers',
        description: 'Get list of available map layers',
        timeout: 5000, // 5 seconds
        retries: 1,
        cacheable: true,
        responseSchema: {
            type: 'array',
            items: {
                type: 'object',
                properties: {
                    id: { type: 'string' },
                    name: { type: 'string' },
                    type: { type: 'string', enum: ['vector', 'raster', 'tile'] },
                    description: { type: 'string' },
                    bounds: {
                        type: 'array',
                        items: { type: 'number' },
                        description: 'Layer bounds [west, south, east, north]',
                    },
                    attributes: {
                        type: 'array',
                        items: { type: 'string' },
                        description: 'Available attribute fields',
                    },
                },
                required: ['id', 'name', 'type'],
            },
        },
    },

    GET_LAYER_INFO: {
        method: 'GET',
        path: '/layers/{layerId}',
        description: 'Get detailed information about a specific layer',
        timeout: 5000,
        retries: 1,
        cacheable: true,
        pathParams: {
            layerId: {
                type: 'string',
                description: 'Layer identifier',
            },
        },
    },

    // Service metadata endpoints
    GET_CAPABILITIES: {
        method: 'GET',
        path: '/capabilities',
        description: 'Get service capabilities (WMS-like)',
        timeout: 5000,
        retries: 1,
        cacheable: true,
        responseSchema: {
            type: 'object',
            properties: {
                service: {
                    type: 'object',
                    properties: {
                        name: { type: 'string' },
                        version: { type: 'string' },
                        title: { type: 'string' },
                        abstract: { type: 'string' },
                    },
                },
                capabilities: {
                    type: 'object',
                    properties: {
                        maxWidth: { type: 'integer' },
                        maxHeight: { type: 'integer' },
                        formats: { type: 'array', items: { type: 'string' } },
                        projections: { type: 'array', items: { type: 'string' } },
                    },
                },
            },
        },
    },

    // Health check endpoint
    GET_HEALTH: {
        method: 'GET',
        path: '/health',
        description: 'Check service health status',
        timeout: 3000, // 3 seconds
        retries: 0,
        cacheable: false,
        responseSchema: {
            type: 'object',
            properties: {
                status: { type: 'string', enum: ['healthy', 'degraded', 'unhealthy'] },
                version: { type: 'string' },
                uptime: { type: 'number' },
                timestamp: { type: 'string', format: 'date-time' },
                checks: {
                    type: 'object',
                    properties: {
                        database: { type: 'boolean' },
                        cache: { type: 'boolean' },
                        renderer: { type: 'boolean' },
                    },
                },
            },
            required: ['status', 'timestamp'],
        },
    },

    // Metrics endpoint
    POST_METRICS: {
        method: 'POST',
        path: '/metrics',
        description: 'Submit client-side metrics',
        timeout: 5000,
        retries: 1,
        cacheable: false,
        requestSchema: {
            type: 'object',
            properties: {
                sessionId: { type: 'string' },
                metrics: {
                    type: 'array',
                    items: {
                        type: 'object',
                        properties: {
                            name: { type: 'string' },
                            value: { type: 'number' },
                            timestamp: { type: 'string', format: 'date-time' },
                            tags: { type: 'object' },
                        },
                        required: ['name', 'value', 'timestamp'],
                    },
                },
            },
            required: ['metrics'],
        },
    },
};

// Utility functions for endpoint management
export function getEndpoint(name, params = {}) {
    const endpoint = ENDPOINTS[name];
    if (!endpoint) {
        throw new Error(`Endpoint not found: ${name}`);
    }

    // Build URL with path parameters
    let url = endpoint.path;
    if (params.path) {
        Object.entries(params.path).forEach(([key, value]) => {
            url = url.replace(`{${key}}`, encodeURIComponent(value));
        });
    }

    // Add query parameters
    if (params.query) {
        const queryString = new URLSearchParams(params.query).toString();
        if (queryString) {
            url += `?${queryString}`;
        }
    }

    return {
        ...endpoint,
        url,
    };
}

export function validateRequestParams(endpointName, params) {
    const endpoint = ENDPOINTS[endpointName];
    if (!endpoint) {
        return { valid: false, errors: [`Endpoint ${endpointName} not found`] };
    }

    const errors = [];

    // Validate path parameters
    if (endpoint.pathParams && params.path) {
        Object.entries(endpoint.pathParams).forEach(([paramName, paramSchema]) => {
            const value = params.path[paramName];
            if (value === undefined) {
                errors.push(`Missing required path parameter: ${paramName}`);
            } else if (paramSchema.type === 'integer' && !Number.isInteger(Number(value))) {
                errors.push(`Path parameter ${paramName} must be an integer`);
            } else if (paramSchema.minimum !== undefined && value < paramSchema.minimum) {
                errors.push(`Path parameter ${paramName} must be >= ${paramSchema.minimum}`);
            } else if (paramSchema.maximum !== undefined && value > paramSchema.maximum) {
                errors.push(`Path parameter ${paramName} must be <= ${paramSchema.maximum}`);
            }
        });
    }

    // Validate query parameters
    if (endpoint.queryParams && params.query) {
        Object.entries(endpoint.queryParams).forEach(([paramName, paramSchema]) => {
            const value = params.query[paramName];
            if (value !== undefined) {
                if (paramSchema.type === 'integer' && !Number.isInteger(Number(value))) {
                    errors.push(`Query parameter ${paramName} must be an integer`);
                } else if (paramSchema.enum && !paramSchema.enum.includes(value)) {
                    errors.push(
                        `Query parameter ${paramName} must be one of: ${paramSchema.enum.join(', ')}`
                    );
                }
            }
        });
    }

    return {
        valid: errors.length === 0,
        errors,
    };
}

export function getDefaultRequestParams(endpointName) {
    const endpoint = ENDPOINTS[endpointName];
    if (!endpoint || !endpoint.requestSchema) {
        return {};
    }

    const defaults = {};
    Object.entries(endpoint.requestSchema.properties || {}).forEach(([key, schema]) => {
        if (schema.default !== undefined) {
            defaults[key] = schema.default;
        }
    });

    return defaults;
}

export function buildRequest(endpointName, params = {}) {
    const endpoint = getEndpoint(endpointName, params);
    const validation = validateRequestParams(endpointName, params);

    if (!validation.valid) {
        throw new Error(`Invalid request parameters: ${validation.errors.join(', ')}`);
    }

    // Merge with defaults
    const defaults = getDefaultRequestParams(endpointName);
    const body = params.body ? { ...defaults, ...params.body } : undefined;

    return {
        method: endpoint.method,
        url: endpoint.url,
        body,
        timeout: endpoint.timeout,
        retries: endpoint.retries,
        cacheable: endpoint.cacheable,
    };
}

// Export endpoints for direct access
export const endpoints = ENDPOINTS;

// Default export
export default ENDPOINTS;
