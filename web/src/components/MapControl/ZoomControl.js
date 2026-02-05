// Zoom Control - Custom zoom control for Leaflet maps

import { EventEmitter } from '../../services/utils/EventEmitter.js';

export class ZoomControl extends EventEmitter {
    constructor(options = {}) {
        super();

        this.options = {
            map: options.map,
            position: options.position || 'topleft',
            zoomInText: options.zoomInText || '+',
            zoomOutText: options.zoomOutText || '−',
            zoomInTitle: options.zoomInTitle || 'Zoom in',
            zoomOutTitle: options.zoomOutTitle || 'Zoom out',
            container: options.container,
            className: options.className || 'leaflet-control-zoom',
            showZoomLevel: options.showZoomLevel !== false,
            zoomLevelFormat: options.zoomLevelFormat || ((z) => `Zoom: ${z}`),
            enableKeyboard: options.enableKeyboard !== false,
            enableWheel: options.enableWheel !== false,
            animationDuration: options.animationDuration || 250,
            ...options,
        };

        if (!this.options.map) {
            throw new Error('Map instance is required');
        }

        this.map = this.options.map;
        this.container = null;
        this.zoomInButton = null;
        this.zoomOutButton = null;
        this.zoomLevelDisplay = null;
        this.isAttached = false;

        // Current state
        this.currentZoom = this.map.getZoom();
        this.minZoom = this.map.getMinZoom();
        this.maxZoom = this.map.getMaxZoom();

        // Animation state
        this.isAnimating = false;
        this.animationTimeout = null;

        // Initialize
        this.init();

        console.debug('ZoomControl initialized');
    }

    /**
     * Initialize the zoom control
     */
    init() {
        // Create container
        this.createContainer();

        // Create zoom buttons
        this.createZoomButtons();

        // Create zoom level display if enabled
        if (this.options.showZoomLevel) {
            this.createZoomLevelDisplay();
        }

        // Attach to map if position specified
        if (this.options.position && !this.options.container) {
            this.attachToMap();
        } else if (this.options.container) {
            this.attachToContainer(this.options.container);
        }

        // Setup event listeners
        this.setupEventListeners();

        // Initial update
        this.updateZoomState();

        // Setup keyboard shortcuts if enabled
        if (this.options.enableKeyboard) {
            this.setupKeyboardShortcuts();
        }
    }

    /**
     * Create container element
     */
    createContainer() {
        this.container = L.DomUtil.create('div', this.options.className);

        // Prevent map click events from propagating through zoom control
        L.DomEvent.disableClickPropagation(this.container);

        // Add container class for styling
        L.DomUtil.addClass(this.container, 'custom-zoom-control');

        // Set container styles
        Object.assign(this.container.style, {
            backgroundColor: 'rgba(255, 255, 255, 0.8)',
            borderRadius: '4px',
            boxShadow: '0 1px 5px rgba(0,0,0,0.4)',
            margin: '10px',
            transition: 'opacity 0.2s ease',
            display: 'flex',
            flexDirection: 'column',
            alignItems: 'center',
        });
    }

    /**
     * Create zoom buttons
     */
    createZoomButtons() {
        // Zoom in button
        this.zoomInButton = L.DomUtil.create('a', 'leaflet-control-zoom-in', this.container);
        this.zoomInButton.innerHTML = this.options.zoomInText;
        this.zoomInButton.title = this.options.zoomInTitle;
        this.zoomInButton.href = '#';
        this.zoomInButton.setAttribute('role', 'button');
        this.zoomInButton.setAttribute('aria-label', this.options.zoomInTitle);

        // Zoom out button
        this.zoomOutButton = L.DomUtil.create('a', 'leaflet-control-zoom-out', this.container);
        this.zoomOutButton.innerHTML = this.options.zoomOutText;
        this.zoomOutButton.title = this.options.zoomOutTitle;
        this.zoomOutButton.href = '#';
        this.zoomOutButton.setAttribute('role', 'button');
        this.zoomOutButton.setAttribute('aria-label', this.options.zoomOutTitle);

        // Style buttons
        [this.zoomInButton, this.zoomOutButton].forEach((button) => {
            Object.assign(button.style, {
                display: 'block',
                width: '30px',
                height: '30px',
                lineHeight: '30px',
                textAlign: 'center',
                textDecoration: 'none',
                color: '#333',
                fontSize: '20px',
                fontWeight: 'bold',
                cursor: 'pointer',
                userSelect: 'none',
                borderBottom: '1px solid #ccc',
            });

            // Remove border from last button
            if (button === this.zoomOutButton) {
                button.style.borderBottom = 'none';
            }

            // Hover effects
            button.addEventListener('mouseover', () => {
                button.style.backgroundColor = '#f4f4f4';
            });

            button.addEventListener('mouseout', () => {
                button.style.backgroundColor = '';
            });

            // Active state
            button.addEventListener('mousedown', () => {
                button.style.backgroundColor = '#ddd';
            });

            button.addEventListener('mouseup', () => {
                button.style.backgroundColor = '#f4f4f4';
            });
        });

        // Add click handlers
        L.DomEvent.on(this.zoomInButton, 'click', (e) => {
            L.DomEvent.stopPropagation(e);
            L.DomEvent.preventDefault(e);
            this.zoomIn();
        });

        L.DomEvent.on(this.zoomOutButton, 'click', (e) => {
            L.DomEvent.stopPropagation(e);
            L.DomEvent.preventDefault(e);
            this.zoomOut();
        });
    }

