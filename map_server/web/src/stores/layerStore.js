// Layer store - manages map layers state

import { EventEmitter } from '../services/utils/EventEmitter.js';
import { settings } from '../config/settings.js';

class LayerStore extends EventEmitter {
    constructor() {
        super();

        // Initial state with default layers
        this.state = {
            layers: this.getDefaultLayers(),
            activeLayerIds: new Set(['base', 'roads', 'buildings']),
            layerOrder: ['base', 'roads', 'buildings'],
            layerStyles: {
                roads: { color: '#ff0000', width: 2 },
                buildings: { fill: '#cccccc', stroke: '#999999', strokeWidth: 1 },
            },
            selectedLayerId: null,
            maxLayers: settings.LAYERS.maxLayers,

            // Metadata
            availableLayers: [], // Will be populated from server
            lastUpdate: null,
            isLoading: false,
        };

        console.debug('LayerStore initialized');
    }

    /**
     * Initialize store
     */
    async init() {
        // Load saved state from localStorage
        await this.loadFromStorage();

        // Fetch available layers from server (optional)
        await this.fetchAvailableLayers();

        // Setup auto-save
        this.setupAutoSave();
    }

    /**
     * Get current state
     */
    getState() {
        return {
            ...this.state,
            layers: [...this.state.layers],
            activeLayerIds: new Set(this.state.activeLayerIds),
        };
    }

    /**
     * Subscribe to state changes
     * @param {Function} callback - Function to call when state changes
     * @returns {Function} Unsubscribe function
     */
    subscribe(callback) {
        this.on('stateChanged', callback);
        return () => this.off('stateChanged', callback);
    }

    /**
     * Get default layers configuration
     */
    getDefaultLayers() {
        return settings.LAYERS.defaultLayers.map((layer) => ({
            ...layer,
            visible: true,
            opacity: 1.0,
            zIndex: this.getDefaultZIndex(layer.id),
        }));
    }

    /**
     * Get default z-index for layer
     */
    getDefaultZIndex(layerId) {
        const order = ['base', 'roads', 'buildings'];
        const index = order.indexOf(layerId);
        return index >= 0 ? index * 10 : 0;
    }

    /**
     * Add a new layer
     */
    addLayer(layerConfig) {
        const maxLayers = this.state.maxLayers;

        if (this.state.layers.length >= maxLayers) {
            throw new Error(`Maximum number of layers (${maxLayers}) reached`);
        }

        // Generate unique ID if not provided
        const layerId = layerConfig.id || this.generateLayerId(layerConfig.name);

        // Check for duplicate ID
        if (this.state.layers.some((layer) => layer.id === layerId)) {
            throw new Error(`Layer with ID "${layerId}" already exists`);
        }

        const newLayer = {
            id: layerId,
            name: layerConfig.name || 'New Layer',
            type: layerConfig.type || 'vector',
            source: layerConfig.source || 'database',
            visible: layerConfig.visible !== false,
            opacity: layerConfig.opacity || 1.0,
            zIndex: layerConfig.zIndex || this.getNextZIndex(),
            description: layerConfig.description || '',
            bounds: layerConfig.bounds || null,
            attributes: layerConfig.attributes || [],
        };

        const updatedLayers = [...this.state.layers, newLayer];
        const updatedActiveIds = new Set(this.state.activeLayerIds);
        const updatedOrder = [...this.state.layerOrder, layerId];

        if (newLayer.visible) {
            updatedActiveIds.add(layerId);
        }

        // Save custom style if provided
        if (layerConfig.style) {
            const updatedStyles = { ...this.state.layerStyles };
            updatedStyles[layerId] = layerConfig.style;
            this.updateState({ layerStyles: updatedStyles });
        }

        this.updateState({
            layers: updatedLayers,
            activeLayerIds: updatedActiveIds,
            layerOrder: updatedOrder,
        });

        this.emit('layerAdded', newLayer);

        return newLayer;
    }

    /**
     * Remove a layer
     */
    removeLayer(layerId) {
        const layerIndex = this.state.layers.findIndex((layer) => layer.id === layerId);

        if (layerIndex === -1) {
            return false;
        }

        const removedLayer = this.state.layers[layerIndex];
        const updatedLayers = this.state.layers.filter((layer) => layer.id !== layerId);
        const updatedActiveIds = new Set(this.state.activeLayerIds);
        const updatedOrder = this.state.layerOrder.filter((id) => id !== layerId);
        const updatedStyles = { ...this.state.layerStyles };

        updatedActiveIds.delete(layerId);
        delete updatedStyles[layerId];

        this.updateState({
            layers: updatedLayers,
            activeLayerIds: updatedActiveIds,
            layerOrder: updatedOrder,
            layerStyles: updatedStyles,
        });

        // Deselect if this layer was selected
        if (this.state.selectedLayerId === layerId) {
            this.updateState({ selectedLayerId: null });
        }

        this.emit('layerRemoved', removedLayer);

        return true;
    }

