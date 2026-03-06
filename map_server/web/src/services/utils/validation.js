// Validation utilities for map parameters and data

/**
 * Validates a bounding box array.
 * @param {Array} bbox - [west, south, east, north]
 * @returns {Object} {valid: boolean, errors: Array<string>}
 */
export function validateBbox(bbox) {
    const errors = [];

    if (!Array.isArray(bbox)) {
        errors.push('Bounding box must be an array');
        return { valid: false, errors };
    }

    if (bbox.length !== 4) {
        errors.push('Bounding box must have exactly 4 elements');
        return { valid: false, errors };
    }

    const [west, south, east, north] = bbox;

    // Check if all values are numbers
    if (!bbox.every((val) => typeof val === 'number' && !isNaN(val))) {
        errors.push('All bounding box values must be numbers');
    }

    // Validate latitude range
    if (south < -90 || south > 90) {
        errors.push(`South coordinate (${south}) must be between -90 and 90`);
    }

    if (north < -90 || north > 90) {
        errors.push(`North coordinate (${north}) must be between -90 and 90`);
    }

    // Validate longitude range
    if (west < -180 || west > 180) {
        errors.push(`West coordinate (${west}) must be between -180 and 180`);
    }

    if (east < -180 || east > 180) {
        errors.push(`East coordinate (${east}) must be between -180 and 180`);
    }

    // Check if south < north
    if (south >= north) {
        errors.push(`South coordinate (${south}) must be less than north coordinate (${north})`);
    }

    // Check if west < east (allow crossing antimeridian)
    if (west >= east && !(west > 0 && east < 0)) {
        errors.push(`West coordinate (${west}) must be less than east coordinate (${east})`);
    }

    // Calculate area (optional warning)
    const area = (east - west) * (north - south);
    if (area > 10000) {
        errors.push(`Warning: Bounding box area (${area.toFixed(2)} sq degrees) is very large`);
    }

    return {
        valid: errors.length === 0,
        errors,
        bbox: [west, south, east, north],
    };
}

/**
 * Validates image dimensions.
 * @param {number} width - Image width in pixels
 * @param {number} height - Image height in pixels
 * @param {Object} options - Validation options
 * @returns {Object} {valid: boolean, errors: Array<string>, width: number, height: number}
 */
export function validateImageDimensions(width, height, options = {}) {
    const {
        minWidth = 1,
        maxWidth = 4096,
        minHeight = 1,
        maxHeight = 4096,
        aspectRatioTolerance = 0.1,
    } = options;

    const errors = [];

    // Validate width
    if (typeof width !== 'number' || isNaN(width)) {
        errors.push('Width must be a number');
    } else {
        width = Math.floor(width);

        if (width < minWidth) {
            errors.push(`Width (${width}) must be at least ${minWidth} pixels`);
        }

        if (width > maxWidth) {
            errors.push(`Width (${width}) must not exceed ${maxWidth} pixels`);
        }
    }

    // Validate height
    if (typeof height !== 'number' || isNaN(height)) {
        errors.push('Height must be a number');
    } else {
        height = Math.floor(height);

        if (height < minHeight) {
            errors.push(`Height (${height}) must be at least ${minHeight} pixels`);
        }

        if (height > maxHeight) {
            errors.push(`Height (${height}) must not exceed ${maxHeight} pixels`);
        }
    }

    // Check aspect ratio if both dimensions are valid
    if (!errors.length && aspectRatioTolerance > 0) {
        const aspectRatio = width / height;

        // Common aspect ratios: 4:3 (1.333), 16:9 (1.777), 1:1 (1.0)
        if (
            Math.abs(aspectRatio - 1.333) > aspectRatioTolerance &&
            Math.abs(aspectRatio - 1.777) > aspectRatioTolerance &&
            Math.abs(aspectRatio - 1.0) > aspectRatioTolerance
        ) {
            errors.push(
                `Warning: Unusual aspect ratio (${aspectRatio.toFixed(2)}) may affect image quality`
            );
        }
    }

    return {
        valid: errors.length === 0,
        errors,
        width,
        height,
    };
}

