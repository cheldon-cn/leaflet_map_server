// Layer Switcher Component - UI for managing map layers

import { EventEmitter } from '../../services/utils/EventEmitter.js';

export class LayerSwitcher extends EventEmitter {
    constructor(options = {}) {
        super();

        this.options = {
            container: options.container,
            store: options.store,
            onLayerChange: options.onLayerChange || (() => {}),
            animationDuration: options.animationDuration || 300,
            maxVisibleLayers: options.maxVisibleLayers || 10,
            ...options,
        };

        if (!this.options.container) {
            throw new Error('Container element is required');
        }

        if (!this.options.store) {
            throw new Error('Store instance is required');
        }

        this.container = this.options.container;
        this.store = this.options.store;
        this.layers = [];
        this.isInitialized = false;
        this.dragState = null;

        // Templates
        this.layerItemTemplate = document.getElementById('layer-item-template');
        if (!this.layerItemTemplate) {
            console.warn('Layer item template not found, creating default');
            this.createDefaultTemplate();
        }

        console.debug('LayerSwitcher initialized');
    }

    /**
     * Initialize the layer switcher
     */
    init() {
        try {
            // Clear container
            this.container.innerHTML = '';

            // Load initial layers from store
            this.loadLayers();

            // Setup store subscription
            this.setupStoreSubscription();

            // Setup drag and drop
            this.setupDragAndDrop();

            this.isInitialized = true;

            this.emit('initialized');
        } catch (error) {
            console.error('Failed to initialize LayerSwitcher:', error);
            this.handleError(error, 'init');
        }
    }

    /**
     * Load layers from store
     */
    loadLayers() {
        const state = this.store.getState();
        this.layers = [...state.layers];

        this.renderLayers();
    }

    /**
     * Setup store subscription
     */
    setupStoreSubscription() {
        this.store.on('stateChanged', (newState, updates) => {
            if (updates.layers) {
                this.layers = [...newState.layers];
                this.renderLayers();
            }
        });
    }

    /**
     * Setup drag and drop
     */
    setupDragAndDrop() {
        this.container.addEventListener('mousedown', (e) => {
            const dragHandle = e.target.closest('.layer-drag-handle');
            if (dragHandle) {
                this.startDrag(e);
            }
        });

        this.container.addEventListener('touchstart', (e) => {
            const dragHandle = e.target.closest('.layer-drag-handle');
            if (dragHandle) {
                this.startDrag(e);
            }
        });

        document.addEventListener('mousemove', (e) => this.handleDrag(e));
        document.addEventListener('touchmove', (e) => this.handleDrag(e));

        document.addEventListener('mouseup', () => this.endDrag());
        document.addEventListener('touchend', () => this.endDrag());

        // Prevent default drag behavior
        this.container.addEventListener('dragstart', (e) => e.preventDefault());
    }

    /**
     * Render layers in the container
     */
    renderLayers() {
        // Clear container
        this.container.innerHTML = '';

        if (this.layers.length === 0) {
            this.showNoLayersMessage();
            return;
        }

        // Sort layers by z-index
        const sortedLayers = [...this.layers].sort((a, b) => b.zIndex - a.zIndex);

        // Create layer items
        sortedLayers.forEach((layer, index) => {
            const layerElement = this.createLayerElement(layer, index);
            this.container.appendChild(layerElement);
        });

        // Update statistics
        this.updateStats();
    }

    /**
     * Create layer element
     */
    createLayerElement(layer, index) {
        // Clone template
        const template = this.layerItemTemplate.content.cloneNode(true);
        const layerElement = template.querySelector('.layer-item');

        // Set layer ID
        layerElement.dataset.layerId = layer.id;
        layerElement.dataset.zIndex = layer.zIndex;

        // Set basic information
        const nameElement = layerElement.querySelector('.layer-name');
        const toggleElement = layerElement.querySelector('.layer-toggle');
        const opacityElement = layerElement.querySelector('.layer-opacity');

        if (nameElement) {
            nameElement.textContent = layer.name;
            nameElement.title = layer.description || layer.name;
        }

        if (toggleElement) {
            toggleElement.checked = layer.visible;
            toggleElement.addEventListener('change', (e) => {
                this.toggleLayerVisibility(layer.id, e.target.checked);
            });
        }

        if (opacityElement) {
            opacityElement.value = Math.round(layer.opacity * 100);
            opacityElement.addEventListener('input', (e) => {
                const opacity = parseInt(e.target.value) / 100;
                this.updateLayerOpacity(layer.id, opacity);
            });

            // Show opacity value
            const opacityValue = document.createElement('span');
            opacityValue.className = 'opacity-value';
            opacityValue.textContent = `${Math.round(layer.opacity * 100)}%`;
            opacityElement.parentNode.appendChild(opacityValue);

            // Update opacity value on input
            opacityElement.addEventListener('input', (e) => {
                opacityValue.textContent = `${e.target.value}%`;
            });
        }

        // Add edit button
        const editButton = this.createEditButton(layer);
        const actionsContainer = layerElement.querySelector('.layer-actions');
        if (actionsContainer) {
            actionsContainer.appendChild(editButton);
        }

        // Add drag handle
        const dragHandle = this.createDragHandle(layer);
        layerElement.prepend(dragHandle);

        // Add layer type indicator
        const typeIndicator = this.createTypeIndicator(layer);
        layerElement.prepend(typeIndicator);

        // Add event listeners
        this.setupLayerElementEvents(layerElement, layer);

        return layerElement;
    }

