// Scale Control - Custom scale control for Leaflet maps

import { EventEmitter } from '../../services/utils/EventEmitter.js';

export class ScaleControl extends EventEmitter {
    constructor(options = {}) {
        super();

        this.options = {
            map: options.map,
            position: options.position || 'bottomleft',
            maxWidth: options.maxWidth || 100,
            metric: options.metric !== false,
            imperial: options.imperial !== false,
            updateWhenIdle: options.updateWhenIdle !== false,
            container: options.container,
            className: options.className || 'leaflet-control-scale',
            ...options,
        };

        if (!this.options.map) {
            throw new Error('Map instance is required');
        }

        this.map = this.options.map;
        this.container = null;
        this.scaleElement = null;
        this.isAttached = false;
        this.currentScale = null;
        this.unitSystem = 'metric'; // 'metric' or 'imperial'

        // Initialize
        this.init();

        console.debug('ScaleControl initialized');
    }

    /**
     * Initialize the scale control
     */
    init() {
        // Create container
        this.createContainer();

        // Create scale element
        this.createScaleElement();

        // Attach to map if position specified
        if (this.options.position && !this.options.container) {
            this.attachToMap();
        } else if (this.options.container) {
            this.attachToContainer(this.options.container);
        }

        // Setup event listeners
        this.setupEventListeners();

        // Initial update
        this.updateScale();
    }

    /**
     * Create container element
     */
    createContainer() {
        this.container = L.DomUtil.create('div', this.options.className);

        // Prevent map click events from propagating through scale control
        L.DomEvent.disableClickPropagation(this.container);

        // Add container class for styling
        L.DomUtil.addClass(this.container, 'custom-scale-control');

        // Set container styles
        Object.assign(this.container.style, {
            backgroundColor: 'rgba(255, 255, 255, 0.8)',
            padding: '4px 8px',
            borderRadius: '4px',
            fontSize: '11px',
            fontWeight: 'bold',
            boxShadow: '0 1px 5px rgba(0,0,0,0.4)',
            margin: '10px',
            lineHeight: '1.4',
            whiteSpace: 'nowrap',
            maxWidth: `${this.options.maxWidth}px`,
            transition: 'opacity 0.2s ease',
        });
    }

    /**
     * Create scale element
     */
    createScaleElement() {
        this.scaleElement = L.DomUtil.create(
            'div',
            'leaflet-control-scale-content',
            this.container
        );

        // Create metric scale
        this.metricScale = L.DomUtil.create('div', 'scale-metric', this.scaleElement);

        // Create imperial scale if enabled
        if (this.options.imperial) {
            L.DomUtil.create('div', 'scale-separator', this.scaleElement).textContent = ' / ';
            this.imperialScale = L.DomUtil.create('div', 'scale-imperial', this.scaleElement);
        }

        // Add unit toggle if both systems enabled
        if (this.options.metric && this.options.imperial) {
            this.addUnitToggle();
        }

        // Add scale label
        this.addScaleLabel();
    }

    /**
     * Add unit toggle
     */
    addUnitToggle() {
        const toggleContainer = L.DomUtil.create('div', 'scale-unit-toggle', this.container);

        // Metric button
        this.metricButton = L.DomUtil.create(
            'button',
            'scale-unit-btn metric-btn',
            toggleContainer
        );
        this.metricButton.textContent = 'km';
        this.metricButton.title = 'Switch to metric units';

        // Imperial button
        this.imperialButton = L.DomUtil.create(
            'button',
            'scale-unit-btn imperial-btn',
            toggleContainer
        );
        this.imperialButton.textContent = 'mi';
        this.imperialButton.title = 'Switch to imperial units';

        // Style buttons
        [this.metricButton, this.imperialButton].forEach((btn) => {
            Object.assign(btn.style, {
                backgroundColor: 'transparent',
                border: '1px solid #ccc',
                borderRadius: '2px',
                padding: '1px 4px',
                margin: '0 2px',
                fontSize: '9px',
                cursor: 'pointer',
                outline: 'none',
            });

            btn.addEventListener('click', (e) => {
                e.stopPropagation();
                this.toggleUnitSystem();
            });

            btn.addEventListener('mouseover', () => {
                btn.style.backgroundColor = '#f0f0f0';
            });

            btn.addEventListener('mouseout', () => {
                btn.style.backgroundColor = 'transparent';
            });
        });

        // Update button states
        this.updateUnitButtons();
    }

