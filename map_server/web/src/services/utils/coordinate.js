// Coordinate conversion utilities

/**
 * Converts degrees to radians.
 * @param {number} degrees - Angle in degrees
 * @returns {number} Angle in radians
 */
export function degreesToRadians(degrees) {
    return (degrees * Math.PI) / 180;
}

/**
 * Converts radians to degrees.
 * @param {number} radians - Angle in radians
 * @returns {number} Angle in degrees
 */
export function radiansToDegrees(radians) {
    return (radians * 180) / Math.PI;
}

/**
 * Converts WGS84 coordinates to Web Mercator.
 * @param {number} lon - Longitude in degrees
 * @param {number} lat - Latitude in degrees
 * @returns {Object} {x, y} in meters
 */
export function wgs84ToWebMercator(lon, lat) {
    const x = (lon * 20037508.34) / 180;
    const y = Math.log(Math.tan(((90 + lat) * Math.PI) / 360)) / (Math.PI / 180);
    const yMeters = (y * 20037508.34) / 180;

    return { x: x, y: yMeters };
}

/**
 * Converts Web Mercator to WGS84 coordinates.
 * @param {number} x - X coordinate in meters
 * @param {number} y - Y coordinate in meters
 * @returns {Object} {lon, lat} in degrees
 */
export function webMercatorToWgs84(x, y) {
    const lon = (x * 180) / 20037508.34;
    const lat =
        (180 / Math.PI) * (2 * Math.atan(Math.exp((y * Math.PI) / 20037508.34)) - Math.PI / 2);

    return { lon: lon, lat: lat };
}

/**
 * Calculates the distance between two WGS84 coordinates using the Haversine formula.
 * @param {number} lat1 - Latitude of point 1 in degrees
 * @param {number} lon1 - Longitude of point 1 in degrees
 * @param {number} lat2 - Latitude of point 2 in degrees
 * @param {number} lon2 - Longitude of point 2 in degrees
 * @returns {number} Distance in meters
 */
export function haversineDistance(lat1, lon1, lat2, lon2) {
    const R = 6371000; // Earth's radius in meters

    const φ1 = degreesToRadians(lat1);
    const φ2 = degreesToRadians(lat2);
    const Δφ = degreesToRadians(lat2 - lat1);
    const Δλ = degreesToRadians(lon2 - lon1);

    const a =
        Math.sin(Δφ / 2) * Math.sin(Δφ / 2) +
        Math.cos(φ1) * Math.cos(φ2) * Math.sin(Δλ / 2) * Math.sin(Δλ / 2);

    const c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));

    return R * c;
}

/**
 * Calculates the bearing between two points.
 * @param {number} lat1 - Latitude of point 1 in degrees
 * @param {number} lon1 - Longitude of point 1 in degrees
 * @param {number} lat2 - Latitude of point 2 in degrees
 * @param {number} lon2 - Longitude of point 2 in degrees
 * @returns {number} Bearing in degrees (0-360)
 */
export function calculateBearing(lat1, lon1, lat2, lon2) {
    const φ1 = degreesToRadians(lat1);
    const φ2 = degreesToRadians(lat2);
    const Δλ = degreesToRadians(lon2 - lon1);

    const y = Math.sin(Δλ) * Math.cos(φ2);
    const x = Math.cos(φ1) * Math.sin(φ2) - Math.sin(φ1) * Math.cos(φ2) * Math.cos(Δλ);

    let bearing = Math.atan2(y, x);
    bearing = radiansToDegrees(bearing);
    bearing = (bearing + 360) % 360;

    return bearing;
}

/**
 * Calculates a destination point given starting point, distance and bearing.
 * @param {number} lat - Starting latitude in degrees
 * @param {number} lon - Starting longitude in degrees
 * @param {number} distance - Distance in meters
 * @param {number} bearing - Bearing in degrees
 * @returns {Object} {lat, lon} of destination point
 */