    /**
     * Update layer properties
     */
    updateLayer(layerId, updates) {
        const layerIndex = this.state.layers.findIndex((layer) => layer.id === layerId);

        if (layerIndex === -1) {
            throw new Error(`Layer with ID "${layerId}" not found`);
        }

        const updatedLayers = [...this.state.layers];
        const updatedActiveIds = new Set(this.state.activeLayerIds);

        // Apply updates
        updatedLayers[layerIndex] = {
            ...updatedLayers[layerIndex],
            ...updates,
        };

        // Update active layer IDs based on visibility
        if (updates.visible !== undefined) {
            if (updates.visible) {
                updatedActiveIds.add(layerId);
            } else {
                updatedActiveIds.delete(layerId);
            }
        }

        // Update styles if provided
        if (updates.style) {
            const updatedStyles = { ...this.state.layerStyles };
            updatedStyles[layerId] = updates.style;
            this.updateState({ layerStyles: updatedStyles });
        }

        const changedState = {
            layers: updatedLayers,
            activeLayerIds: updatedActiveIds,
        };

        this.updateState(changedState);

        this.emit('layerUpdated', {
            layerId,
            updates,
            layer: updatedLayers[layerIndex],
        });

        return updatedLayers[layerIndex];
    }

    /**
     * Toggle layer visibility
     */
    toggleLayerVisibility(layerId) {
        const layer = this.state.layers.find((layer) => layer.id === layerId);

        if (!layer) {
            return false;
        }

        return this.updateLayer(layerId, { visible: !layer.visible });
    }

    /**
     * Set layer opacity
     */
    setLayerOpacity(layerId, opacity) {
        return this.updateLayer(layerId, { opacity: Math.max(0, Math.min(1, opacity)) });
    }

    /**
     * Select a layer
     */
    selectLayer(layerId) {
        if (layerId && !this.state.layers.some((layer) => layer.id === layerId)) {
            throw new Error(`Layer with ID "${layerId}" not found`);
        }

        this.updateState({ selectedLayerId: layerId });
        this.emit('layerSelected', layerId);

        return true;
    }

    /**
     * Reorder layers
     */
    reorderLayers(newOrder) {
        // Validate new order
        const currentIds = new Set(this.state.layers.map((layer) => layer.id));
        const newOrderSet = new Set(newOrder);

        // Check that all IDs exist and no extra IDs are included
        if (newOrder.length !== currentIds.size || !newOrder.every((id) => currentIds.has(id))) {
            throw new Error('Invalid layer order');
        }

        // Update layers with new z-index values
        const updatedLayers = this.state.layers.map((layer) => ({
            ...layer,
            zIndex: newOrder.indexOf(layer.id) * 10,
        }));

        this.updateState({
            layers: updatedLayers,
            layerOrder: [...newOrder],
        });

        this.emit('layersReordered', newOrder);

        return true;
    }

    /**
     * Update layer style
     */
    updateLayerStyle(layerId, styleUpdates) {
        const currentStyle = this.state.layerStyles[layerId] || {};
        const updatedStyle = { ...currentStyle, ...styleUpdates };

        const updatedStyles = {
            ...this.state.layerStyles,
            [layerId]: updatedStyle,
        };

        this.updateState({ layerStyles: updatedStyles });

        this.emit('layerStyleUpdated', { layerId, style: updatedStyle });

        return updatedStyle;
    }

    /**
     * Get active layers
     */
    getActiveLayers() {
        return this.state.layers.filter((layer) => this.state.activeLayerIds.has(layer.id));
    }

    /**
     * Get layer by ID
     */
    getLayer(layerId) {
        return this.state.layers.find((layer) => layer.id === layerId);
    }

    /**
     * Check if layer is active
     */
    isLayerActive(layerId) {
        return this.state.activeLayerIds.has(layerId);
    }

    /**
     * Fetch available layers from server
     */
    async fetchAvailableLayers() {
        try {
            this.updateState({ isLoading: true });

            // This would be an API call in production
            // For now, simulate with mock data or use default layers

            const mockLayers = [
                {
                    id: 'water',
                    name: 'Water Bodies',
                    type: 'vector',
                    description: 'Rivers, lakes, and oceans',
                    bounds: null,
                    attributes: ['name', 'type', 'area'],
                },
                {
                    id: 'parks',
                    name: 'Parks and Green Spaces',
                    type: 'vector',
                    description: 'Public parks and recreational areas',
                    bounds: null,
                    attributes: ['name', 'type', 'area'],
                },
            ];

            this.updateState({
                availableLayers: mockLayers,
                isLoading: false,
                lastUpdate: new Date().toISOString(),
            });

            this.emit('availableLayersUpdated', mockLayers);
        } catch (error) {
            console.error('Failed to fetch available layers:', error);
            this.updateState({ isLoading: false });
            this.emit('availableLayersError', error);
        }
    }