    /**
     * Create edit button
     */
    createEditButton(layer) {
        const button = document.createElement('button');
        button.className = 'btn-icon-small layer-edit';
        button.innerHTML = '✎';
        button.title = 'Edit layer';
        button.addEventListener('click', (e) => {
            e.stopPropagation();
            this.editLayer(layer.id);
        });
        return button;
    }

    /**
     * Create drag handle
     */
    createDragHandle(layer) {
        const handle = document.createElement('div');
        handle.className = 'layer-drag-handle';
        handle.innerHTML = '⋮⋮';
        handle.title = 'Drag to reorder';
        handle.style.cursor = 'grab';
        return handle;
    }

    /**
     * Create type indicator
     */
    createTypeIndicator(layer) {
        const indicator = document.createElement('span');
        indicator.className = 'layer-type-indicator';
        indicator.title = `Type: ${layer.type}`;

        // Different indicators for different layer types
        const indicators = {
            vector: '⬥',
            raster: '⬜',
            tile: '🟦',
            wms: '🌐',
        };

        indicator.textContent = indicators[layer.type] || '?';
        return indicator;
    }

    /**
     * Setup layer element events
     */
    setupLayerElementEvents(element, layer) {
        // Click to select
        element.addEventListener('click', (e) => {
            if (!e.target.closest('.btn-icon-small') && !e.target.closest('.layer-drag-handle')) {
                this.selectLayer(layer.id);
            }
        });

        // Double click to toggle visibility
        element.addEventListener('dblclick', () => {
            this.toggleLayerVisibility(layer.id);
        });
    }

    /**
     * Show no layers message
     */
    showNoLayersMessage() {
        const message = document.createElement('div');
        message.className = 'no-layers-message';
        message.innerHTML = `
            <div class="empty-state">
                <div class="empty-icon">🗺️</div>
                <h3>No Layers Available</h3>
                <p>Add layers to start displaying map data.</p>
                <button class="btn-secondary add-first-layer">Add First Layer</button>
            </div>
        `;

        message.querySelector('.add-first-layer').addEventListener('click', () => {
            this.emit('addFirstLayer');
        });

        this.container.appendChild(message);
    }

    /**
     * Update statistics display
     */
    updateStats() {
        const activeLayers = this.layers.filter((layer) => layer.visible);

        // Update stats in the UI if there's a stats container
        const statsContainer = document.querySelector('.layer-stats');
        if (statsContainer) {
            statsContainer.innerHTML = `
                <span class="stat-item">Total: ${this.layers.length}</span>
                <span class="stat-item">Active: ${activeLayers.length}</span>
            `;
        }
    }

    /**
     * Toggle layer visibility
     */
    toggleLayerVisibility(layerId, visible) {
        try {
            this.store.toggleLayerVisibility(layerId);
            this.emit('layerToggled', layerId);
        } catch (error) {
            this.handleError(error, 'toggleLayerVisibility');
        }
    }

    /**
     * Update layer opacity
     */
    updateLayerOpacity(layerId, opacity) {
        try {
            this.store.setLayerOpacity(layerId, opacity);
            this.emit('layerOpacityChanged', { layerId, opacity });
        } catch (error) {
            this.handleError(error, 'updateLayerOpacity');
        }
    }

    /**
     * Select layer
     */
    selectLayer(layerId) {
        try {
            this.store.selectLayer(layerId);
            this.emit('layerSelected', layerId);
        } catch (error) {
            this.handleError(error, 'selectLayer');
        }
    }

    /**
     * Edit layer
     */
    editLayer(layerId) {
        this.emit('layerEdit', layerId);
    }

    // ========== Drag and Drop Methods ==========

