package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class PositionProvider extends NativeObject {

    static {
        JniBridge.initialize();
    }

    PositionProvider(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreateSerial(String port, int baudRate);
    private static native long nativeCreateNetwork(String host, int port);
    private static native void nativeDestroy(long ptr);
    private native boolean nativeConnect(long ptr);
    private native void nativeDisconnect(long ptr);
    private native boolean nativeIsConnected(long ptr);
}
