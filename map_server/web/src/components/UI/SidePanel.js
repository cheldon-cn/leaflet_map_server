// Side Panel - Collapsible side panel for UI controls

import { EventEmitter } from '../../services/utils/EventEmitter.js';

export class SidePanel extends EventEmitter {
    constructor(options = {}) {
        super();

        this.options = {
            container: options.container || document.body,
            position: options.position || 'right', // 'left' or 'right'
            width: options.width || 300,
            minWidth: options.minWidth || 200,
            maxWidth: options.maxWidth || 500,
            collapsedWidth: options.collapsedWidth || 48,
            defaultState: options.defaultState || 'collapsed', // 'expanded' or 'collapsed'
            animationDuration: options.animationDuration || 300,
            overlay: options.overlay !== false,
            overlayOpacity: options.overlayOpacity || 0.5,
            draggable: options.draggable !== false,
            resizable: options.resizable !== false,
            closeOnEsc: options.closeOnEsc !== false,
            closeOnOverlayClick: options.closeOnOverlayClick !== false,
            title: options.title || 'Panel',
            className: options.className || 'side-panel',
            ...options,
        };

        // State
        this.isInitialized = false;
        this.isExpanded = this.options.defaultState === 'expanded';
        this.isDragging = false;
        this.isResizing = false;
        this.currentWidth = this.isExpanded ? this.options.width : this.options.collapsedWidth;

        // DOM elements
        this.panel = null;
        this.overlay = null;
        this.header = null;
        this.content = null;
        this.footer = null;
        this.iconContainer = null;

        // Drag/resize state
        this.dragStartX = 0;
        this.dragStartWidth = 0;
        this.resizeStartX = 0;
        this.resizeStartWidth = 0;

        // Initialize
        this.init();

        console.debug('SidePanel initialized');
    }

    /**
     * Initialize the side panel
     */
    init() {
        // Create DOM elements
        this.createPanel();
        this.createOverlay();

        // Append to container
        this.appendToContainer();

        // Setup event listeners
        this.setupEventListeners();

        // Set initial state
        this.updatePanelState();

        this.isInitialized = true;

        this.emit('initialized');
    }

    /**
     * Create panel element
     */
    createPanel() {
        // Create main panel element
        this.panel = document.createElement('div');
        this.panel.className = `${this.options.className} ${this.options.className}--${this.options.position}`;

        // Set initial styles
        Object.assign(this.panel.style, {
            position: 'fixed',
            top: '64px',
            [this.options.position]: '0',
            width: `${this.currentWidth}px`,
            height: 'calc(100vh - 64px)',
            backgroundColor: '#ffffff',
            boxShadow: '0 0 20px rgba(0, 0, 0, 0.2)',
            zIndex: '1000',
            transition: `width ${this.options.animationDuration}ms ease`,
            display: 'flex',
            flexDirection: 'column',
            overflow: 'hidden',
        });

        // Create header
        this.createHeader();

        // Create icon container for collapsed state
        this.createIconContainer();

        // Create content area
        this.createContent();

        // Create footer
        this.createFooter();

        // Add resize handle if resizable
        if (this.options.resizable) {
            this.createResizeHandle();
        }
    }

