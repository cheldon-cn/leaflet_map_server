package cn.cycle.chart.jni;

public final class JniBridge {

    private static volatile boolean initialized = false;
    private static final Object lock = new Object();

    static {
        System.loadLibrary("ogc_chart_jni");
    }

    public static void initialize() {
        if (!initialized) {
            synchronized (lock) {
                if (!initialized) {
                    nativeInitialize();
                    initialized = true;
                }
            }
        }
    }

    public static void shutdown() {
        if (initialized) {
            synchronized (lock) {
                if (initialized) {
                    nativeShutdown();
                    initialized = false;
                }
            }
        }
    }

    public static boolean isInitialized() {
        return initialized;
    }

    private static native void nativeInitialize();
    private static native void nativeShutdown();
}
