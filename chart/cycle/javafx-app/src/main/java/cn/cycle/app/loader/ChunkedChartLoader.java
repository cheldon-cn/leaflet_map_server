package cn.cycle.app.loader;

import cn.cycle.app.event.AppEvent;
import cn.cycle.app.event.AppEventBus;
import cn.cycle.app.event.AppEventType;
import cn.cycle.app.lifecycle.AbstractLifecycleComponent;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicLong;

public class ChunkedChartLoader extends AbstractLifecycleComponent {
    
    private static final int DEFAULT_CHUNK_SIZE = 1024 * 1024;
    private static final int DEFAULT_THREAD_POOL_SIZE = 2;
    
    private final int chunkSize;
    private final ExecutorService executor;
    private final AtomicBoolean loading = new AtomicBoolean(false);
    private final AtomicLong loadedBytes = new AtomicLong(0);
    private final AtomicLong totalBytes = new AtomicLong(0);
    
    private ProgressCallback progressCallback;
    
    public ChunkedChartLoader() {
        this(DEFAULT_CHUNK_SIZE, DEFAULT_THREAD_POOL_SIZE);
    }
    
    public ChunkedChartLoader(int chunkSize, int threadPoolSize) {
        this.chunkSize = chunkSize;
        this.executor = Executors.newFixedThreadPool(threadPoolSize);
    }
    
    public void setProgressCallback(ProgressCallback callback) {
        this.progressCallback = callback;
    }
    
    public CompletableFuture<ByteBuffer> loadFileAsync(File file) {
        if (file == null || !file.exists()) {
            CompletableFuture<ByteBuffer> failed = new CompletableFuture<>();
            failed.completeExceptionally(new IOException("File not found"));
            return failed;
        }
        
        return CompletableFuture.supplyAsync(() -> {
            try {
                return loadFile(file);
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }, executor);
    }
    
    public ByteBuffer loadFile(File file) throws IOException {
        if (loading.get()) {
            throw new IOException("Already loading a file");
        }
        
        loading.set(true);
        loadedBytes.set(0);
        totalBytes.set(file.length());
        
        AppEventBus.getInstance().publish(
            new AppEvent(AppEventType.LOADING_STARTED, this)
                .withData("file", file.getName())
                .withData("totalBytes", totalBytes.get())
        );
        
        try (FileInputStream fis = new FileInputStream(file);
             FileChannel channel = fis.getChannel()) {
            
            ByteBuffer buffer = ByteBuffer.allocate((int) totalBytes.get());
            ByteBuffer tempBuffer = ByteBuffer.allocateDirect(chunkSize);
            
            while (channel.read(tempBuffer) != -1) {
                tempBuffer.flip();
                buffer.put(tempBuffer);
                
                loadedBytes.addAndGet(tempBuffer.limit());
                tempBuffer.clear();
                
                updateProgress();
            }
            
            buffer.flip();
            
            AppEventBus.getInstance().publish(
                new AppEvent(AppEventType.LOADING_COMPLETED, this)
                    .withData("file", file.getName())
                    .withData("loadedBytes", loadedBytes.get())
            );
            
            return buffer;
            
        } finally {
            loading.set(false);
        }
    }
    
    private void updateProgress() {
        double progress = (double) loadedBytes.get() / totalBytes.get();
        
        if (progressCallback != null) {
            progressCallback.onProgress(progress, loadedBytes.get(), totalBytes.get());
        }
        
        AppEventBus.getInstance().publish(
            new AppEvent(AppEventType.LOADING_PROGRESS, this)
                .withData("progress", progress)
                .withData("loadedBytes", loadedBytes.get())
                .withData("totalBytes", totalBytes.get())
        );
    }
    
    public boolean isLoading() {
        return loading.get();
    }
    
    public double getProgress() {
        long total = totalBytes.get();
        if (total == 0) {
            return 0;
        }
        return (double) loadedBytes.get() / total;
    }
    
    public long getLoadedBytes() {
        return loadedBytes.get();
    }
    
    public long getTotalBytes() {
        return totalBytes.get();
    }
    
    @Override
    protected void doInitialize() {
    }
    
    @Override
    protected void doDispose() {
        executor.shutdown();
    }
}