    /**
     * Create zoom level display
     */
    createZoomLevelDisplay() {
        // Create separator
        const separator = L.DomUtil.create('div', 'zoom-control-separator', this.container);
        separator.style.cssText = `
            width: 100%;
            height: 1px;
            background-color: #ccc;
            margin: 0;
        `;

        // Create zoom level display
        this.zoomLevelDisplay = L.DomUtil.create('div', 'zoom-level-display', this.container);
        this.zoomLevelDisplay.title = 'Current zoom level';
        this.zoomLevelDisplay.setAttribute('aria-label', 'Current zoom level');

        Object.assign(this.zoomLevelDisplay.style, {
            display: 'block',
            width: '30px',
            height: '24px',
            lineHeight: '24px',
            textAlign: 'center',
            fontSize: '12px',
            fontWeight: 'bold',
            color: '#333',
            cursor: 'default',
            userSelect: 'none',
        });

        // Add click to center at current zoom
        L.DomEvent.on(this.zoomLevelDisplay, 'click', (e) => {
            L.DomEvent.stopPropagation(e);
            this.centerAtCurrentZoom();
        });

        // Add double click to fit bounds
        L.DomEvent.on(this.zoomLevelDisplay, 'dblclick', (e) => {
            L.DomEvent.stopPropagation(e);
            this.fitToVisibleBounds();
        });

        // Update initial display
        this.updateZoomLevelDisplay();
    }

    /**
     * Attach to map
     */
    attachToMap() {
        if (this.isAttached) {
            return;
        }

        // Create Leaflet control container
        const controlContainer = L.DomUtil.create('div', 'leaflet-control');
        controlContainer.appendChild(this.container);

        // Add to map at specified position
        const corner = this.map._controlCorners[this.options.position];
        if (corner) {
            corner.appendChild(controlContainer);
            this.isAttached = true;

            this.emit('attached', { position: this.options.position });
        }
    }

    /**
     * Attach to custom container
     */
    attachToContainer(container) {
        if (this.isAttached) {
            return;
        }

        if (typeof container === 'string') {
            const element = document.querySelector(container);
            if (element) {
                element.appendChild(this.container);
                this.isAttached = true;
            }
        } else if (container instanceof HTMLElement) {
            container.appendChild(this.container);
            this.isAttached = true;
        }

        if (this.isAttached) {
            this.emit('attached', { container: container });
        }
    }

    /**
     * Setup event listeners
     */
    setupEventListeners() {
        // Listen to map zoom events
        this.map.on('zoomstart', () => this.onZoomStart());
        this.map.on('zoomend', () => this.onZoomEnd());
        this.map.on('zoomlevelschange', () => this.onZoomLevelsChange());

        // Update zoom level display on zoom
        this.map.on('zoom', () => this.onZoom());

        // Mouse wheel zoom if enabled
        if (this.options.enableWheel) {
            this.setupWheelZoom();
        }
    }

    /**
     * Setup keyboard shortcuts
     */
    setupKeyboardShortcuts() {
        document.addEventListener('keydown', (e) => {
            // Only handle if not focused on input elements
            if (e.target.tagName === 'INPUT' || e.target.tagName === 'TEXTAREA') {
                return;
            }

            switch (e.key) {
                case '+':
                case '=':
                    e.preventDefault();
                    this.zoomIn();
                    break;

                case '-':
                case '_':
                    e.preventDefault();
                    this.zoomOut();
                    break;

                case '0':
                    if (e.ctrlKey) {
                        e.preventDefault();
                        this.zoomTo(0);
                    }
                    break;

                case 'Home':
                    e.preventDefault();
                    this.zoomToMin();
                    break;

                case 'End':
                    e.preventDefault();
                    this.zoomToMax();
                    break;
            }
        });
    }