    /**
     * Add scale label
     */
    addScaleLabel() {
        this.scaleLabel = L.DomUtil.create('div', 'scale-label', this.container);
        this.scaleLabel.textContent = 'Scale';
        this.scaleLabel.title = 'Map scale (distance on map to real distance)';

        Object.assign(this.scaleLabel.style, {
            fontSize: '9px',
            color: '#666',
            marginTop: '2px',
            textAlign: 'center',
            fontWeight: 'normal',
        });
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
        if (this.options.updateWhenIdle) {
            // Update scale only when map is idle
            this.map.on('moveend', () => this.updateScale());
        } else {
            // Update scale continuously
            this.map.on('move', () => this.updateScale());
        }

        // Update during zoom for real-time feedback
        this.map.on('zoom', () => this.updateScale());
        
        // Update on zoom end
        this.map.on('zoomend', () => this.updateScale());

        // Update on resize
        this.map.on('resize', () => this.updateScale());
    }

    /**
     * Update scale display
     */
    updateScale() {
        if (!this.isAttached || !this.map) {
            return;
        }

        const bounds = this.map.getBounds();
        const center = bounds.getCenter();
        const y = center.lat;
        const x = center.lng;

        // Calculate scale at current zoom level
        const maxMeters = this.getScaleInMeters();

        // Update metric scale
        if (this.options.metric && this.metricScale) {
            this.updateMetricScale(maxMeters);
        }

        // Update imperial scale
        if (this.options.imperial && this.imperialScale) {
            this.updateImperialScale(maxMeters);
        }

        // Update scale ratio
        this.updateScaleRatio(maxMeters);

        // Store current scale
        this.currentScale = {
            meters: maxMeters,
            unitSystem: this.unitSystem,
            zoom: this.map.getZoom(),
            center: { lat: y, lng: x },
            timestamp: new Date().toISOString(),
        };

        this.emit('scaleUpdated', this.currentScale);
    }

    /**
     * Get scale in meters at current zoom level
     */
    getScaleInMeters() {
        const zoom = this.map.getZoom();
        const center = this.map.getCenter();

        // Get map container size
        const container = this.map.getContainer();
        const width = container.clientWidth || 600;

        // Calculate approximate meters per pixel
        // This is a simplified calculation - in production, you might want more accuracy
        const metersPerPixel =
            (156543.03392 * Math.cos((center.lat * Math.PI) / 180)) / Math.pow(2, zoom);

        // Calculate meters for the scale width
        const scaleWidthPixels = this.options.maxWidth;
        const maxMeters = metersPerPixel * scaleWidthPixels;

        return maxMeters;
    }

    /**
     * Update metric scale display
     */
    updateMetricScale(meters) {
        let scaleText;
        let scaleValue;

        if (meters >= 1000) {
            // Show in kilometers
            scaleValue = meters / 1000;
            scaleText = this.formatScale(scaleValue, 'km');
        } else {
            // Show in meters
            scaleText = this.formatScale(meters, 'm');
        }

        this.metricScale.textContent = scaleText;
        this.metricScale.title = `${scaleText} (${meters.toFixed(0)} meters)`;
    }

    /**
     * Update imperial scale display
     */
    updateImperialScale(meters) {
        // Convert meters to feet
        const feet = meters * 3.28084;
        let scaleText;
        let scaleValue;

        if (feet >= 5280) {
            // Show in miles
            scaleValue = feet / 5280;
            scaleText = this.formatScale(scaleValue, 'mi');
        } else {
            // Show in feet
            scaleText = this.formatScale(feet, 'ft');
        }

        if (this.imperialScale) {
            this.imperialScale.textContent = scaleText;
            this.imperialScale.title = `${scaleText} (${feet.toFixed(0)} feet)`;
        }
    }

    /**
     * Update scale ratio
     */
    updateScaleRatio(meters) {
        if (!this.scaleLabel) {
            return;
        }

        const zoom = this.map.getZoom();
        const scaleRatio = this.calculateScaleRatio(meters);

        // Update label with scale ratio
        this.scaleLabel.textContent = `1:${scaleRatio.toLocaleString()}`;
        this.scaleLabel.title = `Scale ratio 1:${scaleRatio.toLocaleString()} at zoom ${zoom}`;
    }

