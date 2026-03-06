import './assets/css/main.css';
import './assets/css/map.css';
import './assets/css/ui.css';

import { MapApiClient } from './services/api/MapApiClient.js';
import { DynamicImageLayer } from './components/Map/DynamicImageLayer.js';
import { LayerSwitcher } from './components/MapControl/LayerSwitcher.js';
import { ZoomControl } from './components/MapControl/ZoomControl.js';
import { ScaleControl } from './components/MapControl/ScaleControl.js';

import { TileLayerManager } from './components/Map/TileLayerManager.js';
import { WorkerManager } from './workers/WorkerManager.js';
import { CacheManager } from './services/cache/CacheManager.js';
import { ToastNotification } from './components/UI/ToastNotification.js';
import { LoadingSpinner } from './components/UI/LoadingSpinner.js';
import { mapStore } from './stores/mapStore.js';
import { layerStore } from './stores/layerStore.js';
import { requestStore } from './stores/requestStore.js';
import { settings } from './config/settings.js';

class MapApplication {
    constructor() {
        this.map = null;
        this.dynamicImageLayer = null;
        this.layerSwitcher = null;
        this.apiClient = null;
        this.toast = null;
        this.spinner = null;
        this.zoomControl = null;
        this.scaleControl = null;
        this.tileLayerManager = null;
        this.workerManager = null;
        this.cacheManager = null;
        this.isInitialized = false;
    }

    async init() {
        try {
            console.log('Initializing Map Application...');

            // Initialize stores
            await this.initStores();

            // Initialize UI components
            this.initUIComponents();

            // Initialize API client
            this.apiClient = new MapApiClient(settings.API_BASE_URL);

            // Initialize worker manager for parallel processing
            this.workerManager = new WorkerManager({
                maxWorkers: 4,
                enableWorkerPool: true,
            });

            // Initialize cache manager
            this.cacheManager = new CacheManager({
                strategy: settings.CACHE.defaultStrategy,
                enableMemoryCache: true,
                enableLocalStorageCache: true,
                enableIndexedDBCache: false,
                enableServiceWorkerCache: false,
            });

            // Initialize map
            await this.initMap();

            // Initialize dynamic image layer
            await this.initDynamicImageLayer();

            // Initialize tile layer manager
            this.tileLayerManager = new TileLayerManager({ map: this.map });

            // Initialize layer switcher
            this.initLayerSwitcher();

            // Set up event listeners
            this.setupEventListeners();

            // Check server health
            await this.checkServerHealth();

            // Update UI state
            this.updateUIState();

            this.isInitialized = true;
            console.log('Map Application initialized successfully');

            this.showToast('Application ready', 'success');
        } catch (error) {
            console.error('Failed to initialize application:', error);
            this.showToast(`Initialization failed: ${error.message}`, 'error');
            this.handleFatalError(error);
        }
    }

    async initStores() {
        await Promise.all([mapStore.init(), layerStore.init(), requestStore.init()]);
    }

    initUIComponents() {
        this.toast = new ToastNotification(document.getElementById('toast-container'));
        this.spinner = new LoadingSpinner(document.getElementById('loading-spinner'));



        // Initialize tile layer manager (requires map instance, will be initialized later)
        // this.tileLayerManager = new TileLayerManager({ map: this.map });
    }

    async initMap() {
        const mapContainer = document.getElementById('map');
        if (!mapContainer) {
            throw new Error('Map container not found');
        }

        // Create map instance
        this.map = L.map('map', {
            center: settings.MAP.defaultCenter,
            zoom: settings.MAP.defaultZoom,
            minZoom: settings.MAP.minZoom,
            maxZoom: settings.MAP.maxZoom,
            maxBounds: settings.MAP.maxBounds,
            renderer: L.canvas(),
            attributionControl: false,
            zoomControl: false,
        });

        // Add custom zoom control
        this.zoomControl = new ZoomControl({
            map: this.map,
            position: 'topleft',
        });

        // Add custom scale control
        this.scaleControl = new ScaleControl({
            map: this.map,
            position: 'bottomleft',
            metric: true,
            imperial: false,
            updateWhenIdle: false,
        });

        // Hide the scale control on map (display in footer instead)
        this.scaleControl.hide();
        
        // Listen for scale updates to display in footer and map control
        this.scaleControl.on('scaleUpdated', (scale) => {
            const scaleRatio = this.scaleControl.calculateScaleRatio(scale.meters);
            
            // Update footer scale info
            const scaleElement = document.getElementById('scale-info');
            if (scaleElement) {
                scaleElement.textContent = `1:${scaleRatio.toLocaleString()}`;
            }
            
            // Update map control scale display
            const mapScaleElement = document.getElementById('map-scale');
            if (mapScaleElement) {
                mapScaleElement.textContent = `Scale: 1:${scaleRatio.toLocaleString()}`;
            }
        });

        // Update store with map state
        mapStore.setMapInstance(this.map);

        // Set up map event listeners
        this.map.on('moveend', () => this.onMapMove());
        this.map.on('zoomend', () => this.onMapZoom());
        this.map.on('resize', () => this.onMapResize());

        console.log('Map initialized');


    }