/**
 * Validates a hex color string.
 * @param {string} color - Hex color string (e.g., #FF0000 or #F00)
 * @returns {Object} {valid: boolean, errors: Array<string>, color: string}
 */
export function validateHexColor(color) {
    const errors = [];

    if (typeof color !== 'string') {
        errors.push('Color must be a string');
        return { valid: false, errors };
    }

    // Check format
    const hexRegex = /^#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})$/;

    if (!hexRegex.test(color)) {
        errors.push('Color must be a valid hex color (e.g., #FF0000 or #F00)');
    }

    return {
        valid: errors.length === 0,
        errors,
        color: color.toUpperCase(),
    };
}

/**
 * Validates a layer configuration object.
 * @param {Object} layer - Layer configuration
 * @returns {Object} {valid: boolean, errors: Array<string>, layer: Object}
 */
export function validateLayer(layer) {
    const errors = [];
    const validTypes = ['vector', 'raster', 'tile', 'wms'];

    if (!layer || typeof layer !== 'object') {
        errors.push('Layer must be an object');
        return { valid: false, errors };
    }

    // Validate ID
    if (!layer.id || typeof layer.id !== 'string') {
        errors.push('Layer must have a string ID');
    } else if (!/^[a-zA-Z0-9_-]+$/.test(layer.id)) {
        errors.push('Layer ID must contain only letters, numbers, underscores, and hyphens');
    }

    // Validate name
    if (!layer.name || typeof layer.name !== 'string') {
        errors.push('Layer must have a string name');
    }

    // Validate type
    if (!layer.type || typeof layer.type !== 'string') {
        errors.push('Layer must have a type');
    } else if (!validTypes.includes(layer.type)) {
        errors.push(`Layer type must be one of: ${validTypes.join(', ')}`);
    }

    // Validate visibility (optional)
    if (layer.visible !== undefined && typeof layer.visible !== 'boolean') {
        errors.push('Layer visibility must be a boolean');
    }

    // Validate opacity (optional)
    if (layer.opacity !== undefined) {
        if (typeof layer.opacity !== 'number' || isNaN(layer.opacity)) {
            errors.push('Layer opacity must be a number');
        } else if (layer.opacity < 0 || layer.opacity > 1) {
            errors.push('Layer opacity must be between 0 and 1');
        }
    }

    // Validate bounds if provided
    if (layer.bounds && Array.isArray(layer.bounds)) {
        const bboxValidation = validateBbox(layer.bounds);
        if (!bboxValidation.valid) {
            errors.push(...bboxValidation.errors.map((e) => `Bounds: ${e}`));
        }
    }

    return {
        valid: errors.length === 0,
        errors,
        layer: { ...layer },
    };
}

/**
 * Validates a layer style object.
 * @param {Object} style - Layer style configuration
 * @param {string} layerType - Type of layer
 * @returns {Object} {valid: boolean, errors: Array<string>, style: Object}
 */
export function validateLayerStyle(style, layerType = 'vector') {
    const errors = [];

    if (!style || typeof style !== 'object') {
        errors.push('Style must be an object');
        return { valid: false, errors };
    }

    // Validate based on layer type
    switch (layerType) {
        case 'vector':
            // Validate color
            if (style.color) {
                const colorValidation = validateHexColor(style.color);
                if (!colorValidation.valid) {
                    errors.push(`Color: ${colorValidation.errors[0]}`);
                }
            }

            // Validate width (for lines)
            if (style.width !== undefined) {
                if (typeof style.width !== 'number' || isNaN(style.width)) {
                    errors.push('Width must be a number');
                } else if (style.width < 0) {
                    errors.push('Width must be positive');
                } else if (style.width > 100) {
                    errors.push('Warning: Width is unusually large');
                }
            }

            // Validate fill color
            if (style.fill) {
                const fillValidation = validateHexColor(style.fill);
                if (!fillValidation.valid) {
                    errors.push(`Fill: ${fillValidation.errors[0]}`);
                }
            }

            // Validate stroke color
            if (style.stroke) {
                const strokeValidation = validateHexColor(style.stroke);
                if (!strokeValidation.valid) {
                    errors.push(`Stroke: ${strokeValidation.errors[0]}`);
                }
            }

            // Validate stroke width
            if (style.strokeWidth !== undefined) {
                if (typeof style.strokeWidth !== 'number' || isNaN(style.strokeWidth)) {
                    errors.push('Stroke width must be a number');
                } else if (style.strokeWidth < 0) {
                    errors.push('Stroke width must be positive');
                }
            }
            break;

        case 'raster':
        case 'tile':
        case 'wms':
            // Basic validation for other layer types
            if (style.opacity !== undefined) {
                if (typeof style.opacity !== 'number' || isNaN(style.opacity)) {
                    errors.push('Opacity must be a number');
                } else if (style.opacity < 0 || style.opacity > 1) {
                    errors.push('Opacity must be between 0 and 1');
                }
            }
            break;

        default:
            errors.push(`Unsupported layer type: ${layerType}`);
    }

    return {
        valid: errors.length === 0,
        errors,
        style: { ...style },
    };
}

