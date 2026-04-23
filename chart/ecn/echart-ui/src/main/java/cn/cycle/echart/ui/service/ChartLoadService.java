package cn.cycle.echart.ui.service;

import cn.cycle.echart.render.ChartRenderService;
import javafx.concurrent.Service;
import javafx.concurrent.Task;

import java.io.File;

public class ChartLoadService extends Service<ChartLoadService.LoadResult> {

    private File chartFile;
    private int renderWidth;
    private int renderHeight;
    private ChartRenderService chartRenderService;
    private ChartCacheService cacheService;

    public static class LoadResult {
        private final boolean success;
        private final byte[] pixels;
        private final int width;
        private final int height;
        private final String errorMessage;
        private final String fileName;
        private final boolean fromCache;

        public LoadResult(boolean success, byte[] pixels, int width, int height, 
                         String errorMessage, String fileName, boolean fromCache) {
            this.success = success;
            this.pixels = pixels;
            this.width = width;
            this.height = height;
            this.errorMessage = errorMessage;
            this.fileName = fileName;
            this.fromCache = fromCache;
        }

        public static LoadResult success(byte[] pixels, int width, int height, String fileName) {
            return new LoadResult(true, pixels, width, height, null, fileName, false);
        }
        
        public static LoadResult fromCache(byte[] pixels, int width, int height, String fileName) {
            return new LoadResult(true, pixels, width, height, null, fileName, true);
        }

        public static LoadResult failure(String errorMessage, String fileName) {
            return new LoadResult(false, null, 0, 0, errorMessage, fileName, false);
        }

        public boolean isSuccess() {
            return success;
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

        public String getErrorMessage() {
            return errorMessage;
        }

        public String getFileName() {
            return fileName;
        }
        
        public boolean isFromCache() {
            return fromCache;
        }
    }

    public ChartLoadService() {
        this.cacheService = ChartCacheService.getInstance();
    }

    public void setChartFile(File chartFile) {
        this.chartFile = chartFile;
    }

    public void setRenderSize(int width, int height) {
        this.renderWidth = width;
        this.renderHeight = height;
    }

    public void setChartRenderService(ChartRenderService service) {
        this.chartRenderService = service;
    }

    @Override
    protected Task<LoadResult> createTask() {
        return new Task<LoadResult>() {
            @Override
            protected LoadResult call() throws Exception {
                if (chartFile == null || !chartFile.exists()) {
                    return LoadResult.failure("文件不存在", chartFile != null ? chartFile.getName() : "null");
                }

                String cacheKey = cacheService.generateCacheKey(chartFile, renderWidth, renderHeight);
                
                ChartCacheService.CacheEntry cachedEntry = cacheService.get(cacheKey);
                if (cachedEntry != null) {
                    updateMessage("从缓存加载: " + chartFile.getName());
                    updateProgress(100, 100);
                    return LoadResult.fromCache(cachedEntry.getPixels(), 
                            cachedEntry.getWidth(), cachedEntry.getHeight(), chartFile.getName());
                }

                updateMessage("正在加载海图: " + chartFile.getName());
                updateProgress(0, 100);

                if (chartRenderService == null) {
                    chartRenderService = new ChartRenderService();
                }

                updateProgress(20, 100);
                updateMessage("正在解析海图数据...");

                ChartRenderService.RenderResult result = chartRenderService.loadAndRender(
                        chartFile.getAbsolutePath(), renderWidth, renderHeight);

                updateProgress(80, 100);

                if (result == null) {
                    return LoadResult.failure("渲染失败", chartFile.getName());
                }

                if (!result.isSuccess()) {
                    return LoadResult.failure(result.getErrorMessage(), chartFile.getName());
                }

                cacheService.put(cacheKey, result.getPixels(), result.getWidth(), result.getHeight());

                updateProgress(100, 100);
                updateMessage("加载完成");

                return LoadResult.success(result.getPixels(), result.getWidth(), 
                        result.getHeight(), chartFile.getName());
            }
        };
    }

    public ChartRenderService getChartRenderService() {
        return chartRenderService;
    }
}