    async initDynamicImageLayer() {
        if (!this.map) {
            throw new Error('Map not initialized');
        }

        this.dynamicImageLayer = new DynamicImageLayer({
            map: this.map,
            apiClient: this.apiClient,
            store: mapStore,
            cacheManager: this.cacheManager,
            workerManager: this.workerManager,
            onLoadingStart: () => this.spinner.show(),
            onLoadingEnd: () => this.spinner.hide(),
            onError: (error) => this.handleMapError(error),
        });

        await this.dynamicImageLayer.init();
        console.log('DynamicImageLayer initialized');
    }

    initLayerSwitcher() {
        this.layerSwitcher = new LayerSwitcher({
            container: document.getElementById('layer-list'),
            store: layerStore,
            onLayerChange: (layers) => this.onLayersChanged(layers),
        });

        this.layerSwitcher.init();
        console.log('LayerSwitcher initialized');
    }

    setupEventListeners() {
        // UI control events

        const btnZoomIn = document.getElementById('btn-zoom-in');
        if (btnZoomIn) {
            btnZoomIn.addEventListener('click', () => this.map.zoomIn());
        }
        const btnZoomOut = document.getElementById('btn-zoom-out');
        if (btnZoomOut) {
            btnZoomOut.addEventListener('click', () => this.map.zoomOut());
        }
        const btnFullscreen = document.getElementById('btn-fullscreen');
        if (btnFullscreen) {
            btnFullscreen.addEventListener('click', () => this.toggleFullscreen());
        }
        const btnAddLayer = document.getElementById('btn-add-layer');
        if (btnAddLayer) {
            btnAddLayer.addEventListener('click', () => this.addLayer());
        }

        const btnLayers = document.getElementById('btn-layers');
        if (btnLayers) {
            btnLayers.addEventListener('click', () => this.toggleRightSidePanel());
        }

        const btnClosePanel = document.getElementById('btn-close-panel');
        if (btnClosePanel) {
            btnClosePanel.addEventListener('click', () => this.toggleRightSidePanel(false));
        }

        // Header buttons
        const btnBackToDashboard = document.getElementById('btn-back-to-dashboard');
        if (btnBackToDashboard) {
            btnBackToDashboard.addEventListener('click', () => this.onBackToDashboard());
        }

        const btnNotifications = document.getElementById('btn-notifications');
        if (btnNotifications) {
            btnNotifications.addEventListener('click', () => this.onNotificationsClick());
        }

        // User menu and dropdown
        const btnUserMenu = document.getElementById('btn-user-menu');
        if (btnUserMenu) {
            btnUserMenu.addEventListener('click', (e) => this.onUserMenuClick(e));
        }

        // User dropdown items
        const userDropdown = document.getElementById('user-dropdown');
        if (userDropdown) {
            const dropdownItems = userDropdown.querySelectorAll('.dropdown-item');
            dropdownItems.forEach(item => {
                item.addEventListener('click', (e) => this.onUserDropdownItemClick(e));
            });
        }

        // Content action bar buttons
        const btnFilter = document.getElementById('btn-filter');
        if (btnFilter) {
            btnFilter.addEventListener('click', () => this.onFilterClick());
        }

        const btnSort = document.getElementById('btn-sort');
        if (btnSort) {
            btnSort.addEventListener('click', () => this.onSortClick());
        }

        const btnExport = document.getElementById('btn-export');
        if (btnExport) {
            btnExport.addEventListener('click', () => this.onExportClick());
        }

        const btnRefresh = document.getElementById('btn-refresh');
        if (btnRefresh) {
            btnRefresh.addEventListener('click', () => this.onRefreshClick());
        }

        const btnSettings = document.getElementById('btn-settings');
        if (btnSettings) {
            btnSettings.addEventListener('click', () => this.onSettingsClick());
        }

        // Global search
        const globalSearchInput = document.getElementById('global-search-input');
        const globalSearchBtn = document.getElementById('global-search-btn');
        if (globalSearchInput) {
            globalSearchInput.addEventListener('keypress', (e) => {
                if (e.key === 'Enter') {
                    this.onGlobalSearch(globalSearchInput.value);
                }
            });
        }
        if (globalSearchBtn) {
            globalSearchBtn.addEventListener('click', () => {
                const searchValue = globalSearchInput ? globalSearchInput.value : '';
                this.onGlobalSearch(searchValue);
            });
        }

        // Settings change events
        const selectQuality = document.getElementById('select-quality');
        if (selectQuality) {
            selectQuality.addEventListener('change', (e) => this.onQualityChange(e));
        }
        const selectCache = document.getElementById('select-cache');
        if (selectCache) {
            selectCache.addEventListener('change', (e) => this.onCacheStrategyChange(e));
        }

        // Keyboard shortcuts
        document.addEventListener('keydown', (e) => this.handleKeyboardShortcuts(e));

        // Window events
        window.addEventListener('resize', () => this.onWindowResize());
        window.addEventListener('online', () => this.onNetworkStatusChange(true));
        window.addEventListener('offline', () => this.onNetworkStatusChange(false));

        // Store subscriptions
        mapStore.subscribe(() => this.updateMapInfo());
        requestStore.subscribe(() => this.updateRequestInfo());

        // Left sidebar events
        const toggleLeftSidebarBtn = document.getElementById('toggle-left-sidebar');
        if (toggleLeftSidebarBtn) {
            toggleLeftSidebarBtn.addEventListener('click', () => this.toggleLeftSidebar());
        }

        // Setup left sidebar resize and menu
        this.setupLeftSidebarResize();
        this.setupLeftSidebarMenu();

        // Close user dropdown when clicking outside
        document.addEventListener('click', (e) => this.closeUserDropdownIfOutside(e));
    }