    /**
     * Create header
     */
    createHeader() {
        this.header = document.createElement('div');
        this.header.className = `${this.options.className}__header`;

        Object.assign(this.header.style, {
            padding: '16px',
            backgroundColor: '#f5f5f5',
            borderBottom: '1px solid #e0e0e0',
            display: 'flex',
            alignItems: 'center',
            justifyContent: 'space-between',
            flexShrink: '0',
            minHeight: '60px',
            boxSizing: 'border-box',
        });

        // Title
        const title = document.createElement('h3');
        title.className = `${this.options.className}__title`;
        title.textContent = this.options.title;

        Object.assign(title.style, {
            margin: '0',
            fontSize: '18px',
            fontWeight: '600',
            color: '#333',
            flex: '1',
            overflow: 'hidden',
            textOverflow: 'ellipsis',
            whiteSpace: 'nowrap',
        });

        // Toggle button
        this.toggleButton = document.createElement('button');
        this.toggleButton.className = `${this.options.className}__toggle`;
        this.toggleButton.innerHTML = this.isExpanded ? '‹' : '›';
        this.toggleButton.title = this.isExpanded ? 'Collapse panel' : 'Expand panel';
        this.toggleButton.setAttribute(
            'aria-label',
            this.isExpanded ? 'Collapse panel' : 'Expand panel'
        );

        Object.assign(this.toggleButton.style, {
            width: '32px',
            height: '32px',
            borderRadius: '4px',
            border: '1px solid #ddd',
            backgroundColor: '#fff',
            cursor: 'pointer',
            fontSize: '16px',
            fontWeight: 'bold',
            display: 'flex',
            alignItems: 'center',
            justifyContent: 'center',
            marginLeft: '10px',
            flexShrink: '0',
        });

        // Close button (only shown when expanded on mobile)
        this.closeButton = document.createElement('button');
        this.closeButton.className = `${this.options.className}__close`;
        this.closeButton.innerHTML = '×';
        this.closeButton.title = 'Close panel';
        this.closeButton.setAttribute('aria-label', 'Close panel');

        Object.assign(this.closeButton.style, {
            width: '32px',
            height: '32px',
            borderRadius: '4px',
            border: '1px solid #ddd',
            backgroundColor: '#fff',
            cursor: 'pointer',
            fontSize: '20px',
            fontWeight: 'bold',
            display: 'none', // Hidden by default, shown on mobile
            alignItems: 'center',
            justifyContent: 'center',
            marginLeft: '10px',
            flexShrink: '0',
        });

        // Add elements to header
        this.header.appendChild(title);
        this.header.appendChild(this.toggleButton);
        this.header.appendChild(this.closeButton);

        // Add header to panel
        this.panel.appendChild(this.header);
    }

    /**
     * Create content area
     */
    createContent() {
        this.content = document.createElement('div');
        this.content.className = `${this.options.className}__content`;

        Object.assign(this.content.style, {
            flex: '1',
            overflow: 'auto',
            padding: '16px',
            boxSizing: 'border-box',
        });

        // Add default content if provided
        if (this.options.content) {
            if (typeof this.options.content === 'string') {
                this.content.innerHTML = this.options.content;
            } else if (this.options.content instanceof HTMLElement) {
                this.content.appendChild(this.options.content);
            }
        }

        // Add content to panel
        this.panel.appendChild(this.content);
    }

    /**
     * Create footer
     */
    createFooter() {
        this.footer = document.createElement('div');
        this.footer.className = `${this.options.className}__footer`;

        Object.assign(this.footer.style, {
            padding: '16px',
            backgroundColor: '#f5f5f5',
            borderTop: '1px solid #e0e0e0',
            flexShrink: '0',
            minHeight: '50px',
            boxSizing: 'border-box',
        });

        // Add footer to panel
        this.panel.appendChild(this.footer);
    }

    /**
     * Create resize handle
     */
    createResizeHandle() {
        this.resizeHandle = document.createElement('div');
        this.resizeHandle.className = `${this.options.className}__resize-handle`;

        const handlePosition = this.options.position === 'left' ? 'right' : 'left';

        Object.assign(this.resizeHandle.style, {
            position: 'absolute',
            top: '0',
            [handlePosition]: '0',
            width: '8px',
            height: '100%',
            cursor: 'col-resize',
            zIndex: '10',
            backgroundColor: 'transparent',
        });

        // Add hover effect
        this.resizeHandle.addEventListener('mouseenter', () => {
            this.resizeHandle.style.backgroundColor = 'rgba(0, 120, 212, 0.1)';
        });

        this.resizeHandle.addEventListener('mouseleave', () => {
            if (!this.isResizing) {
                this.resizeHandle.style.backgroundColor = 'transparent';
            }
        });

        // Add resize handle to panel
        this.panel.appendChild(this.resizeHandle);
    }