export function calculateDestination(lat, lon, distance, bearing) {
    const R = 6371000; // Earth's radius in meters

    const φ1 = degreesToRadians(lat);
    const λ1 = degreesToRadians(lon);
    const θ = degreesToRadians(bearing);
    const δ = distance / R;

    const φ2 = Math.asin(Math.sin(φ1) * Math.cos(δ) + Math.cos(φ1) * Math.sin(δ) * Math.cos(θ));

    const λ2 =
        λ1 +
        Math.atan2(
            Math.sin(θ) * Math.sin(δ) * Math.cos(φ1),
            Math.cos(δ) - Math.sin(φ1) * Math.sin(φ2)
        );

    return {
        lat: radiansToDegrees(φ2),
        lon: radiansToDegrees(λ2),
    };
}

/**
 * Converts a lat/lon bounding box to a polygon geometry.
 * @param {Array} bbox - [west, south, east, north]
 * @returns {Array} Polygon coordinates
 */
export function bboxToPolygon(bbox) {
    const [west, south, east, north] = bbox;

    return [
        [
            [west, south],
            [east, south],
            [east, north],
            [west, north],
            [west, south],
        ],
    ];
}

/**
 * Checks if a point is within a bounding box.
 * @param {number} lon - Longitude of point
 * @param {number} lat - Latitude of point
 * @param {Array} bbox - [west, south, east, north]
 * @returns {boolean} True if point is within bbox
 */
export function pointInBbox(lon, lat, bbox) {
    const [west, south, east, north] = bbox;

    return lon >= west && lon <= east && lat >= south && lat <= north;
}

/**
 * Calculates the center point of a bounding box.
 * @param {Array} bbox - [west, south, east, north]
 * @returns {Object} {lon, lat} of center point
 */
export function bboxCenter(bbox) {
    const [west, south, east, north] = bbox;

    return {
        lon: (west + east) / 2,
        lat: (south + north) / 2,
    };
}

/**
 * Calculates the area of a bounding box in square meters.
 * @param {Array} bbox - [west, south, east, north]
 * @returns {number} Area in square meters
 */
export function bboxArea(bbox) {
    const [west, south, east, north] = bbox;

    // Calculate area using spherical approximation
    const R = 6371000; // Earth's radius in meters

    const lat1 = degreesToRadians(south);
    const lat2 = degreesToRadians(north);
    const lon1 = degreesToRadians(west);
    const lon2 = degreesToRadians(east);

    // Area = R² * |λ2 - λ1| * |sin φ2 - sin φ1|
    const area = Math.abs(R * R * (lon2 - lon1) * (Math.sin(lat2) - Math.sin(lat1)));

    return Math.abs(area);
}

/**
 * Converts latitude to tile Y coordinate for a given zoom level.
 * @param {number} lat - Latitude in degrees
 * @param {number} zoom - Zoom level
 * @returns {number} Tile Y coordinate
 */
export function lat2tile(lat, zoom) {
    return Math.floor(
        ((1 -
            Math.log(Math.tan((lat * Math.PI) / 180) + 1 / Math.cos((lat * Math.PI) / 180)) /
                Math.PI) /
            2) *
            Math.pow(2, zoom)
    );
}

/**
 * Converts longitude to tile X coordinate for a given zoom level.
 * @param {number} lon - Longitude in degrees
 * @param {number} zoom - Zoom level
 * @returns {number} Tile X coordinate
 */
export function lon2tile(lon, zoom) {
    return Math.floor(((lon + 180) / 360) * Math.pow(2, zoom));
}

/**
 * Converts tile coordinates to latitude for a given zoom level.
 * @param {number} y - Tile Y coordinate
 * @param {number} zoom - Zoom level
 * @returns {number} Latitude in degrees
 */
export function tile2lat(y, zoom) {
    const n = Math.PI - (2 * Math.PI * y) / Math.pow(2, zoom);
    return (180 / Math.PI) * Math.atan(0.5 * (Math.exp(n) - Math.exp(-n)));
}