    async checkServerHealth() {
        try {
            const health = await this.apiClient.getHealth();
            this.updateServerStatus(health);
            this.showToast('Server connected', 'success');
        } catch (error) {
            console.warn('Server health check failed:', error);
            this.updateServerStatus({ status: 'unreachable' });
            this.showToast('Server connection failed', 'warning');
        }
    }

    onMapMove() {
        if (!this.isInitialized) return;

        const bounds = this.map.getBounds();
        const center = this.map.getCenter();
        const zoom = this.map.getZoom();

        mapStore.setViewport(bounds, center, zoom);

        // Update dynamic image layer
        if (this.dynamicImageLayer) {
            this.dynamicImageLayer.updateViewport();
        }
    }

    onMapZoom() {
        this.onMapMove();
    }

    onMapResize() {
        if (this.dynamicImageLayer) {
            this.dynamicImageLayer.onResize();
        }
    }

    onLayersChanged(layers) {
        console.log('Layers changed:', layers);
        if (this.dynamicImageLayer) {
            this.dynamicImageLayer.updateLayers(layers);
        }
    }

    onQualityChange(event) {
        const quality = event.target.value;
        mapStore.setQuality(quality);
        if (this.dynamicImageLayer) {
            this.dynamicImageLayer.updateQuality(quality);
        }
    }

    onCacheStrategyChange(event) {
        const strategy = event.target.value;
        mapStore.setCacheStrategy(strategy);
        if (this.dynamicImageLayer) {
            this.dynamicImageLayer.updateCacheStrategy(strategy);
        }
    }

    onWindowResize() {
        if (this.map) {
            this.map.invalidateSize();
        }
    }

    onNetworkStatusChange(isOnline) {
        const statusElement = document.getElementById('connection-status');
        if (isOnline) {
            statusElement.textContent = '● Online';
            statusElement.className = 'status-online';
            this.showToast('Back online', 'success');
            // Retry any failed requests
            if (this.dynamicImageLayer) {
                this.dynamicImageLayer.retryFailedRequests();
            }
        } else {
            statusElement.textContent = '● Offline';
            statusElement.className = 'status-offline';
            this.showToast('You are offline', 'warning');
        }
    }

