package cn.cycle.echart.render;

import cn.cycle.chart.api.adapter.ImageDevice;
import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.api.core.Viewport;
import cn.cycle.chart.api.geometry.Envelope;

import java.io.File;
import java.util.Objects;

/**
 * 海图渲染服务。
 * 
 * <p>封装海图加载、渲染和图像生成的核心逻辑。</p>
 * <p>支持视口变化时的动态渲染，根据显示范围重新计算逻辑范围并选择性绘制。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class ChartRenderService {

    private ChartViewer chartViewer;
    private ImageDevice imageDevice;
    private String currentChartPath;
    private int lastRenderWidth;
    private int lastRenderHeight;
    
    private double currentCenterX;
    private double currentCenterY;
    private double currentScale;
    private double currentRotation;
    
    private Envelope chartExtent;

    public ChartRenderService() {
        this.lastRenderWidth = 0;
        this.lastRenderHeight = 0;
        this.currentCenterX = 0;
        this.currentCenterY = 0;
        this.currentScale = 1.0;
        this.currentRotation = 0;
    }

    /**
     * 加载海图文件。
     * 
     * @param filePath 海图文件路径
     * @return 加载结果，0表示成功
     */
    public int loadChart(String filePath) {
        Objects.requireNonNull(filePath, "filePath cannot be null");
        
        File file = new File(filePath);
        if (!file.exists()) {
            return -1;
        }
        
        try {
            if (chartViewer == null) {
                chartViewer = new ChartViewer();
            }
            
            int result = chartViewer.loadChart(filePath);
            if (result == 0) {
                currentChartPath = filePath;
                chartExtent = chartViewer.getFullExtent();
                
                if (chartExtent != null) {
                    currentCenterX = (chartExtent.getMinX() + chartExtent.getMaxX()) / 2.0;
                    currentCenterY = (chartExtent.getMinY() + chartExtent.getMaxY()) / 2.0;
                }
                
                currentScale = 1.0;
            }
            return result;
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Failed to load native library: " + e.getMessage());
            return -2;
        } catch (Exception e) {
            System.err.println("Failed to load chart: " + e.getMessage());
            return -3;
        }
    }

    /**
     * 计算初始缩放比例，使海图全幅显示。
     * 
     * @param viewWidth 视口宽度
     * @param viewHeight 视口高度
     * @return 初始缩放比例
     */
    public double calculateInitialScale(int viewWidth, int viewHeight) {
        if (chartExtent == null || viewWidth <= 0 || viewHeight <= 0) {
            return 1.0;
        }
        
        double extentWidth = chartExtent.getMaxX() - chartExtent.getMinX();
        double extentHeight = chartExtent.getMaxY() - chartExtent.getMinY();
        
        if (extentWidth <= 0 || extentHeight <= 0) {
            return 1.0;
        }
        
        double scaleX = viewWidth / extentWidth;
        double scaleY = viewHeight / extentHeight;
        
        return Math.min(scaleX, scaleY) * 0.9;
    }

    /**
     * 初始化视口以显示全幅海图。
     * 
     * @param viewWidth 视口宽度
     * @param viewHeight 视口高度
     */
    public void initializeViewportForFullExtent(int viewWidth, int viewHeight) {
        if (chartExtent == null) {
            return;
        }
        
        currentCenterX = (chartExtent.getMinX() + chartExtent.getMaxX()) / 2.0;
        currentCenterY = (chartExtent.getMinY() + chartExtent.getMaxY()) / 2.0;
        currentScale = calculateInitialScale(viewWidth, viewHeight);
    }

    /**
     * 渲染海图到指定尺寸（全幅显示）。
     * 
     * @param width 渲染宽度
     * @param height 渲染高度
     * @return 渲染结果，null表示失败
     */
    public RenderResult render(int width, int height) {
        if (chartViewer == null || currentChartPath == null) {
            return null;
        }
        
        if (width <= 0 || height <= 0) {
            return null;
        }
        
        try {
            if (chartExtent == null) {
                chartExtent = chartViewer.getFullExtent();
            }
            if (chartExtent == null) {
                return null;
            }
            
            initializeViewportForFullExtent(width, height);
            
            Envelope viewExtent = calculateViewExtent(width, height, currentCenterX, currentCenterY, currentScale);
            
            ensureImageDevice(width, height);
            
            Viewport viewport = chartViewer.getViewportObject();
            if (viewport != null) {
                viewport.setSize(width, height);
                viewport.setExtent(viewExtent);
            }
            
            imageDevice.clear();
            
            int renderResult = chartViewer.render(imageDevice.getNativePtr(), width, height);
            if (renderResult != 0) {
                return new RenderResult(renderResult, null, width, height);
            }
            
            byte[] pixels = imageDevice.getPixels();
            return new RenderResult(0, pixels, width, height);
            
        } catch (Exception e) {
            System.err.println("Failed to render chart: " + e.getMessage());
            return null;
        }
    }

    /**
     * 根据视口参数渲染海图。
     * 
     * @param width 渲染宽度
     * @param height 渲染高度
     * @param centerX 视口中心X（世界坐标）
     * @param centerY 视口中心Y（世界坐标）
     * @param scale 缩放比例
     * @return 渲染结果
     */
    public RenderResult renderWithViewport(int width, int height, double centerX, double centerY, double scale) {
        if (chartViewer == null || currentChartPath == null) {
            return null;
        }
        
        if (width <= 0 || height <= 0) {
            return null;
        }
        
        try {
            this.currentCenterX = centerX;
            this.currentCenterY = centerY;
            this.currentScale = scale;
            
            Envelope viewExtent = calculateViewExtent(width, height, centerX, centerY, scale);
            
            if (!isExtentValid(viewExtent)) {
                return new RenderResult(-4, null, width, height);
            }
            
            ensureImageDevice(width, height);
            
            Viewport viewport = chartViewer.getViewportObject();
            if (viewport != null) {
                viewport.setSize(width, height);
                viewport.setExtent(viewExtent);
            }
            
            imageDevice.clear();
            
            int renderResult = chartViewer.render(imageDevice.getNativePtr(), width, height);
            if (renderResult != 0) {
                return new RenderResult(renderResult, null, width, height);
            }
            
            byte[] pixels = imageDevice.getPixels();
            return new RenderResult(0, pixels, width, height);
            
        } catch (Exception e) {
            System.err.println("Failed to render chart with viewport: " + e.getMessage());
            return null;
        }
    }

    /**
     * 根据视口范围渲染海图。
     * 
     * @param width 渲染宽度
     * @param height 渲染高度
     * @param extent 视口范围
     * @return 渲染结果
     */
    public RenderResult renderWithExtent(int width, int height, Envelope extent) {
        if (chartViewer == null || currentChartPath == null) {
            return null;
        }
        
        if (width <= 0 || height <= 0 || extent == null) {
            return null;
        }
        
        try {
            this.currentCenterX = (extent.getMinX() + extent.getMaxX()) / 2.0;
            this.currentCenterY = (extent.getMinY() + extent.getMaxY()) / 2.0;
            
            double extentWidth = extent.getMaxX() - extent.getMinX();
            this.currentScale = width / extentWidth;
            
            ensureImageDevice(width, height);
            
            Viewport viewport = chartViewer.getViewportObject();
            if (viewport != null) {
                viewport.setSize(width, height);
                viewport.setExtent(extent);
            }
            
            imageDevice.clear();
            
            int renderResult = chartViewer.render(imageDevice.getNativePtr(), width, height);
            if (renderResult != 0) {
                return new RenderResult(renderResult, null, width, height);
            }
            
            byte[] pixels = imageDevice.getPixels();
            return new RenderResult(0, pixels, width, height);
            
        } catch (Exception e) {
            System.err.println("Failed to render chart with extent: " + e.getMessage());
            return null;
        }
    }

    /**
     * 平移后重新渲染。
     * 
     * @param width 渲染宽度
     * @param height 渲染高度
     * @param dx X方向平移量（屏幕坐标）
     * @param dy Y方向平移量（屏幕坐标）
     * @return 渲染结果
     */
    public RenderResult panAndRender(int width, int height, double dx, double dy) {
        if (chartViewer == null) {
            return null;
        }
        
        double worldDx = dx / currentScale;
        double worldDy = dy / currentScale;
        
        this.currentCenterX -= worldDx;
        this.currentCenterY += worldDy;
        
        return renderWithViewport(width, height, currentCenterX, currentCenterY, currentScale);
    }

    /**
     * 缩放后重新渲染。
     * 
     * @param width 渲染宽度
     * @param height 渲染高度
     * @param factor 缩放因子（>1放大，<1缩小）
     * @param pivotX 缩放中心X（屏幕坐标）
     * @param pivotY 缩放中心Y（屏幕坐标）
     * @return 渲染结果
     */
    public RenderResult zoomAndRender(int width, int height, double factor, double pivotX, double pivotY) {
        if (chartViewer == null) {
            return null;
        }
        
        double[] worldCoordBefore = screenToWorld(pivotX, pivotY, width, height);
        if (worldCoordBefore == null) {
            return null;
        }
        
        double newScale = currentScale * factor;
        newScale = Math.max(0.001, Math.min(100000.0, newScale));
        
        this.currentScale = newScale;
        
        double[] worldCoordAfter = screenToWorld(pivotX, pivotY, width, height);
        if (worldCoordAfter != null) {
            this.currentCenterX += worldCoordBefore[0] - worldCoordAfter[0];
            this.currentCenterY += worldCoordBefore[1] - worldCoordAfter[1];
        }
        
        return renderWithViewport(width, height, currentCenterX, currentCenterY, currentScale);
    }

    /**
     * 计算视口范围。
     * 
     * @param width 视口宽度
     * @param height 视口高度
     * @param centerX 中心X
     * @param centerY 中心Y
     * @param scale 缩放比例
     * @return 视口范围
     */
    public Envelope calculateViewExtent(int width, int height, double centerX, double centerY, double scale) {
        if (width <= 0 || height <= 0 || scale <= 0) {
            return null;
        }
        
        double halfWidth = width / (2.0 * scale);
        double halfHeight = height / (2.0 * scale);
        
        return new Envelope(
            centerX - halfWidth,
            centerY - halfHeight,
            centerX + halfWidth,
            centerY + halfHeight
        );
    }

    /**
     * 检查范围是否有效且与海图范围有交集。
     * 
     * @param extent 要检查的范围
     * @return 是否有效
     */
    public boolean isExtentValid(Envelope extent) {
        if (extent == null || chartExtent == null) {
            return false;
        }
        
        return extent.getMinX() <= chartExtent.getMaxX() &&
               extent.getMaxX() >= chartExtent.getMinX() &&
               extent.getMinY() <= chartExtent.getMaxY() &&
               extent.getMaxY() >= chartExtent.getMinY();
    }

    /**
     * 屏幕坐标转世界坐标。
     * 
     * @param screenX 屏幕X坐标
     * @param screenY 屏幕Y坐标
     * @param viewWidth 视口宽度
     * @param viewHeight 视口高度
     * @return 世界坐标数组[x, y]
     */
    public double[] screenToWorld(double screenX, double screenY, int viewWidth, int viewHeight) {
        if (currentScale <= 0) {
            return null;
        }
        
        double worldX = (screenX - viewWidth / 2.0) / currentScale + currentCenterX;
        double worldY = (screenY - viewHeight / 2.0) / currentScale + currentCenterY;
        
        return new double[]{worldX, worldY};
    }

    /**
     * 世界坐标转屏幕坐标。
     * 
     * @param worldX 世界X坐标
     * @param worldY 世界Y坐标
     * @param viewWidth 视口宽度
     * @param viewHeight 视口高度
     * @return 屏幕坐标数组[x, y]
     */
    public double[] worldToScreen(double worldX, double worldY, int viewWidth, int viewHeight) {
        if (currentScale <= 0) {
            return null;
        }
        
        double screenX = (worldX - currentCenterX) * currentScale + viewWidth / 2.0;
        double screenY = (worldY - currentCenterY) * currentScale + viewHeight / 2.0;
        
        return new double[]{screenX, screenY};
    }

    /**
     * 加载并渲染海图。
     * 
     * @param filePath 海图文件路径
     * @param width 渲染宽度
     * @param height 渲染高度
     * @return 渲染结果
     */
    public RenderResult loadAndRender(String filePath, int width, int height) {
        int loadResult = loadChart(filePath);
        if (loadResult != 0) {
            return new RenderResult(loadResult, null, width, height);
        }
        return render(width, height);
    }

    /**
     * 获取当前加载的海图范围。
     * 
     * @return 海图范围，如果未加载返回null
     */
    public Envelope getChartExtent() {
        if (chartViewer == null) {
            return null;
        }
        if (chartExtent == null) {
            chartExtent = chartViewer.getFullExtent();
        }
        return chartExtent;
    }

    /**
     * 获取当前视口范围。
     * 
     * @param viewWidth 视口宽度
     * @param viewHeight 视口高度
     * @return 视口范围
     */
    public Envelope getCurrentViewExtent(int viewWidth, int viewHeight) {
        return calculateViewExtent(viewWidth, viewHeight, currentCenterX, currentCenterY, currentScale);
    }

    /**
     * 获取当前海图文件路径。
     * 
     * @return 海图文件路径
     */
    public String getCurrentChartPath() {
        return currentChartPath;
    }

    /**
     * 获取当前中心X坐标。
     * 
     * @return 中心X坐标
     */
    public double getCurrentCenterX() {
        return currentCenterX;
    }

    /**
     * 获取当前中心Y坐标。
     * 
     * @return 中心Y坐标
     */
    public double getCurrentCenterY() {
        return currentCenterY;
    }

    /**
     * 获取当前缩放比例。
     * 
     * @return 缩放比例
     */
    public double getCurrentScale() {
        return currentScale;
    }

    /**
     * 设置视口参数。
     * 
     * @param centerX 中心X
     * @param centerY 中心Y
     * @param scale 缩放比例
     */
    public void setViewport(double centerX, double centerY, double scale) {
        this.currentCenterX = centerX;
        this.currentCenterY = centerY;
        this.currentScale = scale;
    }

    /**
     * 检查是否已加载海图。
     * 
     * @return 是否已加载海图
     */
    public boolean hasChartLoaded() {
        return chartViewer != null && currentChartPath != null;
    }

    /**
     * 平移视图。
     * 
     * @param dx X方向偏移
     * @param dy Y方向偏移
     */
    public void pan(double dx, double dy) {
        if (chartViewer != null) {
            chartViewer.pan(dx, dy);
        }
    }

    /**
     * 缩放视图。
     * 
     * @param factor 缩放因子
     * @param centerX 缩放中心X
     * @param centerY 缩放中心Y
     */
    public void zoom(double factor, double centerX, double centerY) {
        if (chartViewer != null) {
            chartViewer.zoom(factor, centerX, centerY);
        }
    }

    /**
     * 适应窗口。
     */
    public void fitToWindow() {
        if (chartViewer != null) {
            chartViewer.fitToWindow();
        }
    }

    /**
     * 重置视图。
     */
    public void resetView() {
        if (chartViewer != null) {
            chartViewer.resetView();
        }
    }

    /**
     * 释放资源。
     */
    public void dispose() {
        if (imageDevice != null) {
            imageDevice.dispose();
            imageDevice = null;
        }
        if (chartViewer != null) {
            chartViewer.dispose();
            chartViewer = null;
        }
        currentChartPath = null;
        chartExtent = null;
        lastRenderWidth = 0;
        lastRenderHeight = 0;
    }

    private void ensureImageDevice(int width, int height) {
        if (imageDevice == null || lastRenderWidth != width || lastRenderHeight != height) {
            if (imageDevice != null) {
                imageDevice.dispose();
            }
            imageDevice = new ImageDevice(width, height);
            lastRenderWidth = width;
            lastRenderHeight = height;
        }
    }

    /**
     * 渲染结果。
     */
    public static class RenderResult {
        private final int errorCode;
        private final byte[] pixels;
        private final int width;
        private final int height;

        public RenderResult(int errorCode, byte[] pixels, int width, int height) {
            this.errorCode = errorCode;
            this.pixels = pixels;
            this.width = width;
            this.height = height;
        }

        public int getErrorCode() {
            return errorCode;
        }

        public byte[] getPixels() {
            return pixels;
        }

        public int getWidth() {
            return width;
        }

        public int getHeight() {
            return height;
        }

        public boolean isSuccess() {
            return errorCode == 0 && pixels != null && pixels.length > 0;
        }

        public String getErrorMessage() {
            if (errorCode == 0) {
                return isSuccess() ? "Success" : "No pixel data";
            }
            switch (errorCode) {
                case -1:
                    return "File not found";
                case -2:
                    return "Native library not loaded";
                case -3:
                    return "Load chart failed";
                case -4:
                    return "Invalid view extent";
                default:
                    return "Unknown error: " + errorCode;
            }
        }
    }
}