/**
 * Validates a filter expression.
 * @param {string} filter - Filter expression (SQL WHERE clause)
 * @param {Array<string>} allowedFields - Allowed field names
 * @returns {Object} {valid: boolean, errors: Array<string>, filter: string}
 */
export function validateFilter(filter, allowedFields = []) {
    const errors = [];

    if (filter === undefined || filter === null) {
        return { valid: true, errors: [], filter: '' };
    }

    if (typeof filter !== 'string') {
        errors.push('Filter must be a string');
        return { valid: false, errors };
    }

    if (filter.trim() === '') {
        return { valid: true, errors: [], filter: '' };
    }

    // Check for SQL injection patterns (basic protection)
    const dangerousPatterns = [
        /(\b)(DROP\s|ALTER\s|CREATE\s|INSERT\s|UPDATE\s|DELETE\s|TRUNCATE\s|EXEC\s|EXECUTE\s)/i,
        /(\b)(UNION\s|SELECT\s.*\bFROM\b|INSERT\s.*\bINTO\b|DELETE\s.*\bFROM\b)/i,
        /(\b)(--|#|\/\*)/,
        /(\b)(OR\s+['"]?['"]?\s*=\s*['"]?['"]?)/i,
        /(\b)(AND\s+['"]?['"]?\s*=\s*['"]?['"]?)/i,
    ];

    dangerousPatterns.forEach((pattern) => {
        if (pattern.test(filter)) {
            errors.push('Filter contains potentially dangerous SQL patterns');
        }
    });

    // Validate field names if allowedFields is provided
    if (allowedFields.length > 0) {
        // Simple extraction of field names (this is a basic implementation)
        const fieldRegex = /\b([a-zA-Z_][a-zA-Z0-9_]*)\b/g;
        const matches = filter.match(fieldRegex) || [];
        const uniqueFields = [...new Set(matches)];

        // Check for fields not in allowed list
        const invalidFields = uniqueFields.filter(
            (field) =>
                !allowedFields.includes(field) &&
                !['AND', 'OR', 'NOT', 'NULL', 'TRUE', 'FALSE'].includes(field.toUpperCase())
        );

        if (invalidFields.length > 0) {
            errors.push(
                `Invalid field(s): ${invalidFields.join(', ')}. Allowed fields: ${allowedFields.join(', ')}`
            );
        }
    }

    return {
        valid: errors.length === 0,
        errors,
        filter: filter.trim(),
    };
}

/**
 * Validates a zoom level.
 * @param {number} zoom - Zoom level
 * @param {Object} options - Validation options
 * @returns {Object} {valid: boolean, errors: Array<string>, zoom: number}
 */
export function validateZoom(zoom, options = {}) {
    const { minZoom = 0, maxZoom = 18, integerOnly = true } = options;

    const errors = [];

    if (typeof zoom !== 'number' || isNaN(zoom)) {
        errors.push('Zoom must be a number');
        return { valid: false, errors };
    }

    if (integerOnly && !Number.isInteger(zoom)) {
        errors.push('Zoom must be an integer');
    }

    if (zoom < minZoom) {
        errors.push(`Zoom (${zoom}) must be at least ${minZoom}`);
    }

    if (zoom > maxZoom) {
        errors.push(`Zoom (${zoom}) must not exceed ${maxZoom}`);
    }

    return {
        valid: errors.length === 0,
        errors,
        zoom: integerOnly ? Math.floor(zoom) : zoom,
    };
}

/**
 * Validates a coordinate array.
 * @param {Array} coords - [longitude, latitude]
 * @returns {Object} {valid: boolean, errors: Array<string>, coords: Array<number>}
 */
export function validateCoordinates(coords) {
    const errors = [];

    if (!Array.isArray(coords)) {
        errors.push('Coordinates must be an array');
        return { valid: false, errors };
    }

    if (coords.length !== 2) {
        errors.push('Coordinates must have exactly 2 elements [longitude, latitude]');
        return { valid: false, errors };
    }

    const [lon, lat] = coords;

    if (typeof lon !== 'number' || isNaN(lon)) {
        errors.push('Longitude must be a number');
    } else if (lon < -180 || lon > 180) {
        errors.push(`Longitude (${lon}) must be between -180 and 180`);
    }

    if (typeof lat !== 'number' || isNaN(lat)) {
        errors.push('Latitude must be a number');
    } else if (lat < -90 || lat > 90) {
        errors.push(`Latitude (${lat}) must be between -90 and 90`);
    }

    return {
        valid: errors.length === 0,
        errors,
        coords: [lon, lat],
    };
}

/**
 * Validates a color stop array for gradients.
 * @param {Array} stops - Array of color stops [[position, color], ...]
 * @returns {Object} {valid: boolean, errors: Array<string>, stops: Array}
 */
export function validateColorStops(stops) {
    const errors = [];

    if (!Array.isArray(stops)) {
        errors.push('Color stops must be an array');
        return { valid: false, errors };
    }

    if (stops.length < 2) {
        errors.push('At least two color stops are required');
    }

    let lastPos = -1;

    stops.forEach((stop, index) => {
        if (!Array.isArray(stop) || stop.length !== 2) {
            errors.push(`Color stop ${index} must be an array [position, color]`);
            return;
        }

        const [position, color] = stop;

        // Validate position
        if (typeof position !== 'number' || isNaN(position)) {
            errors.push(`Color stop ${index}: position must be a number`);
        } else if (position < 0 || position > 1) {
            errors.push(`Color stop ${index}: position must be between 0 and 1`);
        } else if (position <= lastPos) {
            errors.push(`Color stop ${index}: positions must be in increasing order`);
        }

        // Validate color
        const colorValidation = validateHexColor(color);
        if (!colorValidation.valid) {
            errors.push(`Color stop ${index}: ${colorValidation.errors[0]}`);
        }

        lastPos = position;
    });

    return {
        valid: errors.length === 0,
        errors,
        stops: [...stops],
    };
}

/**
 * Validates an array of layers.
 * @param {Array} layers - Array of layer objects
 * @returns {Object} {valid: boolean, errors: Array<string>, layers: Array}
 */
export function validateLayers(layers) {
    const errors = [];

    if (!Array.isArray(layers)) {
        errors.push('Layers must be an array');
        return { valid: false, errors };
    }

    const validatedLayers = [];
    const layerIds = new Set();

    layers.forEach((layer, index) => {
        const layerValidation = validateLayer(layer);

        if (!layerValidation.valid) {
            errors.push(`Layer ${index}: ${layerValidation.errors.join(', ')}`);
        } else {
            // Check for duplicate IDs
            if (layerIds.has(layerValidation.layer.id)) {
                errors.push(`Layer ${index}: duplicate ID '${layerValidation.layer.id}'`);
            } else {
                layerIds.add(layerValidation.layer.id);
                validatedLayers.push(layerValidation.layer);
            }
        }
    });

    return {
        valid: errors.length === 0,
        errors,
        layers: validatedLayers,
    };
}

/**
 * Validates a complete map request object.
 * @param {Object} request - Map request object
 * @returns {Object} {valid: boolean, errors: Array<string>, request: Object}
 */
export function validateMapRequest(request) {
    const errors = [];
    const validatedRequest = { ...request };

    // Validate bbox
    if (request.bbox) {
        const bboxValidation = validateBbox(request.bbox);
        if (!bboxValidation.valid) {
            errors.push(`Bounding box: ${bboxValidation.errors.join(', ')}`);
        } else {
            validatedRequest.bbox = bboxValidation.bbox;
        }
    } else {
        errors.push('Bounding box is required');
    }

    // Validate image dimensions
    if (request.width !== undefined || request.height !== undefined) {
        const width = request.width || 1024;
        const height = request.height || 768;

        const dimValidation = validateImageDimensions(width, height);
        if (!dimValidation.valid) {
            errors.push(`Image dimensions: ${dimValidation.errors.join(', ')}`);
        } else {
            validatedRequest.width = dimValidation.width;
            validatedRequest.height = dimValidation.height;
        }
    } else {
        // Set defaults
        validatedRequest.width = 1024;
        validatedRequest.height = 768;
    }

    // Validate layers
    if (request.layers !== undefined) {
        if (Array.isArray(request.layers)) {
            const layersValidation = validateLayers(request.layers);
            if (!layersValidation.valid) {
                errors.push(`Layers: ${layersValidation.errors.join(', ')}`);
            } else {
                validatedRequest.layers = layersValidation.layers;
            }
        } else {
            errors.push('Layers must be an array');
        }
    }

    // Validate styles
    if (request.styles !== undefined) {
        if (typeof request.styles !== 'object' || request.styles === null) {
            errors.push('Styles must be an object');
        }
    }

    // Validate background color
    if (request.backgroundColor !== undefined) {
        const colorValidation = validateHexColor(request.backgroundColor);
        if (!colorValidation.valid) {
            errors.push(`Background color: ${colorValidation.errors.join(', ')}`);
        } else {
            validatedRequest.backgroundColor = colorValidation.color;
        }
    }

    // Validate filter
    if (request.filter !== undefined) {
        const filterValidation = validateFilter(request.filter);
        if (!filterValidation.valid) {
            errors.push(`Filter: ${filterValidation.errors.join(', ')}`);
        } else {
            validatedRequest.filter = filterValidation.filter;
        }
    }

    // Validate format
    const validFormats = ['png', 'webp', 'jpeg'];
    if (request.format !== undefined) {
        if (!validFormats.includes(request.format.toLowerCase())) {
            errors.push(`Format must be one of: ${validFormats.join(', ')}`);
        }
    }

    // Validate quality
    if (request.quality !== undefined) {
        const quality = Number(request.quality);
        if (isNaN(quality)) {
            errors.push('Quality must be a number');
        } else if (quality < 0 || quality > 100) {
            errors.push('Quality must be between 0 and 100');
        } else {
            validatedRequest.quality = Math.round(quality);
        }
    }

    return {
        valid: errors.length === 0,
        errors,
        request: validatedRequest,
    };
}

/**
 * Normalizes a map request to ensure all required fields are present.
 * @param {Object} request - Map request object
 * @returns {Object} Normalized request object
 */
export function normalizeMapRequest(request) {
    const defaults = {
        width: 1024,
        height: 768,
        layers: [],
        styles: {},
        backgroundColor: '#FFFFFF',
        filter: '',
        format: 'png',
        quality: 90,
    };

    const normalized = { ...defaults, ...request };

    // Ensure quality is within bounds
    if (normalized.quality < 0) normalized.quality = 0;
    if (normalized.quality > 100) normalized.quality = 100;

    return normalized;
}

// Default export
export default {
    validateBbox,
    validateImageDimensions,
    validateHexColor,
    validateLayer,
    validateLayerStyle,
    validateFilter,
    validateZoom,
    validateCoordinates,
    validateColorStops,
    validateLayers,
    validateMapRequest,
    normalizeMapRequest,
};
