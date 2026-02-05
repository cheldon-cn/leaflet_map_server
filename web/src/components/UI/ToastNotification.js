// Toast Notification Component

import { EventEmitter } from '../../services/utils/EventEmitter.js';

export class ToastNotification extends EventEmitter {
    constructor(container) {
        super();

        this.container = container;
        this.toasts = new Map();
        this.maxToasts = 5;
        this.defaultDuration = 5000; // 5 seconds
        this.autoRemove = true;

        // Load templates
        this.template = document.getElementById('toast-template');
        if (!this.template) {
            this.createDefaultTemplate();
        }

        // Ensure container has proper styling
        this.setupContainer();

        console.debug('ToastNotification initialized');
    }

    /**
     * Show a toast notification
     * @param {string} message - The message to display
     * @param {string} type - Notification type: 'success', 'error', 'warning', 'info'
     * @param {Object} options - Additional options
     * @returns {string} - Toast ID
     */
    show(message, type = 'info', options = {}) {
        const toastId = `toast_${Date.now()}_${Math.random().toString(36).substr(2, 9)}`;

        const toastOptions = {
            duration: options.duration || this.defaultDuration,
            position: options.position || 'top-right',
            dismissible: options.dismissible !== false,
            autoRemove: options.autoRemove !== false,
            icon: options.icon,
            actions: options.actions || [],
            ...options,
        };

        // Create toast element
        const toastElement = this.createToastElement(toastId, message, type, toastOptions);

        // Add to container
        this.container.appendChild(toastElement);

        // Store toast info
        this.toasts.set(toastId, {
            element: toastElement,
            options: toastOptions,
            createdAt: Date.now(),
            timer: null,
        });

        // Limit number of toasts
        this.limitToasts();

        // Animate in
        this.animateIn(toastElement);

        // Setup auto-remove if enabled
        if (toastOptions.autoRemove && toastOptions.duration > 0) {
            this.setupAutoRemove(toastId);
        }

        // Emit event
        this.emit('toastShown', { id: toastId, message, type, options: toastOptions });

        return toastId;
    }

    /**
     * Hide a specific toast
     * @param {string} toastId - The ID of the toast to hide
     */
    hide(toastId) {
        const toast = this.toasts.get(toastId);
        if (!toast) return;

        this.removeToast(toastId, 'dismissed');
    }

    /**
     * Hide all toasts
     */
    hideAll() {
        for (const toastId of this.toasts.keys()) {
            this.hide(toastId);
        }
    }

    /**
     * Update an existing toast
     * @param {string} toastId - The ID of the toast to update
     * @param {Object} updates - Updates to apply
     */
    update(toastId, updates) {
        const toast = this.toasts.get(toastId);
        if (!toast) return;

        const { element } = toast;

        // Update message if provided
        if (updates.message) {
            const messageElement = element.querySelector('.toast-message');
            if (messageElement) {
                messageElement.textContent = updates.message;
            }
        }

        // Update type if provided
        if (updates.type) {
            const oldType = element.className.match(/toast-\w+/)?.[0];
            if (oldType) {
                element.classList.remove(oldType);
            }
            element.classList.add(`toast-${updates.type}`);
        }

        // Update duration if provided
        if (updates.duration !== undefined) {
            toast.options.duration = updates.duration;

            // Reset auto-remove timer
            if (toast.timer) {
                clearTimeout(toast.timer);
                toast.timer = null;
            }

            if (toast.options.autoRemove && updates.duration > 0) {
                this.setupAutoRemove(toastId);
            }
        }

        // Emit event
        this.emit('toastUpdated', { id: toastId, updates });
    }

    /**
     * Get toast information
     * @param {string} toastId - The ID of the toast
     * @returns {Object|null} - Toast information or null if not found
     */
    getToast(toastId) {
        const toast = this.toasts.get(toastId);
        if (!toast) return null;

        return {
            id: toastId,
            message: toast.element.querySelector('.toast-message')?.textContent,
            type: toast.element.className.match(/toast-(\w+)/)?.[1] || 'info',
            createdAt: toast.createdAt,
            options: { ...toast.options },
        };
    }