    /**
     * Start drag operation
     */
    startDrag(event) {
        event.preventDefault();

        const layerElement = event.target.closest('.layer-item');
        if (!layerElement) return;

        const layerId = layerElement.dataset.layerId;
        const layer = this.layers.find((l) => l.id === layerId);
        if (!layer) return;

        this.dragState = {
            layerId,
            startY: event.clientY || event.touches[0].clientY,
            layerElement,
            originalIndex: this.layers.indexOf(layer),
        };

        // Add visual feedback
        layerElement.classList.add('dragging');
        layerElement.style.opacity = '0.5';

        this.emit('dragStart', layerId);
    }

    /**
     * Handle drag movement
     */
    handleDrag(event) {
        if (!this.dragState) return;

        event.preventDefault();

        const clientY = event.clientY || (event.touches && event.touches[0].clientY);
        if (!clientY) return;

        // Calculate new position
        const deltaY = clientY - this.dragState.startY;
        this.dragState.layerElement.style.transform = `translateY(${deltaY}px)`;

        // Find possible drop target
        const layerElements = Array.from(
            this.container.querySelectorAll('.layer-item:not(.dragging)')
        );
        const dropTarget = layerElements.find((element) => {
            const rect = element.getBoundingClientRect();
            return clientY >= rect.top && clientY <= rect.bottom;
        });

        // Update visual feedback
        layerElements.forEach((element) => element.classList.remove('drop-above', 'drop-below'));

        if (dropTarget) {
            const rect = dropTarget.getBoundingClientRect();
            const midpoint = rect.top + rect.height / 2;

            if (clientY < midpoint) {
                dropTarget.classList.add('drop-above');
            } else {
                dropTarget.classList.add('drop-below');
            }
        }
    }

    /**
     * End drag operation
     */
    endDrag() {
        if (!this.dragState) return;

        const { layerId, layerElement } = this.dragState;

        // Remove visual feedback
        layerElement.classList.remove('dragging');
        layerElement.style.opacity = '';
        layerElement.style.transform = '';

        // Find drop position
        const layerElements = Array.from(this.container.querySelectorAll('.layer-item'));
        const dropTarget = layerElements.find(
            (element) =>
                element.classList.contains('drop-above') || element.classList.contains('drop-below')
        );

        if (dropTarget) {
            const isAbove = dropTarget.classList.contains('drop-above');
            const dropIndex = layerElements.indexOf(dropTarget);
            const newIndex = isAbove ? dropIndex : dropIndex + 1;

            // Reorder layers
            const layer = this.layers.find((l) => l.id === layerId);
            this.layers = this.layers.filter((l) => l.id !== layerId);
            this.layers.splice(newIndex, 0, layer);

            // Update z-indices
            this.updateLayerOrder();

            // Save to store
            this.store.reorderLayers(this.layers.map((l) => l.id));
        }

        // Clean up drop indicators
        layerElements.forEach((element) => {
            element.classList.remove('drop-above', 'drop-below');
        });

        this.emit('dragEnd', layerId);
        this.dragState = null;

        // Re-render to ensure proper ordering
        this.renderLayers();
    }

    /**
     * Update layer order based on z-index
     */
    updateLayerOrder() {
        this.layers.forEach((layer, index) => {
            layer.zIndex = (this.layers.length - index) * 10;
        });
    }

    /**
     * Create default template
     */
    createDefaultTemplate() {
        const template = document.createElement('template');
        template.id = 'layer-item-template';
        template.innerHTML = `
            <div class="layer-item">
                <div class="layer-header">
                    <div class="layer-controls">
                        <input type="checkbox" class="layer-toggle">
                        <span class="layer-name"></span>
                    </div>
                    <div class="layer-actions">
                        <!-- Edit button will be added here -->
                    </div>
                </div>
                <div class="layer-settings">
                    <div class="form-group">
                        <label>Opacity</label>
                        <input type="range" class="layer-opacity" min="0" max="100" value="100">
                    </div>
                </div>
            </div>
        `;

        document.body.appendChild(template);
        this.layerItemTemplate = template;
    }

    /**
     * Handle errors
     */
    handleError(error, context) {
        console.error(`LayerSwitcher error (${context}):`, error);

        this.emit('error', {
            error,
            context,
            timestamp: new Date().toISOString(),
        });
    }

    /**
     * Destroy the layer switcher
     */
    destroy() {
        // Clean up event listeners
        document.removeEventListener('mousemove', this.handleDrag);
        document.removeEventListener('touchmove', this.handleDrag);
        document.removeEventListener('mouseup', this.endDrag);
        document.removeEventListener('touchend', this.endDrag);

        // Clear container
        this.container.innerHTML = '';

        this.emit('destroyed');
    }
}

// Default export
export default LayerSwitcher;
