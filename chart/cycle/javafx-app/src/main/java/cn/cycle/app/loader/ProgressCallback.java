package cn.cycle.app.loader;

public interface ProgressCallback {
    
    void onProgress(double progress, long loadedBytes, long totalBytes);
    
    void onMessage(String message);
    
    void onError(String error);
    
    void onComplete();
}
