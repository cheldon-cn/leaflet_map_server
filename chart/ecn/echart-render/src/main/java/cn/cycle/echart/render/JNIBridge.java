package cn.cycle.echart.render;

import java.util.Objects;

/**
 * JNI渲染桥接。
 * 
 * <p>提供Java与C++渲染引擎之间的桥接接口，用于调用底层渲染功能。</p>
 * 
 * <h2>架构说明</h2>
 * <pre>
 * Java (echart-render)
 *      │
 *      ↓ JNIBridge
 * C++ (cycle/render)
 *      │
 *      ↓
 * OpenGL/Vulkan 渲染
 * </pre>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class JNIBridge {

    private static volatile JNIBridge instance;
    private static volatile boolean nativeLoaded = false;

    static {
        try {
            System.loadLibrary("echart_render");
            nativeLoaded = true;
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native library not loaded: " + e.getMessage());
            nativeLoaded = false;
        }
    }

    private long nativeHandle;
    private volatile boolean initialized;

    private JNIBridge() {
        this.nativeHandle = 0;
        this.initialized = false;
    }

    /**
     * 获取单例实例。
     * 
     * @return JNIBridge实例
     */
    public static synchronized JNIBridge getInstance() {
        if (instance == null) {
            instance = new JNIBridge();
        }
        return instance;
    }

    /**
     * 检查本地库是否已加载。
     * 
     * @return 如果本地库已加载返回true
     */
    public static boolean isNativeLoaded() {
        return nativeLoaded;
    }

    /**
     * 初始化渲染桥接。
     * 
     * @param width 渲染宽度
     * @param height 渲染高度
     * @return 是否初始化成功
     */
    public boolean initialize(int width, int height) {
        if (!nativeLoaded) {
            System.err.println("Native library not loaded, cannot initialize");
            return false;
        }
        
        if (initialized) {
            return true;
        }
        
        try {
            nativeHandle = nativeInitialize(width, height);
            initialized = nativeHandle != 0;
            return initialized;
        } catch (Exception e) {
            System.err.println("Failed to initialize JNI bridge: " + e.getMessage());
            return false;
        }
    }

    /**
     * 渲染帧。
     * 
     * @param context 渲染上下文
     * @return 是否渲染成功
     */
    public boolean renderFrame(RenderContext context) {
        Objects.requireNonNull(context, "context cannot be null");
        
        if (!initialized || nativeHandle == 0) {
            return false;
        }
        
        try {
            return nativeRenderFrame(nativeHandle, context);
        } catch (Exception e) {
            System.err.println("Failed to render frame: " + e.getMessage());
            return false;
        }
    }

    /**
     * 更新视口大小。
     * 
     * @param width 新宽度
     * @param height 新高度
     */
    public void resize(int width, int height) {
        if (!initialized || nativeHandle == 0) {
            return;
        }
        
        try {
            nativeResize(nativeHandle, width, height);
        } catch (Exception e) {
            System.err.println("Failed to resize: " + e.getMessage());
        }
    }

    /**
     * 设置渲染参数。
     * 
     * @param key 参数键
     * @param value 参数值
     */
    public void setRenderParameter(String key, Object value) {
        Objects.requireNonNull(key, "key cannot be null");
        
        if (!initialized || nativeHandle == 0) {
            return;
        }
        
        try {
            if (value instanceof Integer) {
                nativeSetIntParameter(nativeHandle, key, (Integer) value);
            } else if (value instanceof Float) {
                nativeSetFloatParameter(nativeHandle, key, (Float) value);
            } else if (value instanceof String) {
                nativeSetStringParameter(nativeHandle, key, (String) value);
            }
        } catch (Exception e) {
            System.err.println("Failed to set parameter: " + e.getMessage());
        }
    }

    /**
     * 获取渲染统计信息。
     * 
     * @return 统计信息JSON字符串
     */
    public String getRenderStatistics() {
        if (!initialized || nativeHandle == 0) {
            return "{}";
        }
        
        try {
            return nativeGetStatistics(nativeHandle);
        } catch (Exception e) {
            System.err.println("Failed to get statistics: " + e.getMessage());
            return "{}";
        }
    }

    /**
     * 销毁渲染桥接，释放资源。
     */
    public void destroy() {
        if (!initialized || nativeHandle == 0) {
            return;
        }
        
        try {
            nativeDestroy(nativeHandle);
        } catch (Exception e) {
            System.err.println("Failed to destroy: " + e.getMessage());
        } finally {
            nativeHandle = 0;
            initialized = false;
        }
    }

    /**
     * 检查是否已初始化。
     * 
     * @return 如果已初始化返回true
     */
    public boolean isInitialized() {
        return initialized;
    }

    // Native methods
    private native long nativeInitialize(int width, int height);
    private native boolean nativeRenderFrame(long handle, RenderContext context);
    private native void nativeResize(long handle, int width, int height);
    private native void nativeSetIntParameter(long handle, String key, int value);
    private native void nativeSetFloatParameter(long handle, String key, float value);
    private native void nativeSetStringParameter(long handle, String key, String value);
    private native String nativeGetStatistics(long handle);
    private native void nativeDestroy(long handle);
}
