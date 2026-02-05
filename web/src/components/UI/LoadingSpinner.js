// Loading Spinner Component

import { EventEmitter } from '../../services/utils/EventEmitter.js';

export class LoadingSpinner extends EventEmitter {
    constructor(container) {
        super();

        this.container = container;
        this.isVisible = false;
        this.progress = 0;
        this.message = '';
        this.isIndeterminate = true;
        this.spinnerElement = null;
        this.progressElement = null;
        this.messageElement = null;

        // Ensure container exists
        if (!this.container) {
            console.warn('LoadingSpinner: No container provided, creating one');
            this.createContainer();
        }

        // Initialize
        this.init();

        console.debug('LoadingSpinner initialized');
    }

    /**
     * Initialize the spinner
     */
    init() {
        // Create spinner structure
        this.createSpinner();

        // Hide initially
        this.hide();
    }

    /**
     * Show the loading spinner
     * @param {Object} options - Display options
     */
    show(options = {}) {
        if (this.isVisible) {
            this.update(options);
            return;
        }

        this.isVisible = true;

        // Update options
        this.update(options);

        // Show container
        this.container.style.display = 'flex';

        // Animate in
        requestAnimationFrame(() => {
            this.container.classList.add('visible');
        });

        // Emit event
        this.emit('shown', {
            message: this.message,
            progress: this.progress,
            isIndeterminate: this.isIndeterminate,
        });
    }

    /**
     * Hide the loading spinner
     */
    hide() {
        if (!this.isVisible) return;

        this.isVisible = false;

        // Animate out
        this.container.classList.remove('visible');

        // Hide container after animation
        setTimeout(() => {
            if (!this.isVisible) {
                this.container.style.display = 'none';
            }
        }, 300);

        // Reset progress
        this.progress = 0;
        this.updateProgress();

        // Emit event
        this.emit('hidden');
    }

    /**
     * Update spinner display
     * @param {Object} options - Update options
     */
    update(options = {}) {
        const changes = {};

        // Update message
        if (options.message !== undefined) {
            this.message = options.message;
            this.updateMessage();
            changes.message = this.message;
        }

        // Update progress
        if (options.progress !== undefined) {
            this.progress = Math.max(0, Math.min(100, options.progress));
            this.updateProgress();
            changes.progress = this.progress;
        }

        // Update indeterminate state
        if (options.indeterminate !== undefined) {
            this.isIndeterminate = options.indeterminate;
            this.updateIndeterminate();
            changes.indeterminate = this.isIndeterminate;
        }

        // Update type (color/style)
        if (options.type) {
            this.updateType(options.type);
            changes.type = options.type;
        }

        // Emit event if there were changes
        if (Object.keys(changes).length > 0) {
            this.emit('updated', changes);
        }
    }

    /**
     * Set loading progress
     * @param {number} progress - Progress percentage (0-100)
     * @param {string} message - Optional progress message
     */
    setProgress(progress, message) {
        const options = { progress };

        if (message !== undefined) {
            options.message = message;
        }

        this.show(options);
    }

    /**
     * Set loading message
     * @param {string} message - Message to display
     */
    setMessage(message) {
        this.show({ message });
    }

    /**
     * Show indeterminate spinner
     * @param {string} message - Optional message
     */
    showIndeterminate(message) {
        this.show({
            message,
            indeterminate: true,
            progress: 0,
        });
    }

    /**
     * Show determinate spinner with progress
     * @param {number} progress - Initial progress (0-100)
     * @param {string} message - Optional message
     */
    showDeterminate(progress = 0, message) {
        this.show({
            message,
            indeterminate: false,
            progress,
        });
    }

    /**
     * Check if spinner is visible
     * @returns {boolean} - Visibility state
     */
    isShowing() {
        return this.isVisible;
    }

    /**
     * Get current progress
     * @returns {number} - Current progress percentage
     */
    getProgress() {
        return this.progress;
    }

    /**
     * Get current message
     * @returns {string} - Current message
     */
    getMessage() {
        return this.message;
    }

    /**
     * Set spinner type/color
     * @param {string} type - Type of spinner: 'primary', 'success', 'warning', 'error', 'info'
     */
    setType(type) {
        this.update({ type });
    }

    // ========== Private Methods ==========

    /**
     * Create container if not provided
     */
    createContainer() {
        const container = document.createElement('div');
        container.className = 'loading-spinner-container';
        container.style.cssText = `
            position: fixed;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background: rgba(0, 0, 0, 0.5);
            display: flex;
            align-items: center;
            justify-content: center;
            z-index: 9999;
            opacity: 0;
            transition: opacity 0.3s ease;
        `;

        document.body.appendChild(container);
        this.container = container;
    }