    handleKeyboardShortcuts(event) {
        // Prevent default behavior for our shortcuts
        switch (event.key) {
            case '+':
            case '=':
                event.preventDefault();
                this.map.zoomIn();
                break;
            case '-':
                event.preventDefault();
                this.map.zoomOut();
                break;
            case 'Escape':
                this.toggleRightSidePanel(false);
                break;
            case 'l':
            case 'L':
                if (event.ctrlKey) {
                    event.preventDefault();
                    this.toggleRightSidePanel();
                }
                break;

        }
    }





    toggleFullscreen() {
        const elem = document.documentElement;
        if (!document.fullscreenElement) {
            elem.requestFullscreen().catch((err) => {
                this.showToast(`Fullscreen failed: ${err.message}`, 'error');
            });
        } else {
            document.exitFullscreen();
        }
    }

    /**
     * Handle back to dashboard button click
     */
    onBackToDashboard() {
        this.showToast('返回仪表盘功能即将上线', 'info');
        // Add actual navigation logic here
    }

    /**
     * Handle notifications button click
     */
    onNotificationsClick() {
        this.showToast('通知功能即将上线', 'info');
        // Add notifications panel logic here
    }

    /**
     * Handle user menu button click
     * @param {Event} e - Click event
     */
    onUserMenuClick(e) {
        e.stopPropagation();
        const dropdown = document.getElementById('user-dropdown');
        if (dropdown) {
            dropdown.classList.toggle('show');
        }
    }

    /**
     * Handle user dropdown item clicks
     * @param {Event} e - Click event
     */
    onUserDropdownItemClick(e) {
        e.preventDefault();
        const item = e.target.closest('.dropdown-item');
        if (!item) return;
        
        const text = item.textContent.trim();
        switch (text) {
            case '个人设置':
                this.showToast('打开个人设置', 'info');
                // Add personal settings logic here
                break;
            case '系统管理':
                this.showToast('打开系统管理', 'info');
                // Add system administration logic here
                break;
            case '退出登录':
                this.showToast('退出登录功能即将上线', 'info');
                // Add logout logic here
                break;
            default:
                this.showToast(`未知操作: ${text}`, 'warning');
        }
        
        // Close dropdown after selection
        const dropdown = document.getElementById('user-dropdown');
        if (dropdown) {
            dropdown.classList.remove('show');
        }
    }

    /**
     * Handle filter button click
     */
    onFilterClick() {
        this.showToast('筛选功能即将上线', 'info');
        // Add filter panel logic here
    }

    /**
     * Handle sort button click
     */
    onSortClick() {
        this.showToast('排序功能即将上线', 'info');
        // Add sort panel logic here
    }

    /**
     * Handle export button click
     */
    onExportClick() {
        this.showToast('导出功能即将上线', 'info');
        // Add export dialog logic here
    }

    /**
     * Handle refresh button click
     */
    onRefreshClick() {
        this.showToast('刷新地图数据', 'info');
        if (this.map) {
            this.map.invalidateSize();
        }
        if (this.dynamicImageLayer) {
            this.dynamicImageLayer.refresh();
        }
    }

    /**
     * Handle global search
     * @param {string} query - Search query
     */
    onGlobalSearch(query) {
        if (!query || query.trim() === '') {
            this.showToast('请输入搜索内容', 'warning');
            return;
        }
        
        this.showToast(`搜索: ${query}`, 'info');
        // Add actual search logic here
        // Example: filter map features, search for projects, etc.
    }

    /**
     * Handle settings button click
     */
    onSettingsClick() {
        this.showToast('打开设置', 'info');
        // Add settings panel logic here
        // Also ensure right side panel is visible for settings
        if (!this.isRightSidePanelVisible()) {
            this.toggleRightSidePanel(true);
        }
    }

    /**
     * Close user dropdown when clicking outside
     * @param {Event} e - Click event
     */
    closeUserDropdownIfOutside(e) {
        const userMenu = document.querySelector('.user-menu');
        const dropdown = document.getElementById('user-dropdown');
        
        if (!userMenu || !dropdown) return;
        
        const isClickInsideMenu = userMenu.contains(e.target);
        const isClickInsideDropdown = dropdown.contains(e.target);
        
        if (!isClickInsideMenu && !isClickInsideDropdown) {
            dropdown.classList.remove('show');
        }
    }