    /**
     * Create overlay
     */
    createOverlay() {
        if (!this.options.overlay) {
            return;
        }

        this.overlay = document.createElement('div');
        this.overlay.className = `${this.options.className}__overlay`;

        Object.assign(this.overlay.style, {
            position: 'fixed',
            top: '0',
            left: '0',
            width: '100vw',
            height: '100vh',
            backgroundColor: `rgba(0, 0, 0, ${this.options.overlayOpacity})`,
            zIndex: '999',
            display: 'none',
            opacity: '0',
            transition: `opacity ${this.options.animationDuration}ms ease`,
        });
    }

    /**
     * Append to container
     */
    appendToContainer() {
        // Append overlay first (lower z-index)
        if (this.overlay) {
            this.options.container.appendChild(this.overlay);
        }

        // Append panel
        this.options.container.appendChild(this.panel);
    }

    /**
     * Setup event listeners
     */
    setupEventListeners() {
        // Toggle button
        this.toggleButton.addEventListener('click', (e) => {
            e.stopPropagation();
            this.toggle();
        });

        // Close button
        this.closeButton.addEventListener('click', (e) => {
            e.stopPropagation();
            this.collapse();
        });

        // Overlay click
        if (this.overlay && this.options.closeOnOverlayClick) {
            this.overlay.addEventListener('click', () => {
                this.collapse();
            });
        }

        // Escape key
        if (this.options.closeOnEsc) {
            document.addEventListener('keydown', (e) => {
                if (e.key === 'Escape' && this.isExpanded) {
                    this.collapse();
                }
            });
        }

        // Drag and drop for header
        if (this.options.draggable) {
            this.setupDrag();
        }

        // Resize handle
        if (this.options.resizable && this.resizeHandle) {
            this.setupResize();
        }

        // Window resize
        window.addEventListener('resize', () => this.onWindowResize());
    }

    /**
     * Setup drag functionality
     */
    setupDrag() {
        let isDragging = false;
        let startX = 0;
        let startY = 0;
        let startLeft = 0;
        let startTop = 0;

        this.header.addEventListener('mousedown', (e) => {
            // Only start drag on the header background, not on buttons
            if (e.target === this.header || e.target === this.header.querySelector('h3')) {
                isDragging = true;
                startX = e.clientX;
                startY = e.clientY;

                // Get current position
                const rect = this.panel.getBoundingClientRect();
                startLeft = rect.left;
                startTop = rect.top;

                // Change cursor
                document.body.style.cursor = 'move';
                this.panel.style.cursor = 'move';

                e.preventDefault();
            }
        });

        document.addEventListener('mousemove', (e) => {
            if (!isDragging) return;

            const dx = e.clientX - startX;
            const dy = e.clientY - startY;

            // Calculate new position
            let newLeft = startLeft + dx;
            let newTop = startTop + dy;

            // Constrain to viewport
            const viewportWidth = window.innerWidth;
            const viewportHeight = window.innerHeight;
            const panelWidth = this.panel.offsetWidth;
            const panelHeight = this.panel.offsetHeight;

            newLeft = Math.max(0, Math.min(newLeft, viewportWidth - panelWidth));
            newTop = Math.max(0, Math.min(newTop, viewportHeight - panelHeight));

            // Apply new position
            this.panel.style.position = 'fixed';
            this.panel.style.left = `${newLeft}px`;
            this.panel.style.top = `${newTop}px`;
            this.panel.style.right = 'auto';
            this.panel.style.bottom = 'auto';

            this.emit('dragging', { x: newLeft, y: newTop });
        });

        document.addEventListener('mouseup', () => {
            if (isDragging) {
                isDragging = false;

                // Reset cursor
                document.body.style.cursor = '';
                this.panel.style.cursor = '';

                this.emit('dragged', {
                    x: parseInt(this.panel.style.left) || 0,
                    y: parseInt(this.panel.style.top) || 0,
                });
            }
        });
    }

    /**
     * Setup resize functionality
     */
    setupResize() {
        this.resizeHandle.addEventListener('mousedown', (e) => {
            this.isResizing = true;
            this.resizeStartX = e.clientX;
            this.resizeStartWidth = this.currentWidth;

            // Change cursor
            document.body.style.cursor = 'col-resize';
            this.panel.style.cursor = 'col-resize';

            // Add event listeners
            document.addEventListener('mousemove', this.handleResizeMouseMove);
            document.addEventListener('mouseup', this.handleResizeMouseUp);

            e.preventDefault();
        });
    }

