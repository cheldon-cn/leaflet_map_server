# Java API封装模块 (java_api)

## 概述

本模块提供海图显示系统的Java API封装，为Android应用提供简洁易用的接口。

## 功能特性

- **ChartViewer**: 海图显示核心类，提供海图加载、显示、交互等功能
- **ChartView**: 基于OpenGL ES的海图显示视图组件
- **ChartConfig**: 海图配置类，用于设置各种参数
- **FeatureInfo**: 海图特征信息类
- **Geometry**: 几何对象类
- **ChartLayer**: 海图图层类

## 构建要求

- Android SDK 21+
- Android Gradle Plugin 7.0+
- CMake 3.16+
- NDK r21+

## 构建配置

在项目的 `settings.gradle` 中添加：

```gradle
include ':java_api'
```

在应用的 `build.gradle` 中添加依赖：

```gradle
dependencies {
    implementation project(':java_api')
}
```

## 使用示例

### 基本使用

```java
import ogc.chart.ChartViewer;
import ogc.chart.ChartView;
import ogc.chart.FeatureInfo;

// 在布局中使用ChartView
<ogc.chart.ChartView
    android:id="@+id/chart_view"
    android:layout_width="match_parent"
    android:layout_height="match_parent" />

// 在Activity中获取并使用
ChartView chartView = findViewById(R.id.chart_view);
ChartViewer viewer = chartView.getChartViewer();

// 加载海图
viewer.loadChart("/sdcard/charts/chart.enc");

// 设置中心点和缩放
viewer.setCenter(116.0, 39.0);
viewer.setZoomLevel(10);
```

### 配置选项

```java
import ogc.chart.ChartConfig;

ChartConfig config = new ChartConfig.Builder()
    .setCachePath(getCacheDir().getAbsolutePath())
    .setMaxCacheSize(100 * 1024 * 1024)  // 100MB
    .setThreadCount(4)
    .setEnableAntiAliasing(true)
    .setEnableHighQuality(true)
    .setDefaultStyle("nautical")
    .setPixelRatio(getResources().getDisplayMetrics().density)
    .build();
```

### 触摸交互

```java
viewer.setTouchCallback(new ChartViewer.TouchCallback() {
    @Override
    public void onSingleTap(float x, float y) {
        // 查询点击位置的特征
        FeatureInfo[] features = viewer.queryFeatures(x, y);
        for (FeatureInfo feature : features) {
            Log.d("Chart", "Feature: " + feature.getFeatureName());
        }
    }
    
    @Override
    public void onDoubleTap(float x, float y) {
        // 双击放大
        viewer.setZoomLevel(viewer.getZoomLevel() + 1);
    }
    
    @Override
    public void onLongPress(float x, float y) {
        // 长按显示详情
    }
    
    @Override
    public void onScale(float focusX, float focusY, float scale) {
        // 缩放手势
    }
    
    @Override
    public void onPan(float dx, float dy) {
        // 平移手势
    }
});
```

### 加载回调

```java
viewer.setLoadCallback(new ChartViewer.LoadCallback() {
    @Override
    public void onLoadSuccess(String chartId) {
        Log.d("Chart", "Chart loaded: " + chartId);
        chartView.requestRender();
    }
    
    @Override
    public void onLoadError(String error) {
        Log.e("Chart", "Load error: " + error);
    }
});
```

### 图层控制

```java
// 设置图层可见性
viewer.setLayerVisible("depth_contours", true);
viewer.setLayerVisible("navigation_aids", true);
viewer.setLayerVisible("land_areas", false);

// 设置显示样式
viewer.setDisplayStyle("nautical");
```

### 坐标转换

```java
// 屏幕坐标转地理坐标
double[] geo = viewer.screenToGeo(screenX, screenY);
double lon = geo[0];
double lat = geo[1];

// 地理坐标转屏幕坐标
float[] screen = viewer.geoToScreen(lon, lat);
float x = screen[0];
float y = screen[1];
```

## API参考

### ChartViewer

| 方法 | 说明 |
|------|------|
| `loadChart(path)` | 加载海图文件 |
| `unloadChart(chartId)` | 卸载海图 |
| `setCenter(lon, lat)` | 设置中心点 |
| `setZoomLevel(level)` | 设置缩放级别 |
| `setRotation(angle)` | 设置旋转角度 |
| `screenToGeo(x, y)` | 屏幕坐标转地理坐标 |
| `geoToScreen(lon, lat)` | 地理坐标转屏幕坐标 |
| `queryFeatures(x, y)` | 查询指定位置的特征 |

### ChartView

| 方法 | 说明 |
|------|------|
| `getChartViewer()` | 获取ChartViewer实例 |
| `loadChart(path)` | 加载海图文件 |
| `setTouchCallback(callback)` | 设置触摸回调 |

### ChartConfig

| 方法 | 说明 |
|------|------|
| `getCachePath()` | 获取缓存路径 |
| `getMaxCacheSize()` | 获取最大缓存大小 |
| `getThreadCount()` | 获取渲染线程数 |
| `isEnableAntiAliasing()` | 是否启用抗锯齿 |

## 注意事项

1. 使用前需要申请存储权限
2. 海图文件需要放在应用可访问的目录
3. OpenGL ES 2.0以上版本支持
4. 建议在后台线程加载大型海图文件

## 版本历史

- v1.0.0: 初始版本，提供基础Java API封装