/**
 * Converts tile coordinates to longitude for a given zoom level.
 * @param {number} x - Tile X coordinate
 * @param {number} zoom - Zoom level
 * @returns {number} Longitude in degrees
 */
export function tile2lon(x, zoom) {
    return (x / Math.pow(2, zoom)) * 360 - 180;
}

/**
 * Calculates the bounding box of a tile.
 * @param {number} x - Tile X coordinate
 * @param {number} y - Tile Y coordinate
 * @param {number} zoom - Zoom level
 * @returns {Array} [west, south, east, north]
 */
export function tile2bbox(x, y, zoom) {
    const west = tile2lon(x, zoom);
    const east = tile2lon(x + 1, zoom);
    const north = tile2lat(y, zoom);
    const south = tile2lat(y + 1, zoom);

    return [west, south, east, north];
}

/**
 * Formats coordinates for display.
 * @param {number} lat - Latitude in degrees
 * @param {number} lon - Longitude in degrees
 * @param {Object} options - Formatting options
 * @returns {string} Formatted coordinate string
 */
export function formatCoordinates(lat, lon, options = {}) {
    const { precision = 6, directionLabels = true, separator = ', ' } = options;

    const formatNumber = (num, isLat) => {
        const absNum = Math.abs(num);
        const degrees = Math.floor(absNum);
        const minutes = (absNum - degrees) * 60;
        const seconds = (minutes - Math.floor(minutes)) * 60;

        if (options.format === 'dms') {
            const dir = isLat ? (num >= 0 ? 'N' : 'S') : num >= 0 ? 'E' : 'W';

            return `${degrees}° ${Math.floor(minutes)}′ ${seconds.toFixed(2)}″ ${directionLabels ? dir : ''}`.trim();
        } else {
            return num.toFixed(precision) + (directionLabels ? '°' : '');
        }
    };

    const latStr = formatNumber(lat, true);
    const lonStr = formatNumber(lon, false);

    return `${latStr}${separator}${lonStr}`;
}

/**
 * Normalizes longitude to the range [-180, 180].
 * @param {number} lon - Longitude in degrees
 * @returns {number} Normalized longitude
 */
export function normalizeLongitude(lon) {
    while (lon < -180) lon += 360;
    while (lon > 180) lon -= 360;
    return lon;
}

/**
 * Normalizes latitude to the range [-90, 90].
 * @param {number} lat - Latitude in degrees
 * @returns {number} Normalized latitude
 */
export function normalizeLatitude(lat) {
    if (lat < -90) return -90;
    if (lat > 90) return 90;
    return lat;
}

/**
 * Calculates the scale denominator for a given zoom level.
 * @param {number} zoom - Zoom level
 * @param {number} dpi - DPI (default 96)
 * @returns {number} Scale denominator
 */
export function zoomToScale(zoom, dpi = 96) {
    const metersPerPixel = (156543.03392 * Math.cos(0)) / Math.pow(2, zoom);
    return (metersPerPixel * dpi) / 0.0254;
}

/**
 * Calculates zoom level from a scale denominator.
 * @param {number} scale - Scale denominator
 * @param {number} dpi - DPI (default 96)
 * @returns {number} Zoom level
 */
export function scaleToZoom(scale, dpi = 96) {
    const metersPerPixel = (scale * 0.0254) / dpi;
    return Math.log2((156543.03392 * Math.cos(0)) / metersPerPixel);
}

// Default export
export default {
    degreesToRadians,
    radiansToDegrees,
    wgs84ToWebMercator,
    webMercatorToWgs84,
    haversineDistance,
    calculateBearing,
    calculateDestination,
    bboxToPolygon,
    pointInBbox,
    bboxCenter,
    bboxArea,
    lat2tile,
    lon2tile,
    tile2lat,
    tile2lon,
    tile2bbox,
    formatCoordinates,
    normalizeLongitude,
    normalizeLatitude,
    zoomToScale,
    scaleToZoom,
};