    /**
     * Create spinner structure
     */
    createSpinner() {
        // Clear container
        this.container.innerHTML = '';

        // Create spinner element
        this.spinnerElement = document.createElement('div');
        this.spinnerElement.className = 'spinner';
        this.spinnerElement.style.cssText = `
            width: 60px;
            height: 60px;
            border: 4px solid rgba(255, 255, 255, 0.3);
            border-radius: 50%;
            border-top-color: #3498db;
            animation: spin 1s ease-in-out infinite;
        `;

        // Create progress element
        this.progressElement = document.createElement('div');
        this.progressElement.className = 'spinner-progress';
        this.progressElement.style.cssText = `
            position: absolute;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
            font-size: 14px;
            font-weight: bold;
            color: white;
            text-shadow: 0 1px 2px rgba(0, 0, 0, 0.5);
        `;

        // Create message element
        this.messageElement = document.createElement('div');
        this.messageElement.className = 'spinner-message';
        this.messageElement.style.cssText = `
            margin-top: 20px;
            color: white;
            text-align: center;
            font-size: 16px;
            max-width: 300px;
            text-shadow: 0 1px 2px rgba(0, 0, 0, 0.5);
        `;

        // Create spinner content container
        const content = document.createElement('div');
        content.className = 'spinner-content';
        content.style.cssText = `
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            text-align: center;
        `;

        // Assemble the spinner
        const spinnerContainer = document.createElement('div');
        spinnerContainer.className = 'spinner-container';
        spinnerContainer.style.cssText = `
            position: relative;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
        `;

        spinnerContainer.appendChild(this.spinnerElement);
        spinnerContainer.appendChild(this.progressElement);

        content.appendChild(spinnerContainer);
        content.appendChild(this.messageElement);

        this.container.appendChild(content);

        // Add keyframes for animation
        this.addKeyframes();
    }

    /**
     * Add CSS keyframes for spinner animation
     */
    addKeyframes() {
        // Check if keyframes already exist
        if (document.getElementById('spinner-keyframes')) {
            return;
        }

        const style = document.createElement('style');
        style.id = 'spinner-keyframes';
        style.textContent = `
            @keyframes spin {
                to { transform: rotate(360deg); }
            }
            
            @keyframes spin-reverse {
                to { transform: rotate(-360deg); }
            }
            
            @keyframes pulse {
                0% { opacity: 1; }
                50% { opacity: 0.5; }
                100% { opacity: 1; }
            }
        `;

        document.head.appendChild(style);
    }

    /**
     * Update progress display
     */
    updateProgress() {
        if (!this.progressElement) return;

        if (this.isIndeterminate) {
            this.progressElement.textContent = '';
        } else {
            this.progressElement.textContent = `${Math.round(this.progress)}%`;
        }

        // Update spinner style based on progress
        if (this.isIndeterminate) {
            this.spinnerElement.style.animation = 'spin 1s ease-in-out infinite';
            this.spinnerElement.style.borderTopColor = this.getColorForType();
        } else {
            // For determinate progress, we could create a circular progress bar
            // This is a simplified version
            const rotation = (this.progress / 100) * 360;
            this.spinnerElement.style.background = `
                conic-gradient(
                    ${this.getColorForType()} 0deg ${rotation}deg,
                    rgba(255, 255, 255, 0.3) ${rotation}deg 360deg
                )
            `;
            this.spinnerElement.style.border = 'none';
            this.spinnerElement.style.animation = 'none';
        }
    }

    /**
     * Update message display
     */
    updateMessage() {
        if (!this.messageElement) return;

        this.messageElement.textContent = this.message;
        this.messageElement.style.display = this.message ? 'block' : 'none';
    }

    /**
     * Update indeterminate state
     */
    updateIndeterminate() {
        this.updateProgress();
    }

    /**
     * Update spinner type/color
     */
    updateType(type) {
        const color = this.getColorForType(type);

        // Update spinner color
        if (this.isIndeterminate) {
            this.spinnerElement.style.borderTopColor = color;
        } else {
            const rotation = (this.progress / 100) * 360;
            this.spinnerElement.style.background = `
                conic-gradient(
                    ${color} 0deg ${rotation}deg,
                    rgba(255, 255, 255, 0.3) ${rotation}deg 360deg
                )
            `;
        }

        // Update progress text color if needed
        this.progressElement.style.color = this.getTextColorForType(type);
    }

    /**
     * Get color for spinner type
     */
    getColorForType(type = 'primary') {
        const colors = {
            primary: '#3498db',
            success: '#2ecc71',
            warning: '#f39c12',
            error: '#e74c3c',
            info: '#3498db',
        };

        return colors[type] || colors.primary;
    }

    /**
     * Get text color for spinner type
     */
    getTextColorForType(type = 'primary') {
        const colors = {
            primary: '#ffffff',
            success: '#ffffff',
            warning: '#ffffff',
            error: '#ffffff',
            info: '#ffffff',
        };

        return colors[type] || colors.primary;
    }

    /**
     * Create and show a temporary spinner
     * @static
     * @param {Object} options - Spinner options
     * @returns {LoadingSpinner} - The created spinner instance
     */
    static showTemporary(options = {}) {
        const spinner = new LoadingSpinner();
        spinner.show(options);

        // Auto-hide after delay if specified
        if (options.duration) {
            setTimeout(() => spinner.hide(), options.duration);
        }

        return spinner;
    }

    /**
     * Show a blocking spinner overlay
     * @static
     * @param {Object} options - Spinner options
     * @returns {Function} - A function to hide the spinner
     */
    static showBlocking(options = {}) {
        const spinner = new LoadingSpinner();

        // Make it blocking
        spinner.container.style.background = 'rgba(0, 0, 0, 0.8)';
        spinner.container.style.pointerEvents = 'all';

        spinner.show(options);

        // Return hide function
        return () => spinner.hide();
    }

    /**
     * Destroy the spinner
     */
    destroy() {
        this.hide();

        // Remove container if we created it
        if (this.container && this.container.className === 'loading-spinner-container') {
            this.container.remove();
        }

        // Emit event
        this.emit('destroyed');
    }
}

// Default export
export default LoadingSpinner;
