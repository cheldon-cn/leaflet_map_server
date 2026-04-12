package cn.cycle.chart.jni;

public abstract class NativeObject implements AutoCloseable {

    private long nativePtr;
    private boolean disposed;

    protected NativeObject() {
        this.nativePtr = 0;
        this.disposed = false;
    }

    protected final void setNativePtr(long ptr) {
        this.nativePtr = ptr;
    }

    public final long getNativePtr() {
        return nativePtr;
    }

    public final boolean isDisposed() {
        return disposed || nativePtr == 0;
    }

    protected final void checkNotDisposed() {
        if (disposed || nativePtr == 0) {
            throw new IllegalStateException("Native object has been disposed");
        }
    }

    protected abstract void nativeDispose(long ptr);

    @Override
    public void close() {
        dispose();
    }

    public synchronized void dispose() {
        if (!disposed && nativePtr != 0) {
            nativeDispose(nativePtr);
            nativePtr = 0;
            disposed = true;
        }
    }

    @Override
    protected void finalize() throws Throwable {
        try {
            if (!disposed && nativePtr != 0) {
                System.err.println("[WARNING] NativeObject finalized without explicit dispose() - ptr: " + nativePtr);
            }
        } finally {
            super.finalize();
        }
    }
}
