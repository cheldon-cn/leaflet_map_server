// Tile Layer Manager - Manages tile-based map layers

import { EventEmitter } from '../../services/utils/EventEmitter.js';

export class TileLayerManager extends EventEmitter {
    constructor(options = {}) {
        super();

        this.options = {
            map: options.map,
            baseLayers: options.baseLayers || {},
            overlayLayers: options.overlayLayers || {},
            defaultBaseLayer: options.defaultBaseLayer || 'esri-satellite',
            ...options,
        };

        if (!this.options.map) {
            throw new Error('Map instance is required');
        }

        this.map = this.options.map;
        this.layers = new Map();
        this.activeBaseLayer = null;
        this.activeOverlays = new Set();
        this.layerControls = null;

        // Layer configurations
        this.baseLayerConfigs = this.options.baseLayers;
        this.overlayLayerConfigs = this.options.overlayLayers;

        // Initialize base layers
        this.initBaseLayers();

        // Initialize overlay layers
        this.initOverlayLayers();

        // Setup layer control
        this.setupLayerControl();

        console.debug('TileLayerManager initialized');
    }

    /**
     * Initialize base layers
     */
    initBaseLayers() {
        // Default base layers if none provided
        if (Object.keys(this.baseLayerConfigs).length === 0) {
            this.baseLayerConfigs = {
                osm: {
                    name: 'OpenStreetMap',
                    layer: L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
                        attribution: '© OpenStreetMap contributors',
                        maxZoom: 19,
                    }),
                },
                'osm-hot': {
                    name: 'OpenStreetMap HOT',
                    layer: L.tileLayer('https://{s}.tile.openstreetmap.fr/hot/{z}/{x}/{y}.png', {
                        attribution:
                            '© OpenStreetMap contributors, Tiles style by Humanitarian OpenStreetMap Team',
                        maxZoom: 19,
                    }),
                },
                'esri-satellite': {
                    name: 'Esri Satellite',
                    layer: L.tileLayer(
                        'https://server.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer/tile/{z}/{y}/{x}',
                        {
                            attribution: '© Esri, Maxar, Earthstar Geographics',
                            maxZoom: 19,
                        }
                    ),
                },
                'gaode-vector': {
                    name: '高德矢量',
                    layer: L.tileLayer(
                        'https://webst01.is.autonavi.com/appmaptile?style=8&x={x}&y={y}&z={z}',
                        {
                            attribution: '© AutoNavi',
                            maxZoom: 19,
                        }
                    ),
                },
                'gaode-raster': {
                    name: '高德影像',
                    layer: L.tileLayer(
                        'https://webst01.is.autonavi.com/appmaptile?style=6&x={x}&y={y}&z={z}',
                        {
                            attribution: '© AutoNavi',
                            maxZoom: 19,
                        }
                    ),
                },
            };
        }

        // Create base layers
        for (const [id, config] of Object.entries(this.baseLayerConfigs)) {
            if (config.layer && !config.layer._leaflet_id) {
                // Add to map if it's the default layer
                if (id === this.options.defaultBaseLayer) {
                    config.layer.addTo(this.map);
                    this.activeBaseLayer = id;
                }

                this.layers.set(id, {
                    type: 'base',
                    config,
                    instance: config.layer,
                });
            }
        }
    }

    /**
     * Initialize overlay layers
     */
    initOverlayLayers() {
        // Create overlay layers if provided
        for (const [id, config] of Object.entries(this.overlayLayerConfigs)) {
            if (config.layer && !config.layer._leaflet_id) {
                this.layers.set(id, {
                    type: 'overlay',
                    config,
                    instance: config.layer,
                });
            }
        }
    }

    /**
     * Setup layer control
     */
    setupLayerControl() {
        const baseLayers = {};
        const overlays = {};

        // Collect layers for control
        for (const [id, layerInfo] of this.layers) {
            if (layerInfo.type === 'base') {
                baseLayers[layerInfo.config.name || id] = layerInfo.instance;
            } else if (layerInfo.type === 'overlay') {
                overlays[layerInfo.config.name || id] = layerInfo.instance;
            }
        }

        // Create layer control
        if (Object.keys(baseLayers).length > 1 || Object.keys(overlays).length > 0) {
            this.layerControls = L.control
                .layers(baseLayers, overlays, {
                    collapsed: true,
                    position: 'topright',
                })
                .addTo(this.map);

            // Listen to layer change events
            this.map.on('baselayerchange', (event) => this.onBaseLayerChange(event));
            this.map.on('overlayadd', (event) => this.onOverlayChange(event, true));
            this.map.on('overlayremove', (event) => this.onOverlayChange(event, false));
        }
    }

    /**
     * Handle base layer change
     */
    onBaseLayerChange(event) {
        // Find which base layer was selected
        for (const [id, layerInfo] of this.layers) {
            if (layerInfo.type === 'base' && layerInfo.instance === event.layer) {
                this.activeBaseLayer = id;
                this.emit('baseLayerChanged', {
                    layerId: id,
                    layerName: layerInfo.config.name || id,
                    layer: event.layer,
                });
                break;
            }
        }
    }

    /**
     * Handle overlay change
     */
    onOverlayChange(event, added) {
        for (const [id, layerInfo] of this.layers) {
            if (layerInfo.type === 'overlay' && layerInfo.instance === event.layer) {
                if (added) {
                    this.activeOverlays.add(id);
                } else {
                    this.activeOverlays.delete(id);
                }

                this.emit('overlayChanged', {
                    layerId: id,
                    layerName: layerInfo.config.name || id,
                    layer: event.layer,
                    added: added,
                });
                break;
            }
        }
    }

    /**
     * Add a new base layer
     */
    addBaseLayer(id, config) {
        if (this.layers.has(id)) {
            console.warn(`Layer with id "${id}" already exists`);
            return false;
        }

        if (!config.layer || !(config.layer instanceof L.Layer)) {
            console.error('Invalid layer instance');
            return false;
        }

        this.baseLayerConfigs[id] = config;

        const layerInfo = {
            type: 'base',
            config,
            instance: config.layer,
        };

        this.layers.set(id, layerInfo);

        // Update layer control
        this.updateLayerControl();

        this.emit('layerAdded', {
            layerId: id,
            layerType: 'base',
            layer: config.layer,
        });

        return true;
    }

    /**
     * Add a new overlay layer
     */
    addOverlayLayer(id, config) {
        if (this.layers.has(id)) {
            console.warn(`Layer with id "${id}" already exists`);
            return false;
        }

        if (!config.layer || !(config.layer instanceof L.Layer)) {
            console.error('Invalid layer instance');
            return false;
        }

        this.overlayLayerConfigs[id] = config;

        const layerInfo = {
            type: 'overlay',
            config,
            instance: config.layer,
        };

        this.layers.set(id, layerInfo);

        // Update layer control
        this.updateLayerControl();

        this.emit('layerAdded', {
            layerId: id,
            layerType: 'overlay',
            layer: config.layer,
        });

        return true;
    }

    /**
     * Remove a layer
     */
    removeLayer(id) {
        const layerInfo = this.layers.get(id);
        if (!layerInfo) {
            return false;
        }

        // Remove from map
        if (this.map.hasLayer(layerInfo.instance)) {
            this.map.removeLayer(layerInfo.instance);
        }

        // Remove from active sets
        if (layerInfo.type === 'base' && this.activeBaseLayer === id) {
            this.activeBaseLayer = null;
        } else if (layerInfo.type === 'overlay') {
            this.activeOverlays.delete(id);
        }

        // Remove from configs
        if (layerInfo.type === 'base') {
            delete this.baseLayerConfigs[id];
        } else {
            delete this.overlayLayerConfigs[id];
        }

        // Remove from layers map
        this.layers.delete(id);

        // Update layer control
        this.updateLayerControl();

        this.emit('layerRemoved', { layerId: id });

        return true;
    }

    /**
     * Update layer control with current layers
     */
    updateLayerControl() {
        // Remove existing control
        if (this.layerControls) {
            this.map.removeControl(this.layerControls);
        }

        // Recreate control
        this.setupLayerControl();
    }

    /**
     * Show a layer
     */
    showLayer(id) {
        const layerInfo = this.layers.get(id);
        if (!layerInfo) {
            return false;
        }

        if (!this.map.hasLayer(layerInfo.instance)) {
            if (layerInfo.type === 'base') {
                // For base layer, switch to it
                this.switchBaseLayer(id);
            } else {
                // For overlay, add it
                layerInfo.instance.addTo(this.map);
            }
        }

        return true;
    }

    /**
     * Hide a layer
     */
    hideLayer(id) {
        const layerInfo = this.layers.get(id);
        if (!layerInfo) {
            return false;
        }

        if (this.map.hasLayer(layerInfo.instance)) {
            this.map.removeLayer(layerInfo.instance);
        }

        return true;
    }

    /**
     * Switch base layer
     */
    switchBaseLayer(id) {
        const layerInfo = this.layers.get(id);
        if (!layerInfo || layerInfo.type !== 'base') {
            return false;
        }

        // Remove current base layer from map
        for (const [currentId, currentLayer] of this.layers) {
            if (currentLayer.type === 'base' && this.map.hasLayer(currentLayer.instance)) {
                this.map.removeLayer(currentLayer.instance);
            }
        }

        // Add new base layer
        layerInfo.instance.addTo(this.map);
        this.activeBaseLayer = id;

        // Update layer control state
        if (this.layerControls) {
            // Layer control will update automatically through events
        }

        this.emit('baseLayerSwitched', {
            layerId: id,
            layerName: layerInfo.config.name || id,
        });

        return true;
    }

    /**
     * Get layer by ID
     */
    getLayer(id) {
        const layerInfo = this.layers.get(id);
        return layerInfo ? layerInfo.instance : null;
    }

    /**
     * Get all layers
     */
    getAllLayers() {
        const result = {};

        for (const [id, layerInfo] of this.layers) {
            result[id] = {
                id,
                type: layerInfo.type,
                name: layerInfo.config.name || id,
                visible: this.map.hasLayer(layerInfo.instance),
                instance: layerInfo.instance,
            };
        }

        return result;
    }

    /**
     * Get base layers
     */
    getBaseLayers() {
        const result = {};

        for (const [id, layerInfo] of this.layers) {
            if (layerInfo.type === 'base') {
                result[id] = {
                    id,
                    name: layerInfo.config.name || id,
                    visible: this.map.hasLayer(layerInfo.instance),
                    active: this.activeBaseLayer === id,
                };
            }
        }

        return result;
    }

    /**
     * Get overlay layers
     */
    getOverlayLayers() {
        const result = {};

        for (const [id, layerInfo] of this.layers) {
            if (layerInfo.type === 'overlay') {
                result[id] = {
                    id,
                    name: layerInfo.config.name || id,
                    visible: this.map.hasLayer(layerInfo.instance),
                    active: this.activeOverlays.has(id),
                };
            }
        }

        return result;
    }

    /**
     * Check if layer is visible
     */
    isLayerVisible(id) {
        const layerInfo = this.layers.get(id);
        return layerInfo ? this.map.hasLayer(layerInfo.instance) : false;
    }

    /**
     * Set layer opacity
     */
    setLayerOpacity(id, opacity) {
        const layerInfo = this.layers.get(id);
        if (!layerInfo) {
            return false;
        }

        if (layerInfo.instance.setOpacity) {
            layerInfo.instance.setOpacity(opacity);
            return true;
        }

        return false;
    }

    /**
     * Set layer z-index
     */
    setLayerZIndex(id, zIndex) {
        const layerInfo = this.layers.get(id);
        if (!layerInfo) {
            return false;
        }

        if (layerInfo.instance.setZIndex) {
            layerInfo.instance.setZIndex(zIndex);
            return true;
        }

        return false;
    }

    /**
     * Get layer state
     */
    getLayerState(id) {
        const layerInfo = this.layers.get(id);
        if (!layerInfo) {
            return null;
        }

        const visible = this.map.hasLayer(layerInfo.instance);
        const opacity = layerInfo.instance.getOpacity ? layerInfo.instance.getOpacity() : 1.0;
        const zIndex = layerInfo.instance.getZIndex ? layerInfo.instance.getZIndex() : 0;

        return {
            id,
            type: layerInfo.type,
            name: layerInfo.config.name || id,
            visible,
            opacity,
            zIndex,
            active:
                layerInfo.type === 'base'
                    ? this.activeBaseLayer === id
                    : this.activeOverlays.has(id),
        };
    }

    /**
     * Get manager state
     */
    getState() {
        return {
            baseLayers: this.getBaseLayers(),
            overlayLayers: this.getOverlayLayers(),
            activeBaseLayer: this.activeBaseLayer,
            activeOverlays: Array.from(this.activeOverlays),
            totalLayers: this.layers.size,
        };
    }

    /**
     * Clean up resources
     */
    destroy() {
        // Remove layer control
        if (this.layerControls) {
            this.map.removeControl(this.layerControls);
        }

        // Remove event listeners
        this.map.off('baselayerchange', (event) => this.onBaseLayerChange(event));
        this.map.off('overlayadd', (event) => this.onOverlayChange(event, true));
        this.map.off('overlayremove', (event) => this.onOverlayChange(event, false));

        // Clear layers
        this.layers.clear();
        this.activeOverlays.clear();
        this.activeBaseLayer = null;

        this.clear();

        console.debug('TileLayerManager destroyed');
    }
}

// Default export
export default TileLayerManager;