    /**
     * Toggle right side panel visibility
     * @param {boolean} forceState - true to show, false to hide, undefined to toggle
     */
    toggleRightSidePanel(forceState) {
        const sidePanel = document.getElementById('side-panel');
        if (!sidePanel) return;

        if (forceState !== undefined) {
            if (forceState) {
                sidePanel.classList.remove('hidden');
            } else {
                sidePanel.classList.add('hidden');
            }
        } else {
            sidePanel.classList.toggle('hidden');
        }

        // Adjust map container after panel visibility change
        this.adjustMapContainerForRightPanel();
    }

    /**
     * Check if right side panel is visible
     * @returns {boolean}
     */
    isRightSidePanelVisible() {
        const sidePanel = document.getElementById('side-panel');
        return sidePanel && !sidePanel.classList.contains('hidden');
    }

    /**
     * Adjust map container margin based on right panel visibility
     */
    adjustMapContainerForRightPanel() {
        if (!this.map) return;
        // Invalidate map size to ensure proper rendering
        setTimeout(() => this.map.invalidateSize(), 50);
    }






    addLayer() {
        // Implementation for adding new layer
        this.showToast('Add layer feature coming soon', 'info');
    }

    updateMapInfo() {
        const state = mapStore.getState();

        // Update viewport info
        const viewportInfo = document.getElementById('viewport-info');
        if (viewportInfo && state.bounds && state.bounds.getSouth && typeof state.bounds.getSouth === 'function') {
            const { bounds } = state;
            try {
                viewportInfo.textContent =
                    `${bounds.getSouth().toFixed(2)}, ${bounds.getWest().toFixed(2)} to ` +
                    `${bounds.getNorth().toFixed(2)}, ${bounds.getEast().toFixed(2)}`;
            } catch (error) {
                console.warn('Failed to format bounds:', error);
                viewportInfo.textContent = 'Bounds unavailable';
            }
        } else if (viewportInfo) {
            viewportInfo.textContent = 'Bounds unavailable';
        }

        // Update zoom info
        const zoomInfo = document.getElementById('zoom-info');
        if (zoomInfo) {
            zoomInfo.textContent = state.zoom || 0;
        }

        // Update image size info
        const imageSizeInfo = document.getElementById('image-size-info');
        if (imageSizeInfo && state.imageSize && state.imageSize.width && state.imageSize.height) {
            imageSizeInfo.textContent = `${state.imageSize.width}×${state.imageSize.height}`;
        } else if (imageSizeInfo) {
            imageSizeInfo.textContent = '0×0';
        }

        // Update scale
        const scaleInfo = document.getElementById('map-scale');
        if (scaleInfo && state.scale) {
            scaleInfo.textContent = `Scale: 1:${state.scale.toLocaleString()}`;
        }
    }

    updateRequestInfo() {
        const state = requestStore.getState();

        // Update active requests
        const activeRequests = document.getElementById('stat-active-requests');
        if (activeRequests) {
            activeRequests.textContent = state.activeRequests || 0;
        }

        // Update cache hit rate
        const cacheHit = document.getElementById('stat-cache-hit');
        if (cacheHit) {
            const hitRate = state.cacheHitRate || 0;
            cacheHit.textContent = `${Math.round(hitRate * 100)}%`;
        }

        // Update render time
        const renderTime = document.getElementById('stat-render-time');
        if (renderTime) {
            renderTime.textContent = `${state.lastRenderTime || 0}ms`;
        }
    }

    updateServerStatus(health) {
        const serverStatus = document.getElementById('server-status');
        if (serverStatus) {
            serverStatus.textContent = `Server: ${health.status || 'unknown'}`;
            serverStatus.className = `status-${health.status || 'unknown'}`;
        }
    }

    updateUIState() {
        // Update various UI elements based on current state
        this.updateMapInfo();
        this.updateRequestInfo();
    }

    /**
     * Toggle left sidebar collapsed state
     */
    toggleLeftSidebar() {
        const leftSidebar = document.getElementById('left-sidebar');
        if (leftSidebar) {
            const isCollapsing = !leftSidebar.classList.contains('collapsed');
            leftSidebar.classList.toggle('collapsed');
            const toggleIcon = leftSidebar.querySelector('.toggle-icon');
            if (toggleIcon) {
                toggleIcon.textContent = leftSidebar.classList.contains('collapsed') ? '▶' : '◀';
            }
            
            // Handle width preservation during collapse/expand
            if (isCollapsing) {
                // When collapsing, save current width to data attribute and remove inline style
                leftSidebar.dataset.width = leftSidebar.style.width || '';
                leftSidebar.style.width = '';
            } else {
                // When expanding, restore width from data attribute if exists
                if (leftSidebar.dataset.width) {
                    leftSidebar.style.width = leftSidebar.dataset.width;
                }
            }
            
            // Adjust map container
            if (this.map) {
                setTimeout(() => this.map.invalidateSize(), 100);
            }
            this.showToast(leftSidebar.classList.contains('collapsed') ? '左侧边栏已折叠' : '左侧边栏已展开', 'info');
        }
    }