    /**
     * Setup mouse wheel zoom
     */
    setupWheelZoom() {
        L.DomEvent.on(this.container, 'wheel', (e) => {
            L.DomEvent.stopPropagation(e);

            if (e.deltaY < 0) {
                this.zoomIn();
            } else {
                this.zoomOut();
            }
        });
    }

    /**
     * Handle zoom start
     */
    onZoomStart() {
        this.isAnimating = true;

        // Disable buttons during animation
        this.setButtonsEnabled(false);

        this.emit('zoomStart');
    }

    /**
     * Handle zoom end
     */
    onZoomEnd() {
        this.isAnimating = false;

        // Re-enable buttons
        this.setButtonsEnabled(true);

        // Update zoom state
        this.updateZoomState();

        this.emit('zoomEnd');
    }

    /**
     * Handle zoom
     */
    onZoom() {
        this.currentZoom = this.map.getZoom();

        // Update zoom level display
        this.updateZoomLevelDisplay();

        // Update button states
        this.updateButtonStates();

        this.emit('zoomChanged', { zoom: this.currentZoom });
    }

    /**
     * Handle zoom levels change
     */
    onZoomLevelsChange() {
        this.minZoom = this.map.getMinZoom();
        this.maxZoom = this.map.getMaxZoom();

        this.updateButtonStates();

        this.emit('zoomLevelsChanged', {
            minZoom: this.minZoom,
            maxZoom: this.maxZoom,
        });
    }

    /**
     * Zoom in
     */
    zoomIn() {
        if (this.isAnimating || this.currentZoom >= this.maxZoom) {
            return;
        }

        const newZoom = this.currentZoom + 1;
        this.zoomTo(newZoom);
    }

    /**
     * Zoom out
     */
    zoomOut() {
        if (this.isAnimating || this.currentZoom <= this.minZoom) {
            return;
        }

        const newZoom = this.currentZoom - 1;
        this.zoomTo(newZoom);
    }

    /**
     * Zoom to specific level
     */
    zoomTo(zoomLevel) {
        if (this.isAnimating) {
            return;
        }

        // Clamp zoom level to valid range
        const clampedZoom = Math.max(this.minZoom, Math.min(this.maxZoom, zoomLevel));

        if (clampedZoom === this.currentZoom) {
            return;
        }

        // Set zoom with animation
        this.map.setZoom(clampedZoom, {
            animate: true,
            duration: this.options.animationDuration / 1000,
        });

        this.emit('zoomTo', { zoom: clampedZoom });
    }

    /**
     * Zoom to minimum level
     */
    zoomToMin() {
        this.zoomTo(this.minZoom);
    }

    /**
     * Zoom to maximum level
     */
    zoomToMax() {
        this.zoomTo(this.maxZoom);
    }

    /**
     * Center map at current zoom level
     */
    centerAtCurrentZoom() {
        const center = this.map.getCenter();
        this.map.setView(center, this.currentZoom, {
            animate: true,
            duration: this.options.animationDuration / 1000,
        });

        this.emit('centered', { center, zoom: this.currentZoom });
    }

    /**
     * Fit map to visible bounds
     */
    fitToVisibleBounds() {
        const bounds = this.map.getBounds();
        this.map.fitBounds(bounds, {
            animate: true,
            duration: this.options.animationDuration / 1000,
            padding: [50, 50],
        });

        this.emit('fittedToBounds', { bounds });
    }

    /**
     * Update zoom state
     */
    updateZoomState() {
        this.currentZoom = this.map.getZoom();
        this.minZoom = this.map.getMinZoom();
        this.maxZoom = this.map.getMaxZoom();

        this.updateZoomLevelDisplay();
        this.updateButtonStates();
    }

    /**
     * Update zoom level display
     */
    updateZoomLevelDisplay() {
        if (!this.zoomLevelDisplay) {
            return;
        }

        const zoomLevel = this.currentZoom;
        const formattedZoom = this.options.zoomLevelFormat(zoomLevel);

        this.zoomLevelDisplay.textContent = formattedZoom;
        this.zoomLevelDisplay.title = `Zoom level: ${zoomLevel} (min: ${this.minZoom}, max: ${this.maxZoom})`;
    }