    /**
     * Reset store to initial state
     */
    reset() {
        const initialState = {
            layers: this.getDefaultLayers(),
            activeLayerIds: new Set(['base', 'roads', 'buildings']),
            layerOrder: ['base', 'roads', 'buildings'],
            layerStyles: {
                roads: { color: '#ff0000', width: 2 },
                buildings: { fill: '#cccccc', stroke: '#999999', strokeWidth: 1 },
            },
            selectedLayerId: null,
            maxLayers: settings.LAYERS.maxLayers,
            availableLayers: [],
            lastUpdate: null,
            isLoading: false,
        };

        this.state = initialState;
        this.emit('storeReset', initialState);
    }

    // ========== Private Methods ==========

    /**
     * Update state with changes
     */
    updateState(updates) {
        const oldState = { ...this.state };
        const newState = { ...this.state, ...updates };

        // Ensure Set objects are properly handled
        if (updates.activeLayerIds && !(updates.activeLayerIds instanceof Set)) {
            newState.activeLayerIds = new Set(updates.activeLayerIds);
        }

        this.state = newState;
        this.emit('stateChanged', newState, updates);
    }

    /**
     * Generate unique layer ID
     */
    generateLayerId(baseName) {
        const sanitized = baseName.toLowerCase().replace(/[^a-z0-9]/g, '_');
        let id = sanitized;
        let counter = 1;

        while (this.state.layers.some((layer) => layer.id === id)) {
            id = `${sanitized}_${counter}`;
            counter++;
        }

        return id;
    }

    /**
     * Get next available z-index
     */
    getNextZIndex() {
        const maxZIndex = Math.max(...this.state.layers.map((layer) => layer.zIndex), 0);
        return maxZIndex + 10;
    }

    /**
     * Setup auto-save to localStorage
     */
    setupAutoSave() {
        let saveTimeout = null;

        this.on('stateChanged', () => {
            if (saveTimeout) {
                clearTimeout(saveTimeout);
            }

            saveTimeout = setTimeout(() => {
                this.saveToStorage();
            }, 2000);
        });

        // Save on page unload
        window.addEventListener('beforeunload', () => {
            this.saveToStorage();
        });
    }

    /**
     * Save state to localStorage
     */
    saveToStorage() {
        try {
            const storageState = {
                layers: this.state.layers,
                activeLayerIds: Array.from(this.state.activeLayerIds),
                layerOrder: this.state.layerOrder,
                layerStyles: this.state.layerStyles,
                selectedLayerId: this.state.selectedLayerId,
                lastUpdate: this.state.lastUpdate,
            };

            localStorage.setItem('layerStore_state', JSON.stringify(storageState));

            console.debug('LayerStore state saved to localStorage');
        } catch (error) {
            console.error('Failed to save LayerStore state:', error);
        }
    }

    /**
     * Load state from localStorage
     */
    async loadFromStorage() {
        try {
            const saved = localStorage.getItem('layerStore_state');

            if (saved) {
                const parsed = JSON.parse(saved);

                // Convert array back to Set
                if (parsed.activeLayerIds && Array.isArray(parsed.activeLayerIds)) {
                    parsed.activeLayerIds = new Set(parsed.activeLayerIds);
                }

                this.updateState(parsed);

                console.debug('LayerStore state loaded from localStorage');
            }
        } catch (error) {
            console.error('Failed to load LayerStore state:', error);
        }
    }

    // ========== Import/Export Methods ==========

    /**
     * Export layers configuration
     */
    exportLayers() {
        try {
            const exportData = {
                version: '1.0',
                timestamp: new Date().toISOString(),
                layers: this.state.layers,
                activeLayerIds: Array.from(this.state.activeLayerIds),
                layerOrder: this.state.layerOrder,
                layerStyles: this.state.layerStyles,
            };

            return JSON.stringify(exportData, null, 2);
        } catch (error) {
            console.error('Failed to export layers:', error);
            return null;
        }
    }

    /**
     * Import layers configuration
     */
    importLayers(jsonString) {
        try {
            const importData = JSON.parse(jsonString);

            if (!importData.layers) {
                throw new Error('Invalid import data: missing layers');
            }

            // Validate version
            if (importData.version !== '1.0') {
                console.warn(
                    `Importing from version ${importData.version}, may have compatibility issues`
                );
            }

            // Update state with imported data
            const updates = {
                layers: importData.layers.map((layer) => ({
                    ...layer,
                    visible: importData.activeLayerIds?.includes(layer.id) ?? layer.visible,
                    zIndex: layer.zIndex || this.getNextZIndex(),
                })),
                activeLayerIds: new Set(importData.activeLayerIds || []),
                layerOrder: importData.layerOrder || importData.layers.map((l) => l.id),
                layerStyles: importData.layerStyles || {},
            };

            this.updateState(updates);

            this.emit('layersImported', importData);

            return true;
        } catch (error) {
            console.error('Failed to import layers:', error);
            return false;
        }
    }
}

// Create and export singleton instance
export const layerStore = new LayerStore();

// Default export
export default layerStore;