    /**
     * Handle resize mouse move
     */
    handleResizeMouseMove = (e) => {
        if (!this.isResizing) return;

        const deltaX =
            this.options.position === 'left'
                ? e.clientX - this.resizeStartX
                : this.resizeStartX - e.clientX;

        let newWidth = this.resizeStartWidth + deltaX;

        // Constrain width
        newWidth = Math.max(this.options.minWidth, Math.min(newWidth, this.options.maxWidth));

        // Update width
        this.currentWidth = newWidth;
        this.panel.style.width = `${newWidth}px`;

        this.emit('resizing', { width: newWidth });
    };

    /**
     * Handle resize mouse up
     */
    handleResizeMouseUp = () => {
        if (!this.isResizing) return;

        this.isResizing = false;

        // Reset cursor
        document.body.style.cursor = '';
        this.panel.style.cursor = '';

        // Remove event listeners
        document.removeEventListener('mousemove', this.handleResizeMouseMove);
        document.removeEventListener('mouseup', this.handleResizeMouseUp);

        this.emit('resized', { width: this.currentWidth });
    };

    /**
     * Handle window resize
     */
    onWindowResize() {
        // Adjust panel width if it exceeds viewport
        const viewportWidth = window.innerWidth;
        const maxAllowedWidth = viewportWidth * 0.8; // Max 80% of viewport

        if (this.currentWidth > maxAllowedWidth) {
            this.currentWidth = Math.min(maxAllowedWidth, this.options.maxWidth);
            this.panel.style.width = `${this.currentWidth}px`;
        }

        // Update responsive states
        this.updateResponsiveState();
    }

    /**
     * Update responsive state
     */
    updateResponsiveState() {
        const isMobile = window.innerWidth <= 768;

        // Show/hide close button on mobile
        if (isMobile) {
            this.closeButton.style.display = 'flex';
        } else {
            this.closeButton.style.display = 'none';
        }

        // Adjust width for mobile
        if (isMobile && this.isExpanded) {
            this.panel.style.width = '100vw';
        } else if (!isMobile && this.isExpanded) {
            this.panel.style.width = `${this.currentWidth}px`;
        }
    }

    /**
     * Update panel state
     */
    updatePanelState() {
        if (this.isExpanded) {
            this.expand();
        } else {
            this.collapse();
        }

        this.updateResponsiveState();
    }

    /**
     * Expand the panel
     */
    expand() {
        if (this.isExpanded) return;

        this.isExpanded = true;
        this.currentWidth = this.options.width;

        // Update panel width
        this.panel.style.width = `${this.currentWidth}px`;

        // Update toggle button
        this.toggleButton.innerHTML = '‹';
        this.toggleButton.title = 'Collapse panel';
        this.toggleButton.setAttribute('aria-label', 'Collapse panel');

        // Show overlay
        if (this.overlay) {
            this.overlay.style.display = 'block';
            setTimeout(() => {
                this.overlay.style.opacity = '1';
            }, 10);
        }

        // Update content visibility
        this.updateContentVisibility(true);

        this.emit('expanded');
    }

    /**
     * Collapse the panel
     */
    collapse() {
        if (!this.isExpanded) return;

        this.isExpanded = false;
        this.currentWidth = this.options.collapsedWidth;

        // Update panel width
        this.panel.style.width = `${this.currentWidth}px`;

        // Update toggle button
        this.toggleButton.innerHTML = '›';
        this.toggleButton.title = 'Expand panel';
        this.toggleButton.setAttribute('aria-label', 'Expand panel');

        // Hide overlay
        if (this.overlay) {
            this.overlay.style.opacity = '0';
            setTimeout(() => {
                this.overlay.style.display = 'none';
            }, this.options.animationDuration);
        }

        // Update content visibility
        this.updateContentVisibility(false);

        this.emit('collapsed');
    }

    /**
     * Toggle panel state
     */
    toggle() {
        if (this.isExpanded) {
            this.collapse();
        } else {
            this.expand();
        }
    }

