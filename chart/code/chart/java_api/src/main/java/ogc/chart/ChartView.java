package ogc.chart;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.GestureDetector;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * 海图显示视图组件
 * 
 * <p>基于OpenGL ES的海图显示视图，封装了触摸交互和渲染逻辑。</p>
 * 
 * <h3>使用示例：</h3>
 * <pre>{@code
 * <ogc.chart.ChartView
 *     android:id="@+id/chart_view"
 *     android:layout_width="match_parent"
 *     android:layout_height="match_parent" />
 * 
 * ChartView chartView = findViewById(R.id.chart_view);
 * chartView.loadChart("/sdcard/charts/chart.enc");
 * }</pre>
 * 
 * @version 1.0.0
 * @since 1.0.0
 */
public class ChartView extends GLSurfaceView {
    
    private ChartViewer chartViewer;
    private ChartRenderer renderer;
    private ScaleGestureDetector scaleDetector;
    private GestureDetector gestureDetector;
    private ChartViewer.TouchCallback touchCallback;
    
    /**
     * 构造函数
     * 
     * @param context Android上下文
     */
    public ChartView(Context context) {
        super(context);
        init(context);
    }
    
    /**
     * 构造函数
     * 
     * @param context Android上下文
     * @param attrs 属性集
     */
    public ChartView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(context);
    }
    
    private void init(Context context) {
        setEGLContextClientVersion(2);
        setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        
        chartViewer = new ChartViewer(context);
        renderer = new ChartRenderer(chartViewer);
        setRenderer(renderer);
        setRenderMode(RENDERMODE_WHEN_DIRTY);
        
        scaleDetector = new ScaleGestureDetector(context, new ScaleListener());
        gestureDetector = new GestureDetector(context, new GestureListener());
    }
    
    /**
     * 获取ChartViewer实例
     * 
     * @return ChartViewer实例
     */
    public ChartViewer getChartViewer() {
        return chartViewer;
    }
    
    /**
     * 加载海图文件
     * 
     * @param filePath 海图文件路径
     */
    public void loadChart(String filePath) {
        chartViewer.loadChart(filePath);
    }
    
    /**
     * 设置触摸回调
     * 
     * @param callback 触摸回调接口
     */
    public void setTouchCallback(ChartViewer.TouchCallback callback) {
        this.touchCallback = callback;
    }
    
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        scaleDetector.onTouchEvent(event);
        gestureDetector.onTouchEvent(event);
        return true;
    }
    
    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
        if (chartViewer != null) {
            chartViewer.release();
        }
    }
    
    private class ScaleListener extends ScaleGestureDetector.SimpleOnScaleGestureListener {
        @Override
        public boolean onScale(ScaleGestureDetector detector) {
            float scale = detector.getScaleFactor();
            float focusX = detector.getFocusX();
            float focusY = detector.getFocusY();
            
            int currentZoom = chartViewer.getZoomLevel();
            int newZoom = (int) (currentZoom + Math.log(scale) / Math.log(1.5));
            newZoom = Math.max(1, Math.min(20, newZoom));
            
            if (newZoom != currentZoom) {
                chartViewer.setZoomLevel(newZoom);
                requestRender();
            }
            
            if (touchCallback != null) {
                touchCallback.onScale(focusX, focusY, scale);
            }
            
            return true;
        }
    }
    
    private class GestureListener extends GestureDetector.SimpleOnGestureListener {
        @Override
        public boolean onSingleTapConfirmed(MotionEvent e) {
            if (touchCallback != null) {
                touchCallback.onSingleTap(e.getX(), e.getY());
            }
            return true;
        }
        
        @Override
        public boolean onDoubleTap(MotionEvent e) {
            if (touchCallback != null) {
                touchCallback.onDoubleTap(e.getX(), e.getY());
            }
            return true;
        }
        
        @Override
        public void onLongPress(MotionEvent e) {
            if (touchCallback != null) {
                touchCallback.onLongPress(e.getX(), e.getY());
            }
        }
        
        @Override
        public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
            chartViewer.setCenter(
                chartViewer.getCenterLon() + distanceX * 0.0001,
                chartViewer.getCenterLat() - distanceY * 0.0001
            );
            
            if (touchCallback != null) {
                touchCallback.onPan(-distanceX, -distanceY);
            }
            
            requestRender();
            return true;
        }
    }
    
    private static class ChartRenderer implements Renderer {
        private ChartViewer chartViewer;
        
        ChartRenderer(ChartViewer chartViewer) {
            this.chartViewer = chartViewer;
        }
        
        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        }
        
        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            chartViewer.setViewport(width, height);
        }
        
        @Override
        public void onDrawFrame(GL10 gl) {
            chartViewer.render();
        }
    }
}