    /**
     * Calculate scale ratio
     */
    calculateScaleRatio(meters) {
        const scaleWidthPixels = this.options.maxWidth;
        const metersPerPixel = meters / scaleWidthPixels;

        // Standard screen DPI (96) converted to meters per inch (0.0254 meters)
        const metersPerInch = 0.0254;
        const pixelsPerInch = 96;

        // Calculate scale ratio
        const scaleRatio = Math.round(1 / ((metersPerPixel * pixelsPerInch) / metersPerInch));

        return scaleRatio;
    }

    /**
     * Format scale value with appropriate unit
     */
    formatScale(value, unit) {
        if (value >= 100) {
            // Round to nearest 10, 100, etc.
            if (value >= 1000) {
                return `${Math.round(value / 100) * 100} ${unit}`;
            } else {
                return `${Math.round(value / 10) * 10} ${unit}`;
            }
        } else if (value >= 10) {
            return `${Math.round(value)} ${unit}`;
        } else {
            return `${value.toFixed(1)} ${unit}`;
        }
    }

    /**
     * Toggle unit system
     */
    toggleUnitSystem() {
        this.unitSystem = this.unitSystem === 'metric' ? 'imperial' : 'metric';

        // Update button states
        this.updateUnitButtons();

        // Update scale display
        this.updateScale();

        this.emit('unitSystemChanged', { unitSystem: this.unitSystem });
    }

    /**
     * Update unit toggle buttons
     */
    updateUnitButtons() {
        if (!this.metricButton || !this.imperialButton) {
            return;
        }

        if (this.unitSystem === 'metric') {
            this.metricButton.style.fontWeight = 'bold';
            this.metricButton.style.backgroundColor = '#e6f3ff';
            this.metricButton.style.borderColor = '#007cbf';

            this.imperialButton.style.fontWeight = 'normal';
            this.imperialButton.style.backgroundColor = 'transparent';
            this.imperialButton.style.borderColor = '#ccc';
        } else {
            this.imperialButton.style.fontWeight = 'bold';
            this.imperialButton.style.backgroundColor = '#e6f3ff';
            this.imperialButton.style.borderColor = '#007cbf';

            this.metricButton.style.fontWeight = 'normal';
            this.metricButton.style.backgroundColor = 'transparent';
            this.metricButton.style.borderColor = '#ccc';
        }
    }

    /**
     * Set unit system
     */
    setUnitSystem(system) {
        if (system !== 'metric' && system !== 'imperial') {
            console.warn('Invalid unit system. Use "metric" or "imperial".');
            return;
        }

        if (this.unitSystem !== system) {
            this.unitSystem = system;
            this.updateUnitButtons();
            this.updateScale();

            this.emit('unitSystemChanged', { unitSystem: this.unitSystem });
        }
    }

    /**
     * Show the scale control
     */
    show() {
        if (this.container) {
            this.container.style.display = 'block';
            this.container.style.opacity = '1';

            this.emit('visibilityChanged', { visible: true });
        }
    }

    /**
     * Hide the scale control
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
     * Set scale width
     */
    setScaleWidth(width) {
        this.options.maxWidth = width;

        if (this.container) {
            this.container.style.maxWidth = `${width}px`;
        }

        this.updateScale();

        this.emit('scaleWidthChanged', { width });
    }

    /**
     * Get current scale information
     */
    getScaleInfo() {
        return this.currentScale ? { ...this.currentScale } : null;
    }

    /**
     * Get control state
     */
    getState() {
        return {
            isAttached: this.isAttached,
            unitSystem: this.unitSystem,
            options: { ...this.options },
            currentScale: this.currentScale ? { ...this.currentScale } : null,
            visible: this.container ? this.container.style.display !== 'none' : false,
        };
    }

    /**
     * Clean up resources
     */
    destroy() {
        // Remove event listeners
        this.map.off('moveend', () => this.updateScale());
        this.map.off('move', () => this.updateScale());
        this.map.off('zoomend', () => this.updateScale());
        this.map.off('resize', () => this.updateScale());

        // Remove buttons event listeners
        if (this.metricButton) {
            const newMetricButton = this.metricButton.cloneNode(true);
            this.metricButton.parentNode.replaceChild(newMetricButton, this.metricButton);
        }

        if (this.imperialButton) {
            const newImperialButton = this.imperialButton.cloneNode(true);
            this.imperialButton.parentNode.replaceChild(newImperialButton, this.imperialButton);
        }

        // Remove container from DOM
        if (this.container && this.container.parentNode) {
            this.container.parentNode.removeChild(this.container);
        }

        this.clear();

        console.debug('ScaleControl destroyed');
    }
}

// Default export
export default ScaleControl;