    /**
     * Update content visibility
     */
    updateContentVisibility(visible) {
        if (visible) {
            // Show header, content, footer
            this.header.style.display = 'flex';
            this.content.style.display = 'block';
            this.footer.style.display = 'block';
            // Hide icon container
            if (this.iconContainer) {
                this.iconContainer.style.display = 'none';
            }

            // Fade in
            setTimeout(() => {
                this.header.style.opacity = '1';
                this.content.style.opacity = '1';
                this.footer.style.opacity = '1';
            }, this.options.animationDuration / 2);
        } else {
            // Fade out header, content, footer
            this.header.style.opacity = '0';
            this.content.style.opacity = '0';
            this.footer.style.opacity = '0';

            // Hide after fade out
            setTimeout(() => {
                this.header.style.display = 'none';
                this.content.style.display = 'none';
                this.footer.style.display = 'none';
                // Show icon container
                if (this.iconContainer) {
                    this.iconContainer.style.display = 'flex';
                }
            }, this.options.animationDuration);
        }
    }

    /**
     * Set panel content
     */
    setContent(content) {
        // Clear existing content
        this.content.innerHTML = '';

        // Add new content
        if (typeof content === 'string') {
            this.content.innerHTML = content;
        } else if (content instanceof HTMLElement) {
            this.content.appendChild(content);
        } else if (Array.isArray(content)) {
            content.forEach((item) => {
                if (item instanceof HTMLElement) {
                    this.content.appendChild(item);
                }
            });
        }
    }

    /**
     * Add content to panel
     */
    addContent(content) {
        if (typeof content === 'string') {
            const div = document.createElement('div');
            div.innerHTML = content;
            this.content.appendChild(div);
        } else if (content instanceof HTMLElement) {
            this.content.appendChild(content);
        }
    }

    /**
     * Clear panel content
     */
    clearContent() {
        this.content.innerHTML = '';
    }

    /**
     * Set panel title
     */
    setTitle(title) {
        const titleElement = this.header.querySelector('h3');
        if (titleElement) {
            titleElement.textContent = title;
        }
    }

    /**
     * Set panel width
     */
    setWidth(width, animate = true) {
        const newWidth = Math.max(this.options.minWidth, Math.min(width, this.options.maxWidth));

        if (animate) {
            this.panel.style.transition = `width ${this.options.animationDuration}ms ease`;
        } else {
            this.panel.style.transition = 'none';
        }

        this.currentWidth = newWidth;
        this.panel.style.width = `${newWidth}px`;

        if (this.isExpanded) {
            this.options.width = newWidth;
        }

        // Restore transition
        if (!animate) {
            setTimeout(() => {
                this.panel.style.transition = `width ${this.options.animationDuration}ms ease`;
            }, 10);
        }

        this.emit('widthChanged', { width: newWidth });
    }

    /**
     * Set panel position
     */
    setPosition(position) {
        if (position !== 'left' && position !== 'right') {
            console.warn('Invalid position. Use "left" or "right".');
            return;
        }

        this.options.position = position;

        // Update panel class
        this.panel.className = `${this.options.className} ${this.options.className}--${position}`;

        // Update CSS position
        this.panel.style.left = position === 'left' ? '0' : 'auto';
        this.panel.style.right = position === 'right' ? '0' : 'auto';

        // Update resize handle position
        if (this.resizeHandle) {
            const handlePosition = position === 'left' ? 'right' : 'left';
            this.resizeHandle.style.left = handlePosition === 'left' ? '0' : 'auto';
            this.resizeHandle.style.right = handlePosition === 'right' ? '0' : 'auto';
        }

        this.emit('positionChanged', { position });
    }

    /**
     * Show the panel
     */
    show() {
        this.panel.style.display = 'flex';

        // If expanded, show overlay
        if (this.isExpanded && this.overlay) {
            this.overlay.style.display = 'block';
            setTimeout(() => {
                this.overlay.style.opacity = '1';
            }, 10);
        }

        this.emit('shown');
    }