    /**
     * Setup left sidebar resize functionality
     */
    setupLeftSidebarResize() {
        const leftSidebar = document.getElementById('left-sidebar');
        const resizeHandle = document.getElementById('resize-left-sidebar');
        if (!leftSidebar || !resizeHandle) return;

        let isResizing = false;
        let startX, startWidth;

        const startResize = (e) => {
            isResizing = true;
            startX = e.clientX || e.touches?.[0].clientX;
            startWidth = leftSidebar.offsetWidth;
            resizeHandle.classList.add('resizing');
            document.body.classList.add('resizing');
            e.preventDefault();
        };

        const doResize = (e) => {
            if (!isResizing) return;
            const currentX = e.clientX || e.touches?.[0].clientX;
            const deltaX = currentX - startX;
            const newWidth = Math.max(200, Math.min(400, startWidth + deltaX));
            leftSidebar.style.width = `${newWidth}px`;
            // Update saved width in data attribute
            leftSidebar.dataset.width = `${newWidth}px`;
            // Adjust map container
            if (this.map) {
                setTimeout(() => this.map.invalidateSize(), 50);
            }
        };

        const stopResize = () => {
            if (!isResizing) return;
            isResizing = false;
            resizeHandle.classList.remove('resizing');
            document.body.classList.remove('resizing');
        };

        // Mouse events
        resizeHandle.addEventListener('mousedown', startResize);
        document.addEventListener('mousemove', doResize);
        document.addEventListener('mouseup', stopResize);

        // Touch events for mobile
        resizeHandle.addEventListener('touchstart', startResize);
        document.addEventListener('touchmove', doResize);
        document.addEventListener('touchend', stopResize);

        // Prevent text selection during resize
        document.addEventListener('selectstart', (e) => {
            if (isResizing) e.preventDefault();
        });
    }

    /**
     * Handle left sidebar menu clicks
     */
    setupLeftSidebarMenu() {
        const navItems = document.querySelectorAll('.left-sidebar .nav-item');
        navItems.forEach((item) => {
            const link = item.querySelector('.nav-link');
            if (link) {
                link.addEventListener('click', (e) => {
                    e.preventDefault();
                    // Remove active class from all items
                    navItems.forEach((i) => i.classList.remove('active'));
                    // Add active class to clicked item
                    item.classList.add('active');
                    
                    // Handle different menu items
                    const text = item.querySelector('.nav-text')?.textContent;
                    if (text === '地图视图') {
                        this.showToast('切换到地图视图', 'info');
                        // Add any map view specific logic here
                        // If right side panel is hidden, show it
                        if (!this.isRightSidePanelVisible()) {
                            this.toggleRightSidePanel(true);
                        }
                        // Ensure map is the primary view
                        this.switchToMapView();
                    } else if (text === '数据分析') {
                        this.showToast('切换到数据分析视图', 'info');
                        // Add data analysis logic here
                        this.switchToDataAnalysisView();
                    } else if (text === '系统设置') {
                        this.showToast('打开系统设置', 'info');
                        // Add settings logic here
                        this.switchToSystemSettingsView();
                    }
                });
            }
        });
    }

    /**
     * Switch to map view
     */
    switchToMapView() {
        // Hide any analysis or settings panels
        const analysisPanel = document.getElementById('analysis-panel');
        const settingsPanel = document.getElementById('settings-panel');
        
        if (analysisPanel) analysisPanel.style.display = 'none';
        if (settingsPanel) settingsPanel.style.display = 'none';
        
        // Ensure map is visible and focused
        if (this.map) {
            this.map.invalidateSize();
        }
    }