    /**
     * Get all active toasts
     * @returns {Array} - Array of toast information objects
     */
    getAllToasts() {
        return Array.from(this.toasts.entries()).map(([id, toast]) => ({
            id,
            message: toast.element.querySelector('.toast-message')?.textContent,
            type: toast.element.className.match(/toast-(\w+)/)?.[1] || 'info',
            createdAt: toast.createdAt,
            options: { ...toast.options },
        }));
    }

    /**
     * Set default duration for new toasts
     * @param {number} duration - Duration in milliseconds
     */
    setDefaultDuration(duration) {
        this.defaultDuration = Math.max(0, duration);
    }

    /**
     * Set maximum number of visible toasts
     * @param {number} max - Maximum number of toasts
     */
    setMaxToasts(max) {
        this.maxToasts = Math.max(1, max);
        this.limitToasts();
    }

    // ========== Private Methods ==========

    /**
     * Setup container styling
     */
    setupContainer() {
        // Ensure container has proper classes
        this.container.classList.add('toast-container');

        // Ensure container is positioned
        const style = window.getComputedStyle(this.container);
        if (style.position === 'static') {
            this.container.style.position = 'relative';
        }
    }

    /**
     * Create toast element
     */
    createToastElement(toastId, message, type, options) {
        // Clone template
        const template = this.template.content.cloneNode(true);
        const toastElement = template.querySelector('.toast');

        // Set ID
        toastElement.id = toastId;

        // Set type class
        toastElement.classList.add(`toast-${type}`);

        // Set message
        const messageElement = toastElement.querySelector('.toast-message');
        if (messageElement) {
            messageElement.textContent = message;
        }

        // Set icon
        const iconElement = toastElement.querySelector('.toast-icon');
        if (iconElement) {
            const icon = this.getIconForType(type, options.icon);
            iconElement.textContent = icon;
        }

        // Setup close button
        const closeButton = toastElement.querySelector('.toast-close');
        if (closeButton && options.dismissible) {
            closeButton.addEventListener('click', () => {
                this.hide(toastId);
            });
        } else if (closeButton) {
            closeButton.style.display = 'none';
        }

        // Add actions if provided
        if (options.actions && options.actions.length > 0) {
            this.addActions(toastElement, toastId, options.actions);
        }

        return toastElement;
    }

    /**
     * Get icon for notification type
     */
    getIconForType(type, customIcon) {
        if (customIcon) return customIcon;

        const icons = {
            success: '✓',
            error: '✗',
            warning: '⚠',
            info: 'ℹ',
        };

        return icons[type] || 'ℹ';
    }

    /**
     * Add action buttons to toast
     */
    addActions(toastElement, toastId, actions) {
        const contentElement = toastElement.querySelector('.toast-content');
        if (!contentElement) return;

        const actionsContainer = document.createElement('div');
        actionsContainer.className = 'toast-actions';

        actions.forEach((action) => {
            const button = document.createElement('button');
            button.className = `toast-action ${action.className || ''}`;
            button.textContent = action.label;
            button.type = 'button';

            button.addEventListener('click', () => {
                action.handler(toastId);
                if (action.dismissOnClick !== false) {
                    this.hide(toastId);
                }
            });

            actionsContainer.appendChild(button);
        });

        contentElement.appendChild(actionsContainer);
    }

    /**
     * Animate toast in
     */
    animateIn(element) {
        // Add animation class
        element.classList.add('toast-entering');

        // Force reflow
        void element.offsetWidth;

        // Start animation
        element.classList.add('toast-visible');

        // Remove entering class after animation
        setTimeout(() => {
            element.classList.remove('toast-entering');
        }, 300);
    }