    /**
     * Update button states
     */
    updateButtonStates() {
        if (!this.zoomInButton || !this.zoomOutButton) {
            return;
        }

        // Zoom in button
        if (this.currentZoom >= this.maxZoom) {
            this.zoomInButton.style.opacity = '0.5';
            this.zoomInButton.style.cursor = 'not-allowed';
            this.zoomInButton.title = 'Maximum zoom reached';
        } else {
            this.zoomInButton.style.opacity = '1';
            this.zoomInButton.style.cursor = 'pointer';
            this.zoomInButton.title = this.options.zoomInTitle;
        }

        // Zoom out button
        if (this.currentZoom <= this.minZoom) {
            this.zoomOutButton.style.opacity = '0.5';
            this.zoomOutButton.style.cursor = 'not-allowed';
            this.zoomOutButton.title = 'Minimum zoom reached';
        } else {
            this.zoomOutButton.style.opacity = '1';
            this.zoomOutButton.style.cursor = 'pointer';
            this.zoomOutButton.title = this.options.zoomOutTitle;
        }
    }

    /**
     * Set buttons enabled/disabled
     */
    setButtonsEnabled(enabled) {
        if (!this.zoomInButton || !this.zoomOutButton) {
            return;
        }

        if (enabled) {
            this.zoomInButton.style.pointerEvents = 'auto';
            this.zoomOutButton.style.pointerEvents = 'auto';
        } else {
            this.zoomInButton.style.pointerEvents = 'none';
            this.zoomOutButton.style.pointerEvents = 'none';
        }
    }

    /**
     * Show the zoom control
     */
    show() {
        if (this.container) {
            this.container.style.display = 'flex';
            this.container.style.opacity = '1';

            this.emit('visibilityChanged', { visible: true });
        }
    }

    /**
     * Hide the zoom control
     */
    hide() {
        if (this.container) {
            this.container.style.opacity = '0';

            // Wait for transition to complete before hiding
            setTimeout(() => {
                this.container.style.display = 'none';
            }, 200);

            this.emit('visibilityChanged', { visible: false });
        }
    }

    /**
     * Toggle visibility
     */
    toggleVisibility() {
        if (this.container.style.display === 'none') {
            this.show();
        } else {
            this.hide();
        }
    }

    /**
     * Set zoom range
     */
    setZoomRange(minZoom, maxZoom) {
        this.minZoom = minZoom;
        this.maxZoom = maxZoom;

        // Update map zoom range
        this.map.setMinZoom(minZoom);
        this.map.setMaxZoom(maxZoom);

        // Update button states
        this.updateButtonStates();

        this.emit('zoomRangeChanged', { minZoom, maxZoom });
    }

    /**
     * Get current zoom information
     */
    getZoomInfo() {
        return {
            zoom: this.currentZoom,
            minZoom: this.minZoom,
            maxZoom: this.maxZoom,
            canZoomIn: this.currentZoom < this.maxZoom,
            canZoomOut: this.currentZoom > this.minZoom,
            isAnimating: this.isAnimating,
        };
    }

    /**
     * Get control state
     */
    getState() {
        return {
            isAttached: this.isAttached,
            currentZoom: this.currentZoom,
            minZoom: this.minZoom,
            maxZoom: this.maxZoom,
            options: { ...this.options },
            isAnimating: this.isAnimating,
            visible: this.container ? this.container.style.display !== 'none' : false,
        };
    }

    /**
     * Clean up resources
     */
    destroy() {
        // Clear animation timeout
        if (this.animationTimeout) {
            clearTimeout(this.animationTimeout);
        }

        // Remove event listeners
        this.map.off('zoomstart', () => this.onZoomStart());
        this.map.off('zoomend', () => this.onZoomEnd());
        this.map.off('zoomlevelschange', () => this.onZoomLevelsChange());
        this.map.off('zoom', () => this.onZoom());

        // Remove keyboard shortcuts
        document.removeEventListener('keydown', (e) => {
            if (e.key === '+' || e.key === '=' || e.key === '-' || e.key === '_') {
                e.preventDefault();
            }
        });

        // Remove wheel event
        L.DomEvent.off(this.container, 'wheel');

        // Remove button event listeners
        L.DomEvent.off(this.zoomInButton);
        L.DomEvent.off(this.zoomOutButton);

        if (this.zoomLevelDisplay) {
            L.DomEvent.off(this.zoomLevelDisplay);
        }

        // Remove container from DOM
        if (this.container && this.container.parentNode) {
            this.container.parentNode.removeChild(this.container);
        }

        this.clear();

        console.debug('ZoomControl destroyed');
    }
}

// Default export
export default ZoomControl;