    /**
     * Hide the panel
     */
    hide() {
        // Hide overlay first
        if (this.overlay) {
            this.overlay.style.opacity = '0';
            setTimeout(() => {
                this.overlay.style.display = 'none';
            }, this.options.animationDuration);
        }

        // Hide panel
        this.panel.style.display = 'none';

        this.emit('hidden');
    }

    /**
     * Get panel state
     */
    getState() {
        return {
            isExpanded: this.isExpanded,
            isInitialized: this.isInitialized,
            currentWidth: this.currentWidth,
            options: { ...this.options },
            position: this.getPosition(),
            visible: this.panel.style.display !== 'none',
        };
    }

    /**
     * Get panel position relative to viewport
     */
    getPosition() {
        const rect = this.panel.getBoundingClientRect();
        return {
            x: rect.left,
            y: rect.top,
            width: rect.width,
            height: rect.height,
        };
    }

    /**
     * Get panel content element
     */
    getContentElement() {
        return this.content;
    }

    /**
     * Get panel header element
     */
    getHeaderElement() {
        return this.header;
    }

    /**
     * Get panel footer element
     */
    getFooterElement() {
        return this.footer;
    }

    /**
     * Create icon container for collapsed state
     */
    createIconContainer() {
        this.iconContainer = document.createElement('div');
        this.iconContainer.className = `${this.options.className}__icon-container`;

        Object.assign(this.iconContainer.style, {
            display: 'flex',
            flexDirection: 'column',
            alignItems: 'center',
            justifyContent: 'center',
            gap: '16px',
            padding: '16px 0',
            flex: '1',
            overflow: 'hidden',
        });

        // Define icons with tooltips
        const icons = [
            { icon: '📚', tooltip: 'Layers', id: 'layers' },
            { icon: '⚙️', tooltip: 'Render Settings', id: 'settings' },
            { icon: '📊', tooltip: 'Performance', id: 'performance' },
        ];

        icons.forEach(({ icon, tooltip, id }) => {
            const iconButton = document.createElement('button');
            iconButton.className = `${this.options.className}__icon-button`;
            iconButton.innerHTML = icon;
            iconButton.title = tooltip;
            iconButton.setAttribute('aria-label', tooltip);
            iconButton.dataset.section = id;

            Object.assign(iconButton.style, {
                width: '32px',
                height: '32px',
                display: 'flex',
                alignItems: 'center',
                justifyContent: 'center',
                fontSize: '20px',
                background: 'none',
                border: 'none',
                borderRadius: '4px',
                cursor: 'pointer',
                transition: 'background-color 0.2s',
            });

            // Hover effect
            iconButton.addEventListener('mouseenter', () => {
                iconButton.style.backgroundColor = 'rgba(0, 0, 0, 0.05)';
            });
            iconButton.addEventListener('mouseleave', () => {
                iconButton.style.backgroundColor = 'transparent';
            });

            // Click handler to expand panel and focus section (optional)
            iconButton.addEventListener('click', () => {
                if (!this.isExpanded) {
                    this.expand();
                }
                // Emit event for section focus
                this.emit('iconClick', { section: id });
            });

            this.iconContainer.appendChild(iconButton);
        });

        // Initially hidden if expanded
        if (this.isExpanded) {
            this.iconContainer.style.display = 'none';
        }

        this.panel.appendChild(this.iconContainer);
    }

    /**
     * Clean up resources
     */
    destroy() {
        // Remove event listeners
        this.toggleButton.removeEventListener('click', () => {});
        this.closeButton.removeEventListener('click', () => {});

        if (this.overlay) {
            this.overlay.removeEventListener('click', () => {});
        }

        document.removeEventListener('keydown', () => {});
        document.removeEventListener('mousemove', this.handleResizeMouseMove);
        document.removeEventListener('mouseup', this.handleResizeMouseUp);
        window.removeEventListener('resize', () => this.onWindowResize());

        // Remove DOM elements
        if (this.panel && this.panel.parentNode) {
            this.panel.parentNode.removeChild(this.panel);
        }

        if (this.overlay && this.overlay.parentNode) {
            this.overlay.parentNode.removeChild(this.overlay);
        }

        this.clear();

        console.debug('SidePanel destroyed');
    }
}

// Default export
export default SidePanel;
