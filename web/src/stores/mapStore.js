// Map store - manages map state

import { EventEmitter } from '../services/utils/EventEmitter.js';

class MapStore extends EventEmitter {
    constructor() {
        super();

        // Initial state
        this.state = {
            // Map instance
            map: null,

            // Viewport state
            bounds: null,
            center: null,
            zoom: null,
            scale: null,

            // Image state
            imageUrl: null,
            imageSize: { width: 0, height: 0 },
            imageLoadTime: 0,
            imageTimestamp: null,

            // Render settings
            quality: 'medium',
            cacheStrategy: 'balanced',
            backgroundColor: '#ffffff',
            format: 'png',

            // Performance
            viewportUpdates: 0,
            lastUpdateTime: null,

            // Error state
            lastError: null,
            errorCount: 0,
        };

        // History for undo/redo
        this.history = [];
        this.historyIndex = -1;
        this.maxHistory = 20;

        console.debug('MapStore initialized');
    }

    /**
     * Initialize store
     */
    async init() {
        // Load saved state from localStorage
        await this.loadFromStorage();

        // Setup auto-save
        this.setupAutoSave();
    }

    /**
     * Get current state
     */
    getState() {
        return { ...this.state };
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
     * Set map instance
     */
    setMapInstance(map) {
        if (this.state.map === map) {
            return;
        }

        this.updateState({ map });
        this.emit('mapChanged', map);
    }

    /**
     * Set viewport state
     */
    setViewport(bounds, center, zoom) {
        const updates = {
            bounds,
            center,
            zoom,
            viewportUpdates: this.state.viewportUpdates + 1,
            lastUpdateTime: new Date().toISOString(),
        };

        // Calculate approximate scale
        if (bounds && zoom) {
            updates.scale = this.calculateScale(bounds, zoom);
        }

        this.updateState(updates);
        this.emit('viewportChanged', updates);
    }

    /**
     * Update image state
     */
    updateImageState(imageState) {
        const updates = {
            imageUrl: imageState.url,
            imageSize: imageState.size || { width: 0, height: 0 },
            imageLoadTime: imageState.loadTime || 0,
            imageTimestamp: imageState.timestamp || new Date().toISOString(),
        };

        this.updateState(updates);
        this.emit('imageUpdated', updates);
    }

    /**
     * Set quality setting
     */
    setQuality(quality) {
        if (this.state.quality === quality) {
            return;
        }

        this.updateState({ quality });
        this.emit('qualityChanged', quality);
    }

    /**
     * Set cache strategy
     */
    setCacheStrategy(strategy) {
        if (this.state.cacheStrategy === strategy) {
            return;
        }

        this.updateState({ cacheStrategy: strategy });
        this.emit('cacheStrategyChanged', strategy);
    }

    /**
     * Set background color
     */
    setBackgroundColor(color) {
        if (this.state.backgroundColor === color) {
            return;
        }

        this.updateState({ backgroundColor: color });
        this.emit('backgroundColorChanged', color);
    }

    /**
     * Set image format
     */
    setFormat(format) {
        if (this.state.format === format) {
            return;
        }

        this.updateState({ format });
        this.emit('formatChanged', format);
    }

    /**
     * Record error
     */
    recordError(error) {
        const updates = {
            lastError: {
                message: error.message,
                timestamp: new Date().toISOString(),
                stack: error.stack,
            },
            errorCount: this.state.errorCount + 1,
        };

        this.updateState(updates);
        this.emit('errorRecorded', updates);
    }

    /**
     * Clear errors
     */
    clearErrors() {
        this.updateState({
            lastError: null,
        });
        this.emit('errorsCleared');
    }

    /**
     * Reset store to initial state
     */
    reset() {
        const initialState = {
            // Map instance
            map: this.state.map, // Keep map instance

            // Viewport state
            bounds: null,
            center: null,
            zoom: null,
            scale: null,

            // Image state
            imageUrl: null,
            imageSize: { width: 0, height: 0 },
            imageLoadTime: 0,
            imageTimestamp: null,

            // Render settings
            quality: 'medium',
            cacheStrategy: 'balanced',
            backgroundColor: '#ffffff',
            format: 'png',

            // Performance
            viewportUpdates: 0,
            lastUpdateTime: null,

            // Error state
            lastError: null,
            errorCount: this.state.errorCount,
        };

        this.state = initialState;
        this.emit('storeReset', initialState);

        // Clear history
        this.history = [];
        this.historyIndex = -1;
    }

    // ========== Private Methods ==========

    /**
     * Update state with changes
     */
    updateState(updates) {
        const oldState = { ...this.state };
        const newState = { ...this.state, ...updates };

        // Create serializable copies for comparison (exclude non-serializable properties)
        const getSerializableState = (state) => {
            const serializable = { ...state };
            delete serializable.map;
            delete serializable.imageUrl;
            return serializable;
        };

        // Save to history if state actually changed
        if (
            JSON.stringify(getSerializableState(oldState)) !==
            JSON.stringify(getSerializableState(newState))
        ) {
            this.saveToHistory(oldState);
        }

        this.state = newState;
        this.emit('stateChanged', newState, updates);
    }

    /**
     * Save state to history
     */
    saveToHistory(state) {
        // Remove any forward history
        this.history = this.history.slice(0, this.historyIndex + 1);

        // Create serializable copy (exclude non-serializable properties)
        const serializableState = { ...state };
        delete serializableState.map;
        delete serializableState.imageUrl;

        // Add new state to history
        this.history.push(JSON.parse(JSON.stringify(serializableState)));

        // Limit history size
        if (this.history.length > this.maxHistory) {
            this.history.shift();
        } else {
            this.historyIndex++;
        }
    }

    /**
     * Calculate approximate map scale
     */
    calculateScale(bounds, zoom) {
        try {
            // Validate inputs
            if (!bounds || typeof bounds.getSouthWest !== 'function' || typeof bounds.getSouthEast !== 'function') {
                console.warn('Invalid bounds object for scale calculation');
                return null;
            }

            // Get center latitude for accurate meters per pixel calculation
            const center = bounds.getCenter();
            const latRad = (center.lat * Math.PI) / 180;
            
            // Calculate meters per pixel at given zoom level
            // Formula: 156543.03392 * cos(latitude) / 2^zoom
            const metersPerPixel = (156543.03392 * Math.cos(latRad)) / Math.pow(2, zoom);
            
            // Standard screen DPI and conversion factors
            const pixelsPerInch = 96;
            const metersPerInch = 0.0254;
            
            // Calculate scale denominator: 1 / (metersPerPixel * pixelsPerInch / metersPerInch)
            const scaleDenominator = Math.round(1 / ((metersPerPixel * pixelsPerInch) / metersPerInch));
            
            return scaleDenominator;
        } catch (error) {
            console.warn('Failed to calculate scale:', error);
            return null;
        }
    }

    /**
     * Calculate distance between two points in meters
     */
    calculateDistance(point1, point2) {
        try {
            // Validate inputs
            if (!point1 || !point2 || 
                typeof point1.lat !== 'number' || typeof point1.lng !== 'number' ||
                typeof point2.lat !== 'number' || typeof point2.lng !== 'number') {
                console.warn('Invalid point objects for distance calculation');
                return 0;
            }

            // Simplified Haversine formula
            const R = 6371000; // Earth's radius in meters
            const lat1 = (point1.lat * Math.PI) / 180;
            const lat2 = (point2.lat * Math.PI) / 180;
            const dLat = lat2 - lat1;
            const dLng = ((point2.lng - point1.lng) * Math.PI) / 180;

            const a =
                Math.sin(dLat / 2) * Math.sin(dLat / 2) +
                Math.cos(lat1) * Math.cos(lat2) * Math.sin(dLng / 2) * Math.sin(dLng / 2);

            const c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));

            return R * c;
        } catch (error) {
            console.warn('Failed to calculate distance:', error);
            return 0;
        }
    }

    /**
     * Setup auto-save to localStorage
     */
    setupAutoSave() {
        // Save on state changes with debounce
        let saveTimeout = null;

        this.on('stateChanged', (state, updates) => {
            if (saveTimeout) {
                clearTimeout(saveTimeout);
            }

            // Don't save everything - filter sensitive or temporary data
            const saveableUpdates = this.filterSaveableUpdates(updates);

            if (Object.keys(saveableUpdates).length > 0) {
                saveTimeout = setTimeout(() => {
                    this.saveToStorage(state);
                }, 1000);
            }
        });

        // Also save on page unload
        window.addEventListener('beforeunload', () => {
            this.saveToStorage(this.state);
        });
    }

    /**
     * Filter updates for storage
     */
    filterSaveableUpdates(updates) {
        const saveable = { ...updates };

        // Remove temporary or large data
        delete saveable.map;
        delete saveable.imageUrl; // Blob URLs are temporary

        return saveable;
    }

    /**
     * Save state to localStorage
     */
    saveToStorage(state) {
        try {
            // Create a copy and remove non-serializable properties before stringify
            const storageState = { ...state };
            delete storageState.map;
            delete storageState.imageUrl;

            localStorage.setItem('mapStore_state', JSON.stringify(storageState));

            console.debug('MapStore state saved to localStorage');
        } catch (error) {
            console.error('Failed to save MapStore state:', error);
        }
    }

    /**
     * Load state from localStorage
     */
    async loadFromStorage() {
        try {
            const saved = localStorage.getItem('mapStore_state');

            if (saved) {
                const parsed = JSON.parse(saved);

                // Merge with current state (excluding map instance)
                delete parsed.map;

                this.updateState(parsed);

                console.debug('MapStore state loaded from localStorage');
            }
        } catch (error) {
            console.error('Failed to load MapStore state:', error);
        }
    }

    // ========== History Methods ==========

    /**
     * Undo last state change
     */
    undo() {
        if (this.historyIndex < 0) {
            return false;
        }

        const previousState = this.history[this.historyIndex];
        this.historyIndex--;

        // Restore previous state
        const currentMap = this.state.map;
        const currentImageUrl = this.state.imageUrl;

        this.state = { ...previousState };
        this.state.map = currentMap; // Preserve map instance
        this.state.imageUrl = currentImageUrl; // Preserve current image URL

        this.emit('stateChanged', this.state, { isUndo: true });
        this.emit('undo', previousState);

        return true;
    }

    /**
     * Redo previously undone change
     */
    redo() {
        if (this.historyIndex >= this.history.length - 1) {
            return false;
        }

        this.historyIndex++;
        const nextState = this.history[this.historyIndex];

        // Restore next state
        const currentMap = this.state.map;
        const currentImageUrl = this.state.imageUrl;

        this.state = { ...nextState };
        this.state.map = currentMap; // Preserve map instance
        this.state.imageUrl = currentImageUrl; // Preserve current image URL

        this.emit('stateChanged', this.state, { isRedo: true });
        this.emit('redo', nextState);

        return true;
    }

    /**
     * Get history statistics
     */
    getHistoryStats() {
        return {
            total: this.history.length,
            currentIndex: this.historyIndex,
            canUndo: this.historyIndex >= 0,
            canRedo: this.historyIndex < this.history.length - 1,
        };
    }

    // ========== Persistence Methods ==========

    /**
     * Export state as JSON
     */
    exportState() {
        try {
            const exportData = {
                version: '1.0',
                timestamp: new Date().toISOString(),
                state: this.getState(),
            };

            // Clean up for export
            delete exportData.state.map;
            delete exportData.state.imageUrl;

            return JSON.stringify(exportData, null, 2);
        } catch (error) {
            console.error('Failed to export state:', error);
            return null;
        }
    }

    /**
     * Import state from JSON
     */
    importState(jsonString) {
        try {
            const importData = JSON.parse(jsonString);

            if (!importData.state) {
                throw new Error('Invalid import data: missing state');
            }

            // Validate version
            if (importData.version !== '1.0') {
                console.warn(
                    `Importing from version ${importData.version}, may have compatibility issues`
                );
            }

            // Update state
            this.updateState(importData.state);

            this.emit('stateImported', importData);

            return true;
        } catch (error) {
            console.error('Failed to import state:', error);
            return false;
        }
    }
}

// Create and export singleton instance
export const mapStore = new MapStore();

// Default export
export default mapStore;