    /**
     * Switch to data analysis view
     */
    switchToDataAnalysisView() {
        // Create or show analysis panel
        let analysisPanel = document.getElementById('analysis-panel');
        if (!analysisPanel) {
            analysisPanel = document.createElement('div');
            analysisPanel.id = 'analysis-panel';
            analysisPanel.className = 'analysis-panel';
            analysisPanel.innerHTML = `
                <div class="panel-header">
                    <h3>数据分析</h3>
                    <button class="btn-close" id="btn-close-analysis">×</button>
                </div>
                <div class="panel-content">
                    <p>数据分析功能即将上线。这里将显示统计图表、数据分析工具等。</p>
                    <div class="placeholder-content">
                        <div class="placeholder-chart"></div>
                        <div class="placeholder-chart"></div>
                    </div>
                </div>
            `;
            document.querySelector('.app-main').appendChild(analysisPanel);
            
            // Add close button event
            const closeBtn = document.getElementById('btn-close-analysis');
            if (closeBtn) {
                closeBtn.addEventListener('click', () => {
                    analysisPanel.style.display = 'none';
                });
            }
        }
        
        analysisPanel.style.display = 'block';
        
        // Hide settings panel if visible
        const settingsPanel = document.getElementById('settings-panel');
        if (settingsPanel) settingsPanel.style.display = 'none';
    }

    /**
     * Switch to system settings view
     */
    switchToSystemSettingsView() {
        // Create or show settings panel
        let settingsPanel = document.getElementById('settings-panel');
        if (!settingsPanel) {
            settingsPanel = document.createElement('div');
            settingsPanel.id = 'settings-panel';
            settingsPanel.className = 'settings-panel';
            settingsPanel.innerHTML = `
                <div class="panel-header">
                    <h3>系统设置</h3>
                    <button class="btn-close" id="btn-close-settings">×</button>
                </div>
                <div class="panel-content">
                    <p>系统设置功能即将上线。这里将显示应用程序配置选项。</p>
                    <div class="form-group">
                        <label>主题模式</label>
                        <select id="select-theme" class="form-control">
                            <option value="light">浅色模式</option>
                            <option value="dark">深色模式</option>
                            <option value="auto">自动</option>
                        </select>
                    </div>
                    <div class="form-group">
                        <label>语言</label>
                        <select id="select-language" class="form-control">
                            <option value="zh">中文</option>
                            <option value="en">English</option>
                        </select>
                    </div>
                </div>
            `;
            document.querySelector('.app-main').appendChild(settingsPanel);
            
            // Add close button event
            const closeBtn = document.getElementById('btn-close-settings');
            if (closeBtn) {
                closeBtn.addEventListener('click', () => {
                    settingsPanel.style.display = 'none';
                });
            }
        }
        
        settingsPanel.style.display = 'block';
        
        // Hide analysis panel if visible
        const analysisPanel = document.getElementById('analysis-panel');
        if (analysisPanel) analysisPanel.style.display = 'none';
    }

    showToast(message, type = 'info') {
        if (this.toast) {
            this.toast.show(message, type);
        }
    }

    handleMapError(error) {
        console.error('Map error:', error);
        this.showToast(`Map error: ${error.message}`, 'error');
    }

    handleFatalError(error) {
        // Display error to user
        const errorHtml = `
            <div class="error-container">
                <h2>Application Error</h2>
                <p>${error.message}</p>
                <p>Please check the console for details.</p>
                <button onclick="location.reload()">Reload Application</button>
            </div>
        `;

        document.body.innerHTML = errorHtml;
    }

    /**
     * Clean up resources
     */
    destroy() {
        if (this.cacheManager) {
            this.cacheManager.destroy();
        }
        if (this.workerManager) {
            this.workerManager.destroy();
        }
        if (this.tileLayerManager) {
            this.tileLayerManager.destroy();
        }
        if (this.dynamicImageLayer) {
            this.dynamicImageLayer.destroy();
        }
        if (this.layerSwitcher) {
            this.layerSwitcher.destroy();
        }
        if (this.zoomControl) {
            this.zoomControl.destroy();
        }
        if (this.scaleControl) {
            this.scaleControl.destroy();
        }

        if (this.map) {
            this.map.remove();
        }
        console.debug('MapApplication destroyed');
    }
}

// Initialize application when DOM is ready
document.addEventListener('DOMContentLoaded', () => {
    const app = new MapApplication();
    app.init().catch((error) => {
        console.error('Application initialization failed:', error);
    });
});

// Export for testing
if (typeof module !== 'undefined' && module.exports) {
    module.exports = { MapApplication };
}