    /**
     * Animate toast out
     */
    animateOut(element) {
        return new Promise((resolve) => {
            element.classList.add('toast-exiting');
            element.classList.remove('toast-visible');

            // Wait for animation to complete
            setTimeout(() => {
                element.remove();
                resolve();
            }, 300);
        });
    }

    /**
     * Setup auto-remove timer
     */
    setupAutoRemove(toastId) {
        const toast = this.toasts.get(toastId);
        if (!toast) return;

        const { element, options } = toast;

        toast.timer = setTimeout(() => {
            this.removeToast(toastId, 'auto-removed');
        }, options.duration);

        // Pause on hover/focus
        element.addEventListener('mouseenter', () => {
            if (toast.timer) {
                clearTimeout(toast.timer);
                toast.timer = null;
            }
        });

        element.addEventListener('focusin', () => {
            if (toast.timer) {
                clearTimeout(toast.timer);
                toast.timer = null;
            }
        });

        // Resume on leave/blur
        element.addEventListener('mouseleave', () => {
            if (!toast.timer && options.autoRemove) {
                const elapsed = Date.now() - toast.createdAt;
                const remaining = Math.max(0, options.duration - elapsed);

                toast.timer = setTimeout(() => {
                    this.removeToast(toastId, 'auto-removed');
                }, remaining);
            }
        });

        element.addEventListener('focusout', () => {
            if (!toast.timer && options.autoRemove) {
                const elapsed = Date.now() - toast.createdAt;
                const remaining = Math.max(0, options.duration - elapsed);

                toast.timer = setTimeout(() => {
                    this.removeToast(toastId, 'auto-removed');
                }, remaining);
            }
        });
    }

    /**
     * Remove toast
     */
    removeToast(toastId, reason = 'removed') {
        const toast = this.toasts.get(toastId);
        if (!toast) return;

        const { element, timer } = toast;

        // Clear timer
        if (timer) {
            clearTimeout(timer);
        }

        // Animate out and remove
        this.animateOut(element).then(() => {
            this.toasts.delete(toastId);
            this.emit('toastRemoved', { id: toastId, reason });
        });
    }

    /**
     * Limit number of visible toasts
     */
    limitToasts() {
        if (this.toasts.size <= this.maxToasts) return;

        // Sort toasts by creation time (oldest first)
        const sortedIds = Array.from(this.toasts.entries())
            .sort((a, b) => a[1].createdAt - b[1].createdAt)
            .map(([id]) => id);

        // Remove oldest toasts until we're under the limit
        const toRemove = sortedIds.slice(0, this.toasts.size - this.maxToasts);

        toRemove.forEach((toastId) => {
            this.removeToast(toastId, 'limit-exceeded');
        });
    }

    /**
     * Create default template
     */
    createDefaultTemplate() {
        const template = document.createElement('template');
        template.id = 'toast-template';
        template.innerHTML = `
            <div class="toast" role="alert">
                <div class="toast-content">
                    <span class="toast-icon"></span>
                    <span class="toast-message"></span>
                </div>
                <button class="toast-close" aria-label="Close notification">×</button>
            </div>
        `;

        document.body.appendChild(template);
        this.template = template;
    }

    /**
     * Get toast statistics
     */
    getStats() {
        const now = Date.now();
        const activeToasts = Array.from(this.toasts.values());

        return {
            totalActive: activeToasts.length,
            oldestAge:
                activeToasts.length > 0
                    ? Math.round((now - Math.min(...activeToasts.map((t) => t.createdAt))) / 1000)
                    : 0,
            types: activeToasts.reduce((acc, toast) => {
                const type = toast.element.className.match(/toast-(\w+)/)?.[1] || 'info';
                acc[type] = (acc[type] || 0) + 1;
                return acc;
            }, {}),
        };
    }

    /**
     * Clear all toasts
     */
    clear() {
        for (const toastId of this.toasts.keys()) {
            this.removeToast(toastId, 'cleared');
        }
    }
}

// Default export
export default ToastNotification;
