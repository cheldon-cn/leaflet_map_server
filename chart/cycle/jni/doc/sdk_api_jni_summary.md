# SDK C API JNI 接口汇总

> 生成日期: 2026-04-19
> 源文件: cycle/jni/src/native/*_jni.cpp
> JNI函数总数: 841
> Java类总数: 103

---

## 一、总体统计

| 模块 | 类数量 | JNI函数数量 |
|------|--------|-------------|
| Error | 1 | 1 |
| MemoryUtils | 1 | 5 |
| SDKVersion | 1 | 5 |
| adapter | 1 | 5 |
| alert | 4 | 33 |
| base | 2 | 16 |
| cache | 9 | 65 |
| core | 4 | 58 |
| draw | 9 | 61 |
| exception | 3 | 9 |
| feature | 5 | 60 |
| geometry | 12 | 103 |
| graph | 8 | 44 |
| health | 1 | 8 |
| layer | 8 | 88 |
| loader | 2 | 10 |
| navi | 14 | 138 |
| parser | 4 | 30 |
| perf | 2 | 8 |
| plugin | 2 | 12 |
| proj | 2 | 27 |
| recovery | 3 | 21 |
| symbology | 5 | 34 |
| **合计** | **103** | **841** |

---

## 二、各模块JNI接口详细列表

### 2.1 Error 模块

#### cn.cycle.chart.api.Error (1个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jstring | nativeGetMessage | `Java_cn_cycle_chart_api_Error_nativeGetMessage` | util_ext_jni.cpp |

### 2.2 Memoryutils 模块

#### cn.cycle.chart.api.MemoryUtils (5个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | void | nativeFree | `Java_cn_cycle_chart_api_MemoryUtils_nativeFree` | util_ext_jni.cpp |
| 2 | void | nativeFreeMemory | `Java_cn_cycle_chart_api_MemoryUtils_nativeFreeMemory` | util_ext_jni.cpp |
| 3 | void | nativeFreeString | `Java_cn_cycle_chart_api_MemoryUtils_nativeFreeString` | util_ext_jni.cpp |
| 4 | void | nativeStringArrayDestroy | `Java_cn_cycle_chart_api_MemoryUtils_nativeStringArrayDestroy` | util_ext_jni.cpp |
| 5 | void | nativeGeometryArrayDestroy | `Java_cn_cycle_chart_api_MemoryUtils_nativeGeometryArrayDestroy` | util_ext_jni.cpp |

### 2.3 Sdkversion 模块

#### cn.cycle.chart.api.SDKVersion (5个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jstring | nativeGetVersion | `Java_cn_cycle_chart_api_SDKVersion_nativeGetVersion` | util_ext_jni.cpp |
| 2 | jint | nativeGetVersionMajor | `Java_cn_cycle_chart_api_SDKVersion_nativeGetVersionMajor` | util_ext_jni.cpp |
| 3 | jint | nativeGetVersionMinor | `Java_cn_cycle_chart_api_SDKVersion_nativeGetVersionMinor` | util_ext_jni.cpp |
| 4 | jint | nativeGetVersionPatch | `Java_cn_cycle_chart_api_SDKVersion_nativeGetVersionPatch` | util_ext_jni.cpp |
| 5 | jstring | nativeGetBuildDate | `Java_cn_cycle_chart_api_SDKVersion_nativeGetBuildDate` | util_ext_jni.cpp |

### 2.4 Adapter 模块

#### cn.cycle.chart.api.adapter.ImageDevice (5个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_adapter_ImageDevice_nativeCreate` | adapter_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_adapter_ImageDevice_nativeDestroy` | adapter_jni.cpp |
| 3 | void | nativeResize | `Java_cn_cycle_chart_api_adapter_ImageDevice_nativeResize` | adapter_jni.cpp |
| 4 | void | nativeClear | `Java_cn_cycle_chart_api_adapter_ImageDevice_nativeClear` | adapter_jni.cpp |
| 5 | jbyteArray | nativeGetPixels | `Java_cn_cycle_chart_api_adapter_ImageDevice_nativeGetPixels` | adapter_jni.cpp |

### 2.5 Alert 模块

#### cn.cycle.chart.api.alert.Alert (17个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_alert_Alert_nativeCreate` | alert_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_alert_Alert_nativeDestroy` | alert_jni.cpp |
| 3 | jint | nativeGetType | `Java_cn_cycle_chart_api_alert_Alert_nativeGetType` | alert_jni.cpp |
| 4 | jint | nativeGetLevel | `Java_cn_cycle_chart_api_alert_Alert_nativeGetLevel` | alert_jni.cpp |
| 5 | jstring | nativeGetMessage | `Java_cn_cycle_chart_api_alert_Alert_nativeGetMessage` | alert_jni.cpp |
| 6 | jdouble | nativeGetTimestamp | `Java_cn_cycle_chart_api_alert_Alert_nativeGetTimestamp` | alert_jni.cpp |
| 7 | jlong | nativeGetId | `Java_cn_cycle_chart_api_alert_Alert_nativeGetId` | alert_jni.cpp |
| 8 | jint | nativeGetSeverity | `Java_cn_cycle_chart_api_alert_Alert_nativeGetSeverity` | alert_jni.cpp |
| 9 | void | nativeSetSeverity | `Java_cn_cycle_chart_api_alert_Alert_nativeSetSeverity` | alert_jni.cpp |
| 10 | jdoubleArray | nativeGetPosition | `Java_cn_cycle_chart_api_alert_Alert_nativeGetPosition` | alert_jni.cpp |
| 11 | void | nativeSetPosition | `Java_cn_cycle_chart_api_alert_Alert_nativeSetPosition` | alert_jni.cpp |
| 12 | void | nativeSetId | `Java_cn_cycle_chart_api_alert_Alert_nativeSetId` | alert_jni.cpp |
| 13 | void | nativeSetMessage | `Java_cn_cycle_chart_api_alert_Alert_nativeSetMessage` | alert_jni.cpp |
| 14 | void | nativeSetTimestamp | `Java_cn_cycle_chart_api_alert_Alert_nativeSetTimestamp` | alert_jni.cpp |
| 15 | void | nativeSetAcknowledged | `Java_cn_cycle_chart_api_alert_Alert_nativeSetAcknowledged` | alert_jni.cpp |
| 16 | jboolean | nativeIsAcknowledged | `Java_cn_cycle_chart_api_alert_Alert_nativeIsAcknowledged` | alert_jni.cpp |
| 17 | void | nativeArrayDestroy | `Java_cn_cycle_chart_api_alert_Alert_nativeArrayDestroy` | alert_jni.cpp |

#### cn.cycle.chart.api.alert.AlertEngine (14个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeCreate` | alert_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeDestroy` | alert_jni.cpp |
| 3 | jint | nativeInitialize | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeInitialize` | alert_jni.cpp |
| 4 | void | nativeShutdown | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeShutdown` | alert_jni.cpp |
| 5 | void | nativeCheckAll | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeCheckAll` | alert_jni.cpp |
| 6 | jlongArray | nativeGetActiveAlertIds | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeGetActiveAlertIds` | alert_jni.cpp |
| 7 | void | nativeAcknowledgeAlert | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeAcknowledgeAlert` | alert_jni.cpp |
| 8 | jint | nativeAddAlert | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeAddAlert` | alert_jni.cpp |
| 9 | jint | nativeRemoveAlert | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeRemoveAlert` | alert_jni.cpp |
| 10 | jlong | nativeGetAlert | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeGetAlert` | alert_jni.cpp |
| 11 | jint | nativeGetAlertCount | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeGetAlertCount` | alert_jni.cpp |
| 12 | void | nativeClearAlerts | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeClearAlerts` | alert_jni.cpp |
| 13 | void | nativeSetCallback | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeSetCallback` | alert_jni.cpp |
| 14 | void | nativeFreeAlerts | `Java_cn_cycle_chart_api_alert_AlertEngine_nativeFreeAlerts` | alert_jni.cpp |

#### cn.cycle.chart.api.alert.CpaCalculator (1个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jobject | nativeCalculate | `Java_cn_cycle_chart_api_alert_CpaCalculator_nativeCalculate` | alert_jni.cpp |

#### cn.cycle.chart.api.alert.UkcCalculator (1个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jobject | nativeCalculate | `Java_cn_cycle_chart_api_alert_UkcCalculator_nativeCalculate` | alert_jni.cpp |

### 2.6 Base 模块

#### cn.cycle.chart.api.base.Logger (10个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeGetInstance | `Java_cn_cycle_chart_api_base_Logger_nativeGetInstance` | logger_jni.cpp |
| 2 | void | nativeSetLevel | `Java_cn_cycle_chart_api_base_Logger_nativeSetLevel` | logger_jni.cpp |
| 3 | void | nativeTrace | `Java_cn_cycle_chart_api_base_Logger_nativeTrace` | logger_jni.cpp |
| 4 | void | nativeDebug | `Java_cn_cycle_chart_api_base_Logger_nativeDebug` | logger_jni.cpp |
| 5 | void | nativeInfo | `Java_cn_cycle_chart_api_base_Logger_nativeInfo` | logger_jni.cpp |
| 6 | void | nativeWarning | `Java_cn_cycle_chart_api_base_Logger_nativeWarning` | logger_jni.cpp |
| 7 | void | nativeError | `Java_cn_cycle_chart_api_base_Logger_nativeError` | logger_jni.cpp |
| 8 | void | nativeFatal | `Java_cn_cycle_chart_api_base_Logger_nativeFatal` | logger_jni.cpp |
| 9 | void | nativeSetLogFile | `Java_cn_cycle_chart_api_base_Logger_nativeSetLogFile` | logger_jni.cpp |
| 10 | void | nativeSetConsoleOutput | `Java_cn_cycle_chart_api_base_Logger_nativeSetConsoleOutput` | logger_jni.cpp |

#### cn.cycle.chart.api.base.PerformanceMonitor (6个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeGetInstance | `Java_cn_cycle_chart_api_base_PerformanceMonitor_nativeGetInstance` | render_perf_jni.cpp |
| 2 | void | nativeStartFrame | `Java_cn_cycle_chart_api_base_PerformanceMonitor_nativeStartFrame` | render_perf_jni.cpp |
| 3 | void | nativeEndFrame | `Java_cn_cycle_chart_api_base_PerformanceMonitor_nativeEndFrame` | render_perf_jni.cpp |
| 4 | jdouble | nativeGetFps | `Java_cn_cycle_chart_api_base_PerformanceMonitor_nativeGetFps` | render_perf_jni.cpp |
| 5 | jlong | nativeGetMemoryUsed | `Java_cn_cycle_chart_api_base_PerformanceMonitor_nativeGetMemoryUsed` | render_perf_jni.cpp |
| 6 | void | nativeReset | `Java_cn_cycle_chart_api_base_PerformanceMonitor_nativeReset` | render_perf_jni.cpp |

### 2.7 Cache 模块

#### cn.cycle.chart.api.cache.CacheManager (10个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_cache_CacheManager_nativeCreate` | offline_cache_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_cache_CacheManager_nativeDestroy` | offline_cache_jni.cpp |
| 3 | jlong | nativeGetCacheCount | `Java_cn_cycle_chart_api_cache_CacheManager_nativeGetCacheCount` | offline_cache_jni.cpp |
| 4 | jlong | nativeGetCache | `Java_cn_cycle_chart_api_cache_CacheManager_nativeGetCache` | offline_cache_jni.cpp |
| 5 | void | nativeAddCache | `Java_cn_cycle_chart_api_cache_CacheManager_nativeAddCache` | offline_cache_jni.cpp |
| 6 | void | nativeRemoveCache | `Java_cn_cycle_chart_api_cache_CacheManager_nativeRemoveCache` | offline_cache_jni.cpp |
| 7 | void | nativeClearAll | `Java_cn_cycle_chart_api_cache_CacheManager_nativeClearAll` | offline_cache_jni.cpp |
| 8 | jlong | nativeGetTotalSize | `Java_cn_cycle_chart_api_cache_CacheManager_nativeGetTotalSize` | offline_cache_jni.cpp |
| 9 | jlong | nativeGetTotalTileCount | `Java_cn_cycle_chart_api_cache_CacheManager_nativeGetTotalTileCount` | offline_cache_jni.cpp |
| 10 | void | nativeFlushAll | `Java_cn_cycle_chart_api_cache_CacheManager_nativeFlushAll` | offline_cache_jni.cpp |

#### cn.cycle.chart.api.cache.DataEncryption (4个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_cache_DataEncryption_nativeCreate` | encryption_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_cache_DataEncryption_nativeDestroy` | encryption_jni.cpp |
| 3 | jbyteArray | nativeEncrypt | `Java_cn_cycle_chart_api_cache_DataEncryption_nativeEncrypt` | encryption_jni.cpp |
| 4 | jbyteArray | nativeDecrypt | `Java_cn_cycle_chart_api_cache_DataEncryption_nativeDecrypt` | encryption_jni.cpp |

#### cn.cycle.chart.api.cache.DiskTileCache (1个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_cache_DiskTileCache_nativeCreate` | tile_cache_jni.cpp |

#### cn.cycle.chart.api.cache.MemoryTileCache (2个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_cache_MemoryTileCache_nativeCreate` | tile_cache_jni.cpp |
| 2 | jlong | nativeGetMemoryUsage | `Java_cn_cycle_chart_api_cache_MemoryTileCache_nativeGetMemoryUsage` | tile_cache_jni.cpp |

#### cn.cycle.chart.api.cache.MultiLevelTileCache (1个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_cache_MultiLevelTileCache_nativeCreate` | tile_cache_jni.cpp |

#### cn.cycle.chart.api.cache.OfflineRegion (6个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_cache_OfflineRegion_nativeCreate` | offline_cache_ext_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_cache_OfflineRegion_nativeDestroy` | offline_cache_ext_jni.cpp |
| 3 | jboolean | nativeDownload | `Java_cn_cycle_chart_api_cache_OfflineRegion_nativeDownload` | offline_cache_ext_jni.cpp |
| 4 | jfloat | nativeGetProgress | `Java_cn_cycle_chart_api_cache_OfflineRegion_nativeGetProgress` | offline_cache_ext_jni.cpp |
| 5 | jstring | nativeGetId | `Java_cn_cycle_chart_api_cache_OfflineRegion_nativeGetId` | offline_cache_ext_jni.cpp |
| 6 | jstring | nativeGetName | `Java_cn_cycle_chart_api_cache_OfflineRegion_nativeGetName` | offline_cache_ext_jni.cpp |

#### cn.cycle.chart.api.cache.OfflineStorage (20个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeGetRegionById | `Java_cn_cycle_chart_api_cache_OfflineStorage_nativeGetRegionById` | offline_cache_ext_jni.cpp |
| 2 | void | nativeRemoveRegion | `Java_cn_cycle_chart_api_cache_OfflineStorage_nativeRemoveRegion` | offline_cache_ext_jni.cpp |
| 3 | jboolean | nativeRemoveRegionById | `Java_cn_cycle_chart_api_cache_OfflineStorage_nativeRemoveRegionById` | offline_cache_ext_jni.cpp |
| 4 | jboolean | nativeStoreChart | `Java_cn_cycle_chart_api_cache_OfflineStorage_nativeStoreChart` | offline_cache_ext_jni.cpp |
| 5 | jstring | nativeGetChartPath | `Java_cn_cycle_chart_api_cache_OfflineStorage_nativeGetChartPath` | offline_cache_ext_jni.cpp |
| 6 | jboolean | nativeRemoveChart | `Java_cn_cycle_chart_api_cache_OfflineStorage_nativeRemoveChart` | offline_cache_ext_jni.cpp |
| 7 | jlong | nativeCreate | `Java_cn_cycle_chart_api_cache_OfflineStorage_nativeCreate` | offline_cache_jni.cpp |
| 8 | void | nativeDestroy | `Java_cn_cycle_chart_api_cache_OfflineStorage_nativeDestroy` | offline_cache_jni.cpp |
| 9 | jlong | nativeCreateRegion | `Java_cn_cycle_chart_api_cache_OfflineStorage_nativeCreateRegion` | offline_cache_jni.cpp |
| 10 | jlong | nativeGetRegionCount | `Java_cn_cycle_chart_api_cache_OfflineStorage_nativeGetRegionCount` | offline_cache_jni.cpp |
| 11 | jlong | nativeGetRegion | `Java_cn_cycle_chart_api_cache_OfflineStorage_nativeGetRegion` | offline_cache_jni.cpp |
| 12 | jint | nativeStartDownload | `Java_cn_cycle_chart_api_cache_OfflineStorage_nativeStartDownload` | offline_cache_jni.cpp |
| 13 | jint | nativePauseDownload | `Java_cn_cycle_chart_api_cache_OfflineStorage_nativePauseDownload` | offline_cache_jni.cpp |
| 14 | jint | nativeResumeDownload | `Java_cn_cycle_chart_api_cache_OfflineStorage_nativeResumeDownload` | offline_cache_jni.cpp |
| 15 | jint | nativeCancelDownload | `Java_cn_cycle_chart_api_cache_OfflineStorage_nativeCancelDownload` | offline_cache_jni.cpp |
| 16 | jboolean | nativeIsDownloading | `Java_cn_cycle_chart_api_cache_OfflineStorage_nativeIsDownloading` | offline_cache_jni.cpp |
| 17 | jdouble | nativeGetDownloadProgress | `Java_cn_cycle_chart_api_cache_OfflineStorage_nativeGetDownloadProgress` | offline_cache_jni.cpp |
| 18 | jlong | nativeGetStorageSize | `Java_cn_cycle_chart_api_cache_OfflineStorage_nativeGetStorageSize` | offline_cache_jni.cpp |
| 19 | jlong | nativeGetUsedSize | `Java_cn_cycle_chart_api_cache_OfflineStorage_nativeGetUsedSize` | offline_cache_jni.cpp |
| 20 | jlong | nativeGetChartCount | `Java_cn_cycle_chart_api_cache_OfflineStorage_nativeGetChartCount` | offline_cache_jni.cpp |

#### cn.cycle.chart.api.cache.TileCache (13个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_cache_TileCache_nativeCreate` | tile_cache_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_cache_TileCache_nativeDestroy` | tile_cache_jni.cpp |
| 3 | jboolean | nativeHasTile | `Java_cn_cycle_chart_api_cache_TileCache_nativeHasTile` | tile_cache_jni.cpp |
| 4 | jlong | nativeGetTilePtr | `Java_cn_cycle_chart_api_cache_TileCache_nativeGetTilePtr` | tile_cache_jni.cpp |
| 5 | void | nativePutTile | `Java_cn_cycle_chart_api_cache_TileCache_nativePutTile` | tile_cache_jni.cpp |
| 6 | void | nativeRemoveTile | `Java_cn_cycle_chart_api_cache_TileCache_nativeRemoveTile` | tile_cache_jni.cpp |
| 7 | void | nativeClear | `Java_cn_cycle_chart_api_cache_TileCache_nativeClear` | tile_cache_jni.cpp |
| 8 | jlong | nativeGetSize | `Java_cn_cycle_chart_api_cache_TileCache_nativeGetSize` | tile_cache_jni.cpp |
| 9 | jlong | nativeGetMaxSize | `Java_cn_cycle_chart_api_cache_TileCache_nativeGetMaxSize` | tile_cache_jni.cpp |
| 10 | void | nativeSetMaxSize | `Java_cn_cycle_chart_api_cache_TileCache_nativeSetMaxSize` | tile_cache_jni.cpp |
| 11 | jint | nativeGetCount | `Java_cn_cycle_chart_api_cache_TileCache_nativeGetCount` | tile_cache_jni.cpp |
| 12 | jlong | nativeGetCapacity | `Java_cn_cycle_chart_api_cache_TileCache_nativeGetCapacity` | tile_cache_jni.cpp |
| 13 | void | nativeSetCapacity | `Java_cn_cycle_chart_api_cache_TileCache_nativeSetCapacity` | tile_cache_jni.cpp |

#### cn.cycle.chart.api.cache.TileKey (8个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jobject | nativeCreate | `Java_cn_cycle_chart_api_cache_TileKey_nativeCreate` | tile_cache_jni.cpp |
| 2 | jboolean | nativeEquals | `Java_cn_cycle_chart_api_cache_TileKey_nativeEquals` | tile_cache_jni.cpp |
| 3 | jstring | nativeToString | `Java_cn_cycle_chart_api_cache_TileKey_nativeToString` | tile_cache_jni.cpp |
| 4 | jlong | nativeToIndex | `Java_cn_cycle_chart_api_cache_TileKey_nativeToIndex` | tile_cache_jni.cpp |
| 5 | jobject | nativeGetParent | `Java_cn_cycle_chart_api_cache_TileKey_nativeGetParent` | tile_cache_jni.cpp |
| 6 | jobjectArray | nativeGetChildren | `Java_cn_cycle_chart_api_cache_TileKey_nativeGetChildren` | tile_cache_jni.cpp |
| 7 | jobject | nativeFromString | `Java_cn_cycle_chart_api_cache_TileKey_nativeFromString` | tile_cache_jni.cpp |
| 8 | jlong | nativeToEnvelope | `Java_cn_cycle_chart_api_cache_TileKey_nativeToEnvelope` | tile_cache_jni.cpp |

### 2.8 Core 模块

#### cn.cycle.chart.api.core.ChartConfig (8个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_core_ChartConfig_nativeCreate` | chart_config_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_core_ChartConfig_nativeDestroy` | chart_config_jni.cpp |
| 3 | jint | nativeGetDisplayMode | `Java_cn_cycle_chart_api_core_ChartConfig_nativeGetDisplayMode` | chart_config_jni.cpp |
| 4 | void | nativeSetDisplayMode | `Java_cn_cycle_chart_api_core_ChartConfig_nativeSetDisplayMode` | chart_config_jni.cpp |
| 5 | jboolean | nativeGetShowGrid | `Java_cn_cycle_chart_api_core_ChartConfig_nativeGetShowGrid` | chart_config_jni.cpp |
| 6 | void | nativeSetShowGrid | `Java_cn_cycle_chart_api_core_ChartConfig_nativeSetShowGrid` | chart_config_jni.cpp |
| 7 | jdouble | nativeGetDpi | `Java_cn_cycle_chart_api_core_ChartConfig_nativeGetDpi` | chart_config_jni.cpp |
| 8 | void | nativeSetDpi | `Java_cn_cycle_chart_api_core_ChartConfig_nativeSetDpi` | chart_config_jni.cpp |

#### cn.cycle.chart.api.core.ChartViewer (15个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_core_ChartViewer_nativeCreate` | chart_viewer_jni.cpp |
| 2 | jint | nativeInitialize | `Java_cn_cycle_chart_api_core_ChartViewer_nativeInitialize` | chart_viewer_jni.cpp |
| 3 | void | nativeShutdown | `Java_cn_cycle_chart_api_core_ChartViewer_nativeShutdown` | chart_viewer_jni.cpp |
| 4 | void | nativeDisposeImpl | `Java_cn_cycle_chart_api_core_ChartViewer_nativeDisposeImpl` | chart_viewer_jni.cpp |
| 5 | jint | nativeLoadChart | `Java_cn_cycle_chart_api_core_ChartViewer_nativeLoadChart` | chart_viewer_jni.cpp |
| 6 | jint | nativeRender | `Java_cn_cycle_chart_api_core_ChartViewer_nativeRender` | chart_viewer_jni.cpp |
| 7 | void | nativeSetViewport | `Java_cn_cycle_chart_api_core_ChartViewer_nativeSetViewport` | chart_viewer_jni.cpp |
| 8 | void | nativeGetViewport | `Java_cn_cycle_chart_api_core_ChartViewer_nativeGetViewport` | chart_viewer_jni.cpp |
| 9 | void | nativePan | `Java_cn_cycle_chart_api_core_ChartViewer_nativePan` | chart_viewer_jni.cpp |
| 10 | void | nativeZoom | `Java_cn_cycle_chart_api_core_ChartViewer_nativeZoom` | chart_viewer_jni.cpp |
| 11 | jlong | nativeQueryFeature | `Java_cn_cycle_chart_api_core_ChartViewer_nativeQueryFeature` | chart_viewer_jni.cpp |
| 12 | void | nativeScreenToWorld | `Java_cn_cycle_chart_api_core_ChartViewer_nativeScreenToWorld` | chart_viewer_jni.cpp |
| 13 | void | nativeWorldToScreen | `Java_cn_cycle_chart_api_core_ChartViewer_nativeWorldToScreen` | chart_viewer_jni.cpp |
| 14 | jlong | nativeGetViewportPtr | `Java_cn_cycle_chart_api_core_ChartViewer_nativeGetViewportPtr` | chart_viewer_jni.cpp |
| 15 | void | nativeGetFullExtent | `Java_cn_cycle_chart_api_core_ChartViewer_nativeGetFullExtent` | chart_viewer_jni.cpp |

#### cn.cycle.chart.api.core.RenderContext (15个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_core_RenderContext_nativeCreate` | draw_context_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_core_RenderContext_nativeDestroy` | draw_context_jni.cpp |
| 3 | void | nativeBeginDraw | `Java_cn_cycle_chart_api_core_RenderContext_nativeBeginDraw` | draw_context_jni.cpp |
| 4 | void | nativeEndDraw | `Java_cn_cycle_chart_api_core_RenderContext_nativeEndDraw` | draw_context_jni.cpp |
| 5 | void | nativeClear | `Java_cn_cycle_chart_api_core_RenderContext_nativeClear` | draw_context_jni.cpp |
| 6 | void | nativeDrawGeometry | `Java_cn_cycle_chart_api_core_RenderContext_nativeDrawGeometry` | draw_context_jni.cpp |
| 7 | void | nativeDrawPoint | `Java_cn_cycle_chart_api_core_RenderContext_nativeDrawPoint` | draw_context_jni.cpp |
| 8 | void | nativeDrawLine | `Java_cn_cycle_chart_api_core_RenderContext_nativeDrawLine` | draw_context_jni.cpp |
| 9 | void | nativeDrawRect | `Java_cn_cycle_chart_api_core_RenderContext_nativeDrawRect` | draw_context_jni.cpp |
| 10 | void | nativeFillRect | `Java_cn_cycle_chart_api_core_RenderContext_nativeFillRect` | draw_context_jni.cpp |
| 11 | void | nativeDrawText | `Java_cn_cycle_chart_api_core_RenderContext_nativeDrawText` | draw_context_jni.cpp |
| 12 | void | nativeSetTransform | `Java_cn_cycle_chart_api_core_RenderContext_nativeSetTransform` | draw_context_jni.cpp |
| 13 | void | nativeResetTransform | `Java_cn_cycle_chart_api_core_RenderContext_nativeResetTransform` | draw_context_jni.cpp |
| 14 | void | nativeClip | `Java_cn_cycle_chart_api_core_RenderContext_nativeClip` | draw_context_jni.cpp |
| 15 | void | nativeResetClip | `Java_cn_cycle_chart_api_core_RenderContext_nativeResetClip` | draw_context_jni.cpp |

#### cn.cycle.chart.api.core.Viewport (20个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_core_Viewport_nativeCreate` | viewport_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_core_Viewport_nativeDestroy` | viewport_jni.cpp |
| 3 | jdouble | nativeGetCenterX | `Java_cn_cycle_chart_api_core_Viewport_nativeGetCenterX` | viewport_jni.cpp |
| 4 | jdouble | nativeGetCenterY | `Java_cn_cycle_chart_api_core_Viewport_nativeGetCenterY` | viewport_jni.cpp |
| 5 | jdouble | nativeGetScale | `Java_cn_cycle_chart_api_core_Viewport_nativeGetScale` | viewport_jni.cpp |
| 6 | jdouble | nativeGetRotation | `Java_cn_cycle_chart_api_core_Viewport_nativeGetRotation` | viewport_jni.cpp |
| 7 | void | nativeSetCenter | `Java_cn_cycle_chart_api_core_Viewport_nativeSetCenter` | viewport_jni.cpp |
| 8 | void | nativeSetScale | `Java_cn_cycle_chart_api_core_Viewport_nativeSetScale` | viewport_jni.cpp |
| 9 | void | nativeSetRotation | `Java_cn_cycle_chart_api_core_Viewport_nativeSetRotation` | viewport_jni.cpp |
| 10 | void | nativeGetExtent | `Java_cn_cycle_chart_api_core_Viewport_nativeGetExtent` | viewport_jni.cpp |
| 11 | void | nativeSetExtent | `Java_cn_cycle_chart_api_core_Viewport_nativeSetExtent` | viewport_jni.cpp |
| 12 | void | nativePan | `Java_cn_cycle_chart_api_core_Viewport_nativePan` | viewport_jni.cpp |
| 13 | void | nativeZoom | `Java_cn_cycle_chart_api_core_Viewport_nativeZoom` | viewport_jni.cpp |
| 14 | void | nativeZoomAt | `Java_cn_cycle_chart_api_core_Viewport_nativeZoomAt` | viewport_jni.cpp |
| 15 | void | nativeSetSize | `Java_cn_cycle_chart_api_core_Viewport_nativeSetSize` | viewport_jni.cpp |
| 16 | jlong | nativeGetBounds | `Java_cn_cycle_chart_api_core_Viewport_nativeGetBounds` | viewport_jni.cpp |
| 17 | jdoubleArray | nativeScreenToWorld | `Java_cn_cycle_chart_api_core_Viewport_nativeScreenToWorld` | viewport_jni.cpp |
| 18 | jdoubleArray | nativeWorldToScreen | `Java_cn_cycle_chart_api_core_Viewport_nativeWorldToScreen` | viewport_jni.cpp |
| 19 | jint | nativeZoomToExtent | `Java_cn_cycle_chart_api_core_Viewport_nativeZoomToExtent` | viewport_jni.cpp |
| 20 | jint | nativeZoomToScale | `Java_cn_cycle_chart_api_core_Viewport_nativeZoomToScale` | viewport_jni.cpp |

### 2.9 Draw 模块

#### cn.cycle.chart.api.draw.Brush (6个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_draw_Brush_nativeCreate` | draw_style_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_draw_Brush_nativeDestroy` | draw_style_jni.cpp |
| 3 | jint | nativeGetColor | `Java_cn_cycle_chart_api_draw_Brush_nativeGetColor` | draw_style_jni.cpp |
| 4 | void | nativeSetColor | `Java_cn_cycle_chart_api_draw_Brush_nativeSetColor` | draw_style_jni.cpp |
| 5 | jint | nativeGetStyle | `Java_cn_cycle_chart_api_draw_Brush_nativeGetStyle` | draw_style_jni.cpp |
| 6 | void | nativeSetStyle | `Java_cn_cycle_chart_api_draw_Brush_nativeSetStyle` | draw_style_jni.cpp |

#### cn.cycle.chart.api.draw.Color (5个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jint | nativeFromRgba | `Java_cn_cycle_chart_api_draw_Color_nativeFromRgba` | draw_style_jni.cpp |
| 2 | jint | nativeFromRgb | `Java_cn_cycle_chart_api_draw_Color_nativeFromRgb` | draw_style_jni.cpp |
| 3 | jint | nativeFromHex | `Java_cn_cycle_chart_api_draw_Color_nativeFromHex` | draw_style_jni.cpp |
| 4 | jint | nativeToHex | `Java_cn_cycle_chart_api_draw_Color_nativeToHex` | draw_style_jni.cpp |
| 5 | jintArray | nativeToRgba | `Java_cn_cycle_chart_api_draw_Color_nativeToRgba` | draw_style_jni.cpp |

#### cn.cycle.chart.api.draw.DrawDevice (5个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_draw_DrawDevice_nativeCreate` | draw_context_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_draw_DrawDevice_nativeDestroy` | draw_context_jni.cpp |
| 3 | jint | nativeGetWidth | `Java_cn_cycle_chart_api_draw_DrawDevice_nativeGetWidth` | draw_context_jni.cpp |
| 4 | jint | nativeGetHeight | `Java_cn_cycle_chart_api_draw_DrawDevice_nativeGetHeight` | draw_context_jni.cpp |
| 5 | jlong | nativeGetImage | `Java_cn_cycle_chart_api_draw_DrawDevice_nativeGetImage` | draw_context_jni.cpp |

#### cn.cycle.chart.api.draw.DrawEngine (4个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_draw_DrawEngine_nativeCreate` | draw_context_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_draw_DrawEngine_nativeDestroy` | draw_context_jni.cpp |
| 3 | jboolean | nativeInitialize | `Java_cn_cycle_chart_api_draw_DrawEngine_nativeInitialize` | draw_context_jni.cpp |
| 4 | void | nativeFinalize | `Java_cn_cycle_chart_api_draw_DrawEngine_nativeFinalize` | draw_context_jni.cpp |

#### cn.cycle.chart.api.draw.DrawStyle (8个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_draw_DrawStyle_nativeCreate` | draw_style_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_draw_DrawStyle_nativeDestroy` | draw_style_jni.cpp |
| 3 | jlong | nativeGetPen | `Java_cn_cycle_chart_api_draw_DrawStyle_nativeGetPen` | draw_style_jni.cpp |
| 4 | jlong | nativeGetBrush | `Java_cn_cycle_chart_api_draw_DrawStyle_nativeGetBrush` | draw_style_jni.cpp |
| 5 | jlong | nativeGetFont | `Java_cn_cycle_chart_api_draw_DrawStyle_nativeGetFont` | draw_style_jni.cpp |
| 6 | void | nativeSetPen | `Java_cn_cycle_chart_api_draw_DrawStyle_nativeSetPen` | draw_style_jni.cpp |
| 7 | void | nativeSetBrush | `Java_cn_cycle_chart_api_draw_DrawStyle_nativeSetBrush` | draw_style_jni.cpp |
| 8 | void | nativeSetFont | `Java_cn_cycle_chart_api_draw_DrawStyle_nativeSetFont` | draw_style_jni.cpp |

#### cn.cycle.chart.api.draw.Font (8个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_draw_Font_nativeCreate` | draw_style_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_draw_Font_nativeDestroy` | draw_style_jni.cpp |
| 3 | jstring | nativeGetFamily | `Java_cn_cycle_chart_api_draw_Font_nativeGetFamily` | draw_style_jni.cpp |
| 4 | jdouble | nativeGetSize | `Java_cn_cycle_chart_api_draw_Font_nativeGetSize` | draw_style_jni.cpp |
| 5 | jboolean | nativeIsBold | `Java_cn_cycle_chart_api_draw_Font_nativeIsBold` | draw_style_jni.cpp |
| 6 | jboolean | nativeIsItalic | `Java_cn_cycle_chart_api_draw_Font_nativeIsItalic` | draw_style_jni.cpp |
| 7 | void | nativeSetBold | `Java_cn_cycle_chart_api_draw_Font_nativeSetBold` | draw_style_jni.cpp |
| 8 | void | nativeSetItalic | `Java_cn_cycle_chart_api_draw_Font_nativeSetItalic` | draw_style_jni.cpp |

#### cn.cycle.chart.api.draw.Image (9个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_draw_Image_nativeCreate` | draw_context_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_draw_Image_nativeDestroy` | draw_context_jni.cpp |
| 3 | jint | nativeGetWidth | `Java_cn_cycle_chart_api_draw_Image_nativeGetWidth` | draw_context_jni.cpp |
| 4 | jint | nativeGetHeight | `Java_cn_cycle_chart_api_draw_Image_nativeGetHeight` | draw_context_jni.cpp |
| 5 | jint | nativeGetChannels | `Java_cn_cycle_chart_api_draw_Image_nativeGetChannels` | draw_context_jni.cpp |
| 6 | jlong | nativeLoadFromFile | `Java_cn_cycle_chart_api_draw_Image_nativeLoadFromFile` | draw_context_jni.cpp |
| 7 | jboolean | nativeSaveToFile | `Java_cn_cycle_chart_api_draw_Image_nativeSaveToFile` | draw_context_jni.cpp |
| 8 | jlong | nativeGetDataPtr | `Java_cn_cycle_chart_api_draw_Image_nativeGetDataPtr` | draw_style_jni.cpp |
| 9 | jlong | nativeGetDataConstPtr | `Java_cn_cycle_chart_api_draw_Image_nativeGetDataConstPtr` | draw_style_jni.cpp |

#### cn.cycle.chart.api.draw.ImageDevice (8个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_draw_ImageDevice_nativeCreate` | draw_context_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_draw_ImageDevice_nativeDestroy` | draw_context_jni.cpp |
| 3 | jboolean | nativeResize | `Java_cn_cycle_chart_api_draw_ImageDevice_nativeResize` | draw_context_jni.cpp |
| 4 | void | nativeClear | `Java_cn_cycle_chart_api_draw_ImageDevice_nativeClear` | draw_context_jni.cpp |
| 5 | jbyteArray | nativeGetPixels | `Java_cn_cycle_chart_api_draw_ImageDevice_nativeGetPixels` | draw_context_jni.cpp |
| 6 | jint | nativeGetWidth | `Java_cn_cycle_chart_api_draw_ImageDevice_nativeGetWidth` | draw_context_jni.cpp |
| 7 | jint | nativeGetHeight | `Java_cn_cycle_chart_api_draw_ImageDevice_nativeGetHeight` | draw_context_jni.cpp |
| 8 | jlong | nativeGetNativePtr | `Java_cn_cycle_chart_api_draw_ImageDevice_nativeGetNativePtr` | draw_context_jni.cpp |

#### cn.cycle.chart.api.draw.Pen (8个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_draw_Pen_nativeCreate` | draw_style_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_draw_Pen_nativeDestroy` | draw_style_jni.cpp |
| 3 | jint | nativeGetColor | `Java_cn_cycle_chart_api_draw_Pen_nativeGetColor` | draw_style_jni.cpp |
| 4 | jdouble | nativeGetWidth | `Java_cn_cycle_chart_api_draw_Pen_nativeGetWidth` | draw_style_jni.cpp |
| 5 | void | nativeSetColor | `Java_cn_cycle_chart_api_draw_Pen_nativeSetColor` | draw_style_jni.cpp |
| 6 | void | nativeSetWidth | `Java_cn_cycle_chart_api_draw_Pen_nativeSetWidth` | draw_style_jni.cpp |
| 7 | jint | nativeGetStyle | `Java_cn_cycle_chart_api_draw_Pen_nativeGetStyle` | draw_style_jni.cpp |
| 8 | void | nativeSetStyle | `Java_cn_cycle_chart_api_draw_Pen_nativeSetStyle` | draw_style_jni.cpp |

### 2.10 Exception 模块

#### cn.cycle.chart.api.exception.ChartException (5个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_exception_ChartException_nativeCreate` | exception_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_exception_ChartException_nativeDestroy` | exception_jni.cpp |
| 3 | jint | nativeGetErrorCode | `Java_cn_cycle_chart_api_exception_ChartException_nativeGetErrorCode` | exception_jni.cpp |
| 4 | jstring | nativeGetMessage | `Java_cn_cycle_chart_api_exception_ChartException_nativeGetMessage` | exception_jni.cpp |
| 5 | jstring | nativeGetContext | `Java_cn_cycle_chart_api_exception_ChartException_nativeGetContext` | exception_jni.cpp |

#### cn.cycle.chart.api.exception.JniException (2个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_exception_JniException_nativeCreate` | exception_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_exception_JniException_nativeDestroy` | exception_jni.cpp |

#### cn.cycle.chart.api.exception.RenderException (2个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_exception_RenderException_nativeCreate` | exception_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_exception_RenderException_nativeDestroy` | exception_jni.cpp |

### 2.11 Feature 模块

#### cn.cycle.chart.api.feature.Feature (13个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreateWithDefn | `Java_cn_cycle_chart_api_feature_Feature_nativeCreateWithDefn` | feature_layer_ext_jni.cpp |
| 2 | jlong | nativeGetDefn | `Java_cn_cycle_chart_api_feature_Feature_nativeGetDefn` | feature_layer_ext_jni.cpp |
| 3 | jboolean | nativeIsFieldSet | `Java_cn_cycle_chart_api_feature_Feature_nativeIsFieldSet` | feature_layer_ext_jni.cpp |
| 4 | jboolean | nativeIsFieldNull | `Java_cn_cycle_chart_api_feature_Feature_nativeIsFieldNull` | feature_layer_ext_jni.cpp |
| 5 | jint | nativeGetFieldAsIntegerByName | `Java_cn_cycle_chart_api_feature_Feature_nativeGetFieldAsIntegerByName` | feature_layer_ext_jni.cpp |
| 6 | jdouble | nativeGetFieldAsRealByName | `Java_cn_cycle_chart_api_feature_Feature_nativeGetFieldAsRealByName` | feature_layer_ext_jni.cpp |
| 7 | jstring | nativeGetFieldAsStringByName | `Java_cn_cycle_chart_api_feature_Feature_nativeGetFieldAsStringByName` | feature_layer_ext_jni.cpp |
| 8 | jboolean | nativeSetFieldIntegerByName | `Java_cn_cycle_chart_api_feature_Feature_nativeSetFieldIntegerByName` | feature_layer_ext_jni.cpp |
| 9 | jboolean | nativeSetFieldRealByName | `Java_cn_cycle_chart_api_feature_Feature_nativeSetFieldRealByName` | feature_layer_ext_jni.cpp |
| 10 | jboolean | nativeSetFieldStringByName | `Java_cn_cycle_chart_api_feature_Feature_nativeSetFieldStringByName` | feature_layer_ext_jni.cpp |
| 11 | jlong | nativeStealGeometry | `Java_cn_cycle_chart_api_feature_Feature_nativeStealGeometry` | feature_layer_ext_jni.cpp |
| 12 | jlong | nativeGetEnvelope | `Java_cn_cycle_chart_api_feature_Feature_nativeGetEnvelope` | feature_layer_ext_jni.cpp |
| 13 | jlong | nativeClone | `Java_cn_cycle_chart_api_feature_Feature_nativeClone` | feature_layer_ext_jni.cpp |

#### cn.cycle.chart.api.feature.FeatureDefn (8个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_feature_FeatureDefn_nativeCreate` | feature_layer_ext_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_feature_FeatureDefn_nativeDestroy` | feature_layer_ext_jni.cpp |
| 3 | jstring | nativeGetName | `Java_cn_cycle_chart_api_feature_FeatureDefn_nativeGetName` | feature_layer_ext_jni.cpp |
| 4 | jlong | nativeGetFieldCount | `Java_cn_cycle_chart_api_feature_FeatureDefn_nativeGetFieldCount` | feature_layer_ext_jni.cpp |
| 5 | jlong | nativeGetFieldDefn | `Java_cn_cycle_chart_api_feature_FeatureDefn_nativeGetFieldDefn` | feature_layer_ext_jni.cpp |
| 6 | jint | nativeGetFieldIndex | `Java_cn_cycle_chart_api_feature_FeatureDefn_nativeGetFieldIndex` | feature_layer_ext_jni.cpp |
| 7 | void | nativeAddFieldDefn | `Java_cn_cycle_chart_api_feature_FeatureDefn_nativeAddFieldDefn` | feature_layer_ext_jni.cpp |
| 8 | jlong | nativeGetGeomFieldCount | `Java_cn_cycle_chart_api_feature_FeatureDefn_nativeGetGeomFieldCount` | feature_layer_ext_jni.cpp |

#### cn.cycle.chart.api.feature.FeatureInfo (18个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | void | nativeDestroy | `Java_cn_cycle_chart_api_feature_FeatureInfo_nativeDestroy` | feature_jni.cpp |
| 2 | jlong | nativeGetFid | `Java_cn_cycle_chart_api_feature_FeatureInfo_nativeGetFid` | feature_jni.cpp |
| 3 | void | nativeSetFid | `Java_cn_cycle_chart_api_feature_FeatureInfo_nativeSetFid` | feature_jni.cpp |
| 4 | jlong | nativeGetFieldCount | `Java_cn_cycle_chart_api_feature_FeatureInfo_nativeGetFieldCount` | feature_jni.cpp |
| 5 | jboolean | nativeIsFieldSet | `Java_cn_cycle_chart_api_feature_FeatureInfo_nativeIsFieldSet` | feature_jni.cpp |
| 6 | jboolean | nativeIsFieldNull | `Java_cn_cycle_chart_api_feature_FeatureInfo_nativeIsFieldNull` | feature_jni.cpp |
| 7 | jint | nativeGetFieldAsInteger | `Java_cn_cycle_chart_api_feature_FeatureInfo_nativeGetFieldAsInteger` | feature_jni.cpp |
| 8 | jdouble | nativeGetFieldAsReal | `Java_cn_cycle_chart_api_feature_FeatureInfo_nativeGetFieldAsReal` | feature_jni.cpp |
| 9 | jstring | nativeGetFieldAsString | `Java_cn_cycle_chart_api_feature_FeatureInfo_nativeGetFieldAsString` | feature_jni.cpp |
| 10 | void | nativeSetFieldInteger | `Java_cn_cycle_chart_api_feature_FeatureInfo_nativeSetFieldInteger` | feature_jni.cpp |
| 11 | void | nativeSetFieldReal | `Java_cn_cycle_chart_api_feature_FeatureInfo_nativeSetFieldReal` | feature_jni.cpp |
| 12 | void | nativeSetFieldString | `Java_cn_cycle_chart_api_feature_FeatureInfo_nativeSetFieldString` | feature_jni.cpp |
| 13 | void | nativeSetFieldNull | `Java_cn_cycle_chart_api_feature_FeatureInfo_nativeSetFieldNull` | feature_jni.cpp |
| 14 | jlong | nativeGetGeometry | `Java_cn_cycle_chart_api_feature_FeatureInfo_nativeGetGeometry` | feature_jni.cpp |
| 15 | void | nativeSetGeometry | `Java_cn_cycle_chart_api_feature_FeatureInfo_nativeSetGeometry` | feature_jni.cpp |
| 16 | jlong | nativeGetFieldAsInteger64 | `Java_cn_cycle_chart_api_feature_FeatureInfo_nativeGetFieldAsInteger64` | feature_jni.cpp |
| 17 | void | nativeSetFieldInteger64 | `Java_cn_cycle_chart_api_feature_FeatureInfo_nativeSetFieldInteger64` | feature_jni.cpp |
| 18 | void | nativeArrayDestroy | `Java_cn_cycle_chart_api_feature_FeatureInfo_nativeArrayDestroy` | feature_jni.cpp |

#### cn.cycle.chart.api.feature.FieldDefn (8个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_feature_FieldDefn_nativeCreate` | feature_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_feature_FieldDefn_nativeDestroy` | feature_jni.cpp |
| 3 | jstring | nativeGetName | `Java_cn_cycle_chart_api_feature_FieldDefn_nativeGetName` | feature_jni.cpp |
| 4 | jint | nativeGetType | `Java_cn_cycle_chart_api_feature_FieldDefn_nativeGetType` | feature_jni.cpp |
| 5 | jint | nativeGetWidth | `Java_cn_cycle_chart_api_feature_FieldDefn_nativeGetWidth` | feature_jni.cpp |
| 6 | void | nativeSetWidth | `Java_cn_cycle_chart_api_feature_FieldDefn_nativeSetWidth` | feature_jni.cpp |
| 7 | jint | nativeGetPrecision | `Java_cn_cycle_chart_api_feature_FieldDefn_nativeGetPrecision` | feature_jni.cpp |
| 8 | void | nativeSetPrecision | `Java_cn_cycle_chart_api_feature_FieldDefn_nativeSetPrecision` | feature_jni.cpp |

#### cn.cycle.chart.api.feature.FieldValue (13个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_feature_FieldValue_nativeCreate` | feature_layer_ext_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_feature_FieldValue_nativeDestroy` | feature_layer_ext_jni.cpp |
| 3 | jboolean | nativeIsNull | `Java_cn_cycle_chart_api_feature_FieldValue_nativeIsNull` | feature_layer_ext_jni.cpp |
| 4 | jboolean | nativeIsSet | `Java_cn_cycle_chart_api_feature_FieldValue_nativeIsSet` | feature_layer_ext_jni.cpp |
| 5 | jint | nativeGetAsInteger | `Java_cn_cycle_chart_api_feature_FieldValue_nativeGetAsInteger` | feature_layer_ext_jni.cpp |
| 6 | jlong | nativeGetAsInteger64 | `Java_cn_cycle_chart_api_feature_FieldValue_nativeGetAsInteger64` | feature_layer_ext_jni.cpp |
| 7 | jdouble | nativeGetAsReal | `Java_cn_cycle_chart_api_feature_FieldValue_nativeGetAsReal` | feature_layer_ext_jni.cpp |
| 8 | jstring | nativeGetAsString | `Java_cn_cycle_chart_api_feature_FieldValue_nativeGetAsString` | feature_layer_ext_jni.cpp |
| 9 | void | nativeSetInteger | `Java_cn_cycle_chart_api_feature_FieldValue_nativeSetInteger` | feature_layer_ext_jni.cpp |
| 10 | void | nativeSetInteger64 | `Java_cn_cycle_chart_api_feature_FieldValue_nativeSetInteger64` | feature_layer_ext_jni.cpp |
| 11 | void | nativeSetReal | `Java_cn_cycle_chart_api_feature_FieldValue_nativeSetReal` | feature_layer_ext_jni.cpp |
| 12 | void | nativeSetString | `Java_cn_cycle_chart_api_feature_FieldValue_nativeSetString` | feature_layer_ext_jni.cpp |
| 13 | void | nativeSetNull | `Java_cn_cycle_chart_api_feature_FieldValue_nativeSetNull` | feature_layer_ext_jni.cpp |

### 2.12 Geometry 模块

#### cn.cycle.chart.api.geometry.Coordinate (9个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_geometry_Coordinate_nativeCreate` | geometry_spatial_jni.cpp |
| 2 | jlong | nativeCreate3D | `Java_cn_cycle_chart_api_geometry_Coordinate_nativeCreate3D` | geometry_spatial_jni.cpp |
| 3 | jlong | nativeCreateM | `Java_cn_cycle_chart_api_geometry_Coordinate_nativeCreateM` | geometry_spatial_jni.cpp |
| 4 | void | nativeDestroy | `Java_cn_cycle_chart_api_geometry_Coordinate_nativeDestroy` | geometry_spatial_jni.cpp |
| 5 | jdouble | nativeGetX | `Java_cn_cycle_chart_api_geometry_Coordinate_nativeGetX` | geometry_spatial_jni.cpp |
| 6 | jdouble | nativeGetY | `Java_cn_cycle_chart_api_geometry_Coordinate_nativeGetY` | geometry_spatial_jni.cpp |
| 7 | jdouble | nativeGetZ | `Java_cn_cycle_chart_api_geometry_Coordinate_nativeGetZ` | geometry_spatial_jni.cpp |
| 8 | jdouble | nativeGetM | `Java_cn_cycle_chart_api_geometry_Coordinate_nativeGetM` | geometry_spatial_jni.cpp |
| 9 | jdouble | nativeDistance | `Java_cn_cycle_chart_api_geometry_Coordinate_nativeDistance` | geometry_spatial_jni.cpp |

#### cn.cycle.chart.api.geometry.Envelope (8个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreateFromCoords | `Java_cn_cycle_chart_api_geometry_Envelope_nativeCreateFromCoords` | geometry_spatial_jni.cpp |
| 2 | jdouble | nativeGetWidth | `Java_cn_cycle_chart_api_geometry_Envelope_nativeGetWidth` | geometry_spatial_jni.cpp |
| 3 | jdouble | nativeGetHeight | `Java_cn_cycle_chart_api_geometry_Envelope_nativeGetHeight` | geometry_spatial_jni.cpp |
| 4 | jdouble | nativeGetArea | `Java_cn_cycle_chart_api_geometry_Envelope_nativeGetArea` | geometry_spatial_jni.cpp |
| 5 | jboolean | nativeContains | `Java_cn_cycle_chart_api_geometry_Envelope_nativeContains` | geometry_spatial_jni.cpp |
| 6 | jboolean | nativeIntersects | `Java_cn_cycle_chart_api_geometry_Envelope_nativeIntersects` | geometry_spatial_jni.cpp |
| 7 | void | nativeExpand | `Java_cn_cycle_chart_api_geometry_Envelope_nativeExpand` | geometry_spatial_jni.cpp |
| 8 | jdoubleArray | nativeGetCenter | `Java_cn_cycle_chart_api_geometry_Envelope_nativeGetCenter` | geometry_spatial_jni.cpp |

#### cn.cycle.chart.api.geometry.Geometry (28个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | void | nativeDestroy | `Java_cn_cycle_chart_api_geometry_Geometry_nativeDestroy` | geometry_jni.cpp |
| 2 | jint | nativeGetType | `Java_cn_cycle_chart_api_geometry_Geometry_nativeGetType` | geometry_jni.cpp |
| 3 | jint | nativeGetDimension | `Java_cn_cycle_chart_api_geometry_Geometry_nativeGetDimension` | geometry_jni.cpp |
| 4 | jboolean | nativeIsEmpty | `Java_cn_cycle_chart_api_geometry_Geometry_nativeIsEmpty` | geometry_jni.cpp |
| 5 | jboolean | nativeIsValid | `Java_cn_cycle_chart_api_geometry_Geometry_nativeIsValid` | geometry_jni.cpp |
| 6 | jint | nativeGetSRID | `Java_cn_cycle_chart_api_geometry_Geometry_nativeGetSRID` | geometry_jni.cpp |
| 7 | void | nativeSetSRID | `Java_cn_cycle_chart_api_geometry_Geometry_nativeSetSRID` | geometry_jni.cpp |
| 8 | jstring | nativeAsText | `Java_cn_cycle_chart_api_geometry_Geometry_nativeAsText` | geometry_jni.cpp |
| 9 | jstring | nativeAsGeoJSON | `Java_cn_cycle_chart_api_geometry_Geometry_nativeAsGeoJSON` | geometry_jni.cpp |
| 10 | jdoubleArray | nativeGetEnvelope | `Java_cn_cycle_chart_api_geometry_Geometry_nativeGetEnvelope` | geometry_jni.cpp |
| 11 | jlong | nativeGetNumPoints | `Java_cn_cycle_chart_api_geometry_Geometry_nativeGetNumPoints` | geometry_jni.cpp |
| 12 | jboolean | nativeIsSimple | `Java_cn_cycle_chart_api_geometry_Geometry_nativeIsSimple` | geometry_jni.cpp |
| 13 | jboolean | nativeIs3D | `Java_cn_cycle_chart_api_geometry_Geometry_nativeIs3D` | geometry_jni.cpp |
| 14 | jboolean | nativeIsMeasured | `Java_cn_cycle_chart_api_geometry_Geometry_nativeIsMeasured` | geometry_jni.cpp |
| 15 | jboolean | nativeEquals | `Java_cn_cycle_chart_api_geometry_Geometry_nativeEquals` | geometry_spatial_jni.cpp |
| 16 | jboolean | nativeIntersects | `Java_cn_cycle_chart_api_geometry_Geometry_nativeIntersects` | geometry_spatial_jni.cpp |
| 17 | jboolean | nativeContains | `Java_cn_cycle_chart_api_geometry_Geometry_nativeContains` | geometry_spatial_jni.cpp |
| 18 | jboolean | nativeWithin | `Java_cn_cycle_chart_api_geometry_Geometry_nativeWithin` | geometry_spatial_jni.cpp |
| 19 | jboolean | nativeCrosses | `Java_cn_cycle_chart_api_geometry_Geometry_nativeCrosses` | geometry_spatial_jni.cpp |
| 20 | jboolean | nativeTouches | `Java_cn_cycle_chart_api_geometry_Geometry_nativeTouches` | geometry_spatial_jni.cpp |
| 21 | jboolean | nativeOverlaps | `Java_cn_cycle_chart_api_geometry_Geometry_nativeOverlaps` | geometry_spatial_jni.cpp |
| 22 | jdouble | nativeDistance | `Java_cn_cycle_chart_api_geometry_Geometry_nativeDistance` | geometry_spatial_jni.cpp |
| 23 | jlong | nativeIntersection | `Java_cn_cycle_chart_api_geometry_Geometry_nativeIntersection` | geometry_spatial_jni.cpp |
| 24 | jlong | nativeUnion | `Java_cn_cycle_chart_api_geometry_Geometry_nativeUnion` | geometry_spatial_jni.cpp |
| 25 | jlong | nativeDifference | `Java_cn_cycle_chart_api_geometry_Geometry_nativeDifference` | geometry_spatial_jni.cpp |
| 26 | jlong | nativeBuffer | `Java_cn_cycle_chart_api_geometry_Geometry_nativeBuffer` | geometry_spatial_jni.cpp |
| 27 | jlong | nativeClone | `Java_cn_cycle_chart_api_geometry_Geometry_nativeClone` | geometry_spatial_jni.cpp |
| 28 | jstring | nativeGetTypeName | `Java_cn_cycle_chart_api_geometry_Geometry_nativeGetTypeName` | geometry_spatial_jni.cpp |

#### cn.cycle.chart.api.geometry.GeometryCollection (4个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_geometry_GeometryCollection_nativeCreate` | geometry_ext_jni.cpp |
| 2 | jlong | nativeGetNumGeometries | `Java_cn_cycle_chart_api_geometry_GeometryCollection_nativeGetNumGeometries` | geometry_ext_jni.cpp |
| 3 | jlong | nativeGetGeometryN | `Java_cn_cycle_chart_api_geometry_GeometryCollection_nativeGetGeometryN` | geometry_ext_jni.cpp |
| 4 | void | nativeAddGeometry | `Java_cn_cycle_chart_api_geometry_GeometryCollection_nativeAddGeometry` | geometry_ext_jni.cpp |

#### cn.cycle.chart.api.geometry.GeometryFactory (8个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_geometry_GeometryFactory_nativeCreate` | geometry_ext_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_geometry_GeometryFactory_nativeDestroy` | geometry_ext_jni.cpp |
| 3 | jlong | nativeCreatePoint | `Java_cn_cycle_chart_api_geometry_GeometryFactory_nativeCreatePoint` | geometry_ext_jni.cpp |
| 4 | jlong | nativeCreateLineString | `Java_cn_cycle_chart_api_geometry_GeometryFactory_nativeCreateLineString` | geometry_ext_jni.cpp |
| 5 | jlong | nativeCreatePolygon | `Java_cn_cycle_chart_api_geometry_GeometryFactory_nativeCreatePolygon` | geometry_ext_jni.cpp |
| 6 | jlong | nativeCreateFromWkt | `Java_cn_cycle_chart_api_geometry_GeometryFactory_nativeCreateFromWkt` | geometry_ext_jni.cpp |
| 7 | jlong | nativeCreateFromWkb | `Java_cn_cycle_chart_api_geometry_GeometryFactory_nativeCreateFromWkb` | geometry_ext_jni.cpp |
| 8 | jlong | nativeCreateFromGeoJson | `Java_cn_cycle_chart_api_geometry_GeometryFactory_nativeCreateFromGeoJson` | geometry_ext_jni.cpp |

#### cn.cycle.chart.api.geometry.LineString (9个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_geometry_LineString_nativeCreate` | geometry_jni.cpp |
| 2 | jlong | nativeGetNumPoints | `Java_cn_cycle_chart_api_geometry_LineString_nativeGetNumPoints` | geometry_jni.cpp |
| 3 | jdoubleArray | nativeGetPointN | `Java_cn_cycle_chart_api_geometry_LineString_nativeGetPointN` | geometry_jni.cpp |
| 4 | jdouble | nativeGetLength | `Java_cn_cycle_chart_api_geometry_LineString_nativeGetLength` | geometry_jni.cpp |
| 5 | void | nativeAddPoint | `Java_cn_cycle_chart_api_geometry_LineString_nativeAddPoint` | geometry_jni.cpp |
| 6 | jlong | nativeCreateFromCoords | `Java_cn_cycle_chart_api_geometry_LineString_nativeCreateFromCoords` | geometry_spatial_jni.cpp |
| 7 | void | nativeAddPoint3D | `Java_cn_cycle_chart_api_geometry_LineString_nativeAddPoint3D` | geometry_spatial_jni.cpp |
| 8 | void | nativeSetPointN | `Java_cn_cycle_chart_api_geometry_LineString_nativeSetPointN` | geometry_spatial_jni.cpp |
| 9 | jlong | nativeGetPointGeometry | `Java_cn_cycle_chart_api_geometry_LineString_nativeGetPointGeometry` | geometry_spatial_jni.cpp |

#### cn.cycle.chart.api.geometry.LinearRing (3个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_geometry_LinearRing_nativeCreate` | geometry_jni.cpp |
| 2 | jboolean | nativeIsClosed | `Java_cn_cycle_chart_api_geometry_LinearRing_nativeIsClosed` | geometry_jni.cpp |
| 3 | jlong | nativeCreateFromCoords | `Java_cn_cycle_chart_api_geometry_LinearRing_nativeCreateFromCoords` | geometry_spatial_jni.cpp |

#### cn.cycle.chart.api.geometry.MultiLineString (4个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_geometry_MultiLineString_nativeCreate` | geometry_ext_jni.cpp |
| 2 | jlong | nativeGetNumGeometries | `Java_cn_cycle_chart_api_geometry_MultiLineString_nativeGetNumGeometries` | geometry_ext_jni.cpp |
| 3 | jlong | nativeGetGeometryN | `Java_cn_cycle_chart_api_geometry_MultiLineString_nativeGetGeometryN` | geometry_ext_jni.cpp |
| 4 | void | nativeAddGeometry | `Java_cn_cycle_chart_api_geometry_MultiLineString_nativeAddGeometry` | geometry_ext_jni.cpp |

#### cn.cycle.chart.api.geometry.MultiPoint (4个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_geometry_MultiPoint_nativeCreate` | geometry_ext_jni.cpp |
| 2 | jlong | nativeGetNumGeometries | `Java_cn_cycle_chart_api_geometry_MultiPoint_nativeGetNumGeometries` | geometry_ext_jni.cpp |
| 3 | jlong | nativeGetGeometryN | `Java_cn_cycle_chart_api_geometry_MultiPoint_nativeGetGeometryN` | geometry_ext_jni.cpp |
| 4 | void | nativeAddGeometry | `Java_cn_cycle_chart_api_geometry_MultiPoint_nativeAddGeometry` | geometry_ext_jni.cpp |

#### cn.cycle.chart.api.geometry.MultiPolygon (4个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_geometry_MultiPolygon_nativeCreate` | geometry_ext_jni.cpp |
| 2 | jlong | nativeGetNumGeometries | `Java_cn_cycle_chart_api_geometry_MultiPolygon_nativeGetNumGeometries` | geometry_ext_jni.cpp |
| 3 | jlong | nativeGetGeometryN | `Java_cn_cycle_chart_api_geometry_MultiPolygon_nativeGetGeometryN` | geometry_ext_jni.cpp |
| 4 | void | nativeAddGeometry | `Java_cn_cycle_chart_api_geometry_MultiPolygon_nativeAddGeometry` | geometry_ext_jni.cpp |

#### cn.cycle.chart.api.geometry.Point (12个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_geometry_Point_nativeCreate` | geometry_jni.cpp |
| 2 | jlong | nativeCreate3D | `Java_cn_cycle_chart_api_geometry_Point_nativeCreate3D` | geometry_jni.cpp |
| 3 | jdouble | nativeGetX | `Java_cn_cycle_chart_api_geometry_Point_nativeGetX` | geometry_jni.cpp |
| 4 | jdouble | nativeGetY | `Java_cn_cycle_chart_api_geometry_Point_nativeGetY` | geometry_jni.cpp |
| 5 | jdouble | nativeGetZ | `Java_cn_cycle_chart_api_geometry_Point_nativeGetZ` | geometry_jni.cpp |
| 6 | void | nativeSetX | `Java_cn_cycle_chart_api_geometry_Point_nativeSetX` | geometry_jni.cpp |
| 7 | void | nativeSetY | `Java_cn_cycle_chart_api_geometry_Point_nativeSetY` | geometry_jni.cpp |
| 8 | void | nativeSetZ | `Java_cn_cycle_chart_api_geometry_Point_nativeSetZ` | geometry_jni.cpp |
| 9 | jlong | nativeCreateFromCoord | `Java_cn_cycle_chart_api_geometry_Point_nativeCreateFromCoord` | geometry_spatial_jni.cpp |
| 10 | jdouble | nativeGetM | `Java_cn_cycle_chart_api_geometry_Point_nativeGetM` | geometry_spatial_jni.cpp |
| 11 | void | nativeSetM | `Java_cn_cycle_chart_api_geometry_Point_nativeSetM` | geometry_spatial_jni.cpp |
| 12 | jdoubleArray | nativeGetCoordinate | `Java_cn_cycle_chart_api_geometry_Point_nativeGetCoordinate` | geometry_spatial_jni.cpp |

#### cn.cycle.chart.api.geometry.Polygon (10个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_geometry_Polygon_nativeCreate` | geometry_jni.cpp |
| 2 | jlong | nativeGetNumInteriorRings | `Java_cn_cycle_chart_api_geometry_Polygon_nativeGetNumInteriorRings` | geometry_jni.cpp |
| 3 | jlong | nativeGetExteriorRing | `Java_cn_cycle_chart_api_geometry_Polygon_nativeGetExteriorRing` | geometry_jni.cpp |
| 4 | jlong | nativeGetInteriorRingN | `Java_cn_cycle_chart_api_geometry_Polygon_nativeGetInteriorRingN` | geometry_jni.cpp |
| 5 | void | nativeAddInteriorRing | `Java_cn_cycle_chart_api_geometry_Polygon_nativeAddInteriorRing` | geometry_jni.cpp |
| 6 | jdouble | nativeGetArea | `Java_cn_cycle_chart_api_geometry_Polygon_nativeGetArea` | geometry_jni.cpp |
| 7 | jdoubleArray | nativeGetCentroid | `Java_cn_cycle_chart_api_geometry_Polygon_nativeGetCentroid` | geometry_jni.cpp |
| 8 | jlong | nativeCreateFromRing | `Java_cn_cycle_chart_api_geometry_Polygon_nativeCreateFromRing` | geometry_spatial_jni.cpp |
| 9 | jlong | nativeCreateFromCoords | `Java_cn_cycle_chart_api_geometry_Polygon_nativeCreateFromCoords` | geometry_spatial_jni.cpp |
| 10 | jboolean | nativeIsValid | `Java_cn_cycle_chart_api_geometry_Polygon_nativeIsValid` | geometry_spatial_jni.cpp |

### 2.13 Graph 模块

#### cn.cycle.chart.api.graph.HitTest (5个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_graph_HitTest_nativeCreate` | graph_ext_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_graph_HitTest_nativeDestroy` | graph_ext_jni.cpp |
| 3 | jlong | nativeQueryPoint | `Java_cn_cycle_chart_api_graph_HitTest_nativeQueryPoint` | graph_ext_jni.cpp |
| 4 | jlongArray | nativeQueryRect | `Java_cn_cycle_chart_api_graph_HitTest_nativeQueryRect` | graph_ext_jni.cpp |
| 5 | void | nativeFreeFeatures | `Java_cn_cycle_chart_api_graph_HitTest_nativeFreeFeatures` | graph_ext_jni.cpp |

#### cn.cycle.chart.api.graph.LODManager (6个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_graph_LODManager_nativeCreate` | graph_ext_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_graph_LODManager_nativeDestroy` | graph_ext_jni.cpp |
| 3 | jint | nativeGetCurrentLevel | `Java_cn_cycle_chart_api_graph_LODManager_nativeGetCurrentLevel` | graph_ext_jni.cpp |
| 4 | void | nativeSetCurrentLevel | `Java_cn_cycle_chart_api_graph_LODManager_nativeSetCurrentLevel` | graph_ext_jni.cpp |
| 5 | jint | nativeGetLevelCount | `Java_cn_cycle_chart_api_graph_LODManager_nativeGetLevelCount` | graph_ext_jni.cpp |
| 6 | jdouble | nativeGetScaleForLevel | `Java_cn_cycle_chart_api_graph_LODManager_nativeGetScaleForLevel` | graph_ext_jni.cpp |

#### cn.cycle.chart.api.graph.LabelEngine (6个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_graph_LabelEngine_nativeCreate` | graph_ext_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_graph_LabelEngine_nativeDestroy` | graph_ext_jni.cpp |
| 3 | void | nativeSetMaxLabels | `Java_cn_cycle_chart_api_graph_LabelEngine_nativeSetMaxLabels` | graph_ext_jni.cpp |
| 4 | jint | nativeGetMaxLabels | `Java_cn_cycle_chart_api_graph_LabelEngine_nativeGetMaxLabels` | graph_ext_jni.cpp |
| 5 | void | nativeSetCollisionDetection | `Java_cn_cycle_chart_api_graph_LabelEngine_nativeSetCollisionDetection` | graph_ext_jni.cpp |
| 6 | void | nativeClear | `Java_cn_cycle_chart_api_graph_LabelEngine_nativeClear` | graph_ext_jni.cpp |

#### cn.cycle.chart.api.graph.LabelInfo (7个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_graph_LabelInfo_nativeCreate` | graph_ext_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_graph_LabelInfo_nativeDestroy` | graph_ext_jni.cpp |
| 3 | jstring | nativeGetText | `Java_cn_cycle_chart_api_graph_LabelInfo_nativeGetText` | graph_ext_jni.cpp |
| 4 | jdouble | nativeGetX | `Java_cn_cycle_chart_api_graph_LabelInfo_nativeGetX` | graph_ext_jni.cpp |
| 5 | jdouble | nativeGetY | `Java_cn_cycle_chart_api_graph_LabelInfo_nativeGetY` | graph_ext_jni.cpp |
| 6 | void | nativeSetText | `Java_cn_cycle_chart_api_graph_LabelInfo_nativeSetText` | graph_ext_jni.cpp |
| 7 | void | nativeSetPosition | `Java_cn_cycle_chart_api_graph_LabelInfo_nativeSetPosition` | graph_ext_jni.cpp |

#### cn.cycle.chart.api.graph.RenderOptimizer (5个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_graph_RenderOptimizer_nativeCreate` | render_perf_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_graph_RenderOptimizer_nativeDestroy` | render_perf_jni.cpp |
| 3 | void | nativeSetCacheEnabled | `Java_cn_cycle_chart_api_graph_RenderOptimizer_nativeSetCacheEnabled` | render_perf_jni.cpp |
| 4 | jboolean | nativeIsCacheEnabled | `Java_cn_cycle_chart_api_graph_RenderOptimizer_nativeIsCacheEnabled` | render_perf_jni.cpp |
| 5 | void | nativeClearCache | `Java_cn_cycle_chart_api_graph_RenderOptimizer_nativeClearCache` | render_perf_jni.cpp |

#### cn.cycle.chart.api.graph.RenderQueue (6个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_graph_RenderQueue_nativeCreate` | graph_ext_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_graph_RenderQueue_nativeDestroy` | graph_ext_jni.cpp |
| 3 | void | nativePush | `Java_cn_cycle_chart_api_graph_RenderQueue_nativePush` | graph_ext_jni.cpp |
| 4 | jlong | nativePop | `Java_cn_cycle_chart_api_graph_RenderQueue_nativePop` | graph_ext_jni.cpp |
| 5 | jlong | nativeGetSize | `Java_cn_cycle_chart_api_graph_RenderQueue_nativeGetSize` | graph_ext_jni.cpp |
| 6 | void | nativeClear | `Java_cn_cycle_chart_api_graph_RenderQueue_nativeClear` | graph_ext_jni.cpp |

#### cn.cycle.chart.api.graph.RenderTask (5个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_graph_RenderTask_nativeCreate` | graph_ext_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_graph_RenderTask_nativeDestroy` | graph_ext_jni.cpp |
| 3 | jboolean | nativeExecute | `Java_cn_cycle_chart_api_graph_RenderTask_nativeExecute` | graph_ext_jni.cpp |
| 4 | jboolean | nativeIsComplete | `Java_cn_cycle_chart_api_graph_RenderTask_nativeIsComplete` | graph_ext_jni.cpp |
| 5 | void | nativeCancel | `Java_cn_cycle_chart_api_graph_RenderTask_nativeCancel` | graph_ext_jni.cpp |

#### cn.cycle.chart.api.graph.TransformManager (4个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_graph_TransformManager_nativeCreate` | graph_ext_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_graph_TransformManager_nativeDestroy` | graph_ext_jni.cpp |
| 3 | void | nativeSetViewport | `Java_cn_cycle_chart_api_graph_TransformManager_nativeSetViewport` | graph_ext_jni.cpp |
| 4 | jdoubleArray | nativeGetMatrix | `Java_cn_cycle_chart_api_graph_TransformManager_nativeGetMatrix` | graph_ext_jni.cpp |

### 2.14 Health 模块

#### cn.cycle.chart.api.health.HealthCheck (8个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_health_HealthCheck_nativeCreate` | health_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_health_HealthCheck_nativeDestroy` | health_jni.cpp |
| 3 | jlong | nativeExecute | `Java_cn_cycle_chart_api_health_HealthCheck_nativeExecute` | health_jni.cpp |
| 4 | void | nativeDestroyResult | `Java_cn_cycle_chart_api_health_HealthCheck_nativeDestroyResult` | health_jni.cpp |
| 5 | jint | nativeGetResultStatus | `Java_cn_cycle_chart_api_health_HealthCheck_nativeGetResultStatus` | health_jni.cpp |
| 6 | jstring | nativeGetResultComponentName | `Java_cn_cycle_chart_api_health_HealthCheck_nativeGetResultComponentName` | health_jni.cpp |
| 7 | jstring | nativeGetResultMessage | `Java_cn_cycle_chart_api_health_HealthCheck_nativeGetResultMessage` | health_jni.cpp |
| 8 | jdouble | nativeGetResultDuration | `Java_cn_cycle_chart_api_health_HealthCheck_nativeGetResultDuration` | health_jni.cpp |

### 2.15 Layer 模块

#### cn.cycle.chart.api.layer.DataSource (9个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeOpen | `Java_cn_cycle_chart_api_layer_DataSource_nativeOpen` | datasource_jni.cpp |
| 2 | void | nativeClose | `Java_cn_cycle_chart_api_layer_DataSource_nativeClose` | datasource_jni.cpp |
| 3 | jboolean | nativeIsOpen | `Java_cn_cycle_chart_api_layer_DataSource_nativeIsOpen` | datasource_jni.cpp |
| 4 | jstring | nativeGetPath | `Java_cn_cycle_chart_api_layer_DataSource_nativeGetPath` | datasource_jni.cpp |
| 5 | jlong | nativeGetLayerCount | `Java_cn_cycle_chart_api_layer_DataSource_nativeGetLayerCount` | datasource_jni.cpp |
| 6 | jlong | nativeGetLayer | `Java_cn_cycle_chart_api_layer_DataSource_nativeGetLayer` | datasource_jni.cpp |
| 7 | jlong | nativeGetLayerByName | `Java_cn_cycle_chart_api_layer_DataSource_nativeGetLayerByName` | datasource_jni.cpp |
| 8 | jlong | nativeCreateLayer | `Java_cn_cycle_chart_api_layer_DataSource_nativeCreateLayer` | datasource_jni.cpp |
| 9 | jint | nativeDeleteLayer | `Java_cn_cycle_chart_api_layer_DataSource_nativeDeleteLayer` | datasource_jni.cpp |

#### cn.cycle.chart.api.layer.DriverManager (6个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeGetInstance | `Java_cn_cycle_chart_api_layer_DriverManager_nativeGetInstance` | datasource_jni.cpp |
| 2 | jint | nativeRegisterDriver | `Java_cn_cycle_chart_api_layer_DriverManager_nativeRegisterDriver` | datasource_jni.cpp |
| 3 | jint | nativeUnregisterDriver | `Java_cn_cycle_chart_api_layer_DriverManager_nativeUnregisterDriver` | datasource_jni.cpp |
| 4 | jlong | nativeGetDriver | `Java_cn_cycle_chart_api_layer_DriverManager_nativeGetDriver` | datasource_jni.cpp |
| 5 | jint | nativeGetDriverCount | `Java_cn_cycle_chart_api_layer_DriverManager_nativeGetDriverCount` | datasource_jni.cpp |
| 6 | jstring | nativeGetDriverName | `Java_cn_cycle_chart_api_layer_DriverManager_nativeGetDriverName` | datasource_jni.cpp |

#### cn.cycle.chart.api.layer.Layer (11个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeGetFeatureDefn | `Java_cn_cycle_chart_api_layer_Layer_nativeGetFeatureDefn` | feature_layer_ext_jni.cpp |
| 2 | jint | nativeGetGeomType | `Java_cn_cycle_chart_api_layer_Layer_nativeGetGeomType` | feature_layer_ext_jni.cpp |
| 3 | jlong | nativeGetFeatureCount | `Java_cn_cycle_chart_api_layer_Layer_nativeGetFeatureCount` | feature_layer_ext_jni.cpp |
| 4 | void | nativeResetReading | `Java_cn_cycle_chart_api_layer_Layer_nativeResetReading` | feature_layer_ext_jni.cpp |
| 5 | jlong | nativeGetNextFeature | `Java_cn_cycle_chart_api_layer_Layer_nativeGetNextFeature` | feature_layer_ext_jni.cpp |
| 6 | jlong | nativeGetFeature | `Java_cn_cycle_chart_api_layer_Layer_nativeGetFeature` | feature_layer_ext_jni.cpp |
| 7 | void | nativeSetAttributeFilter | `Java_cn_cycle_chart_api_layer_Layer_nativeSetAttributeFilter` | feature_layer_ext_jni.cpp |
| 8 | void | nativeDestroy | `Java_cn_cycle_chart_api_layer_Layer_nativeDestroy` | layer_jni.cpp |
| 9 | jstring | nativeGetName | `Java_cn_cycle_chart_api_layer_Layer_nativeGetName` | layer_jni.cpp |
| 10 | jint | nativeGetType | `Java_cn_cycle_chart_api_layer_Layer_nativeGetType` | layer_jni.cpp |
| 11 | void | nativeArrayDestroy | `Java_cn_cycle_chart_api_layer_Layer_nativeArrayDestroy` | layer_jni.cpp |

#### cn.cycle.chart.api.layer.LayerGroup (14个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_layer_LayerGroup_nativeCreate` | feature_layer_ext_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_layer_LayerGroup_nativeDestroy` | feature_layer_ext_jni.cpp |
| 3 | jstring | nativeGetName | `Java_cn_cycle_chart_api_layer_LayerGroup_nativeGetName` | feature_layer_ext_jni.cpp |
| 4 | jlong | nativeGetLayerCount | `Java_cn_cycle_chart_api_layer_LayerGroup_nativeGetLayerCount` | feature_layer_ext_jni.cpp |
| 5 | jlong | nativeGetLayer | `Java_cn_cycle_chart_api_layer_LayerGroup_nativeGetLayer` | feature_layer_ext_jni.cpp |
| 6 | void | nativeAddLayer | `Java_cn_cycle_chart_api_layer_LayerGroup_nativeAddLayer` | feature_layer_ext_jni.cpp |
| 7 | void | nativeRemoveLayer | `Java_cn_cycle_chart_api_layer_LayerGroup_nativeRemoveLayer` | feature_layer_ext_jni.cpp |
| 8 | void | nativeMoveLayer | `Java_cn_cycle_chart_api_layer_LayerGroup_nativeMoveLayer` | feature_layer_ext_jni.cpp |
| 9 | jboolean | nativeIsVisible | `Java_cn_cycle_chart_api_layer_LayerGroup_nativeIsVisible` | feature_layer_ext_jni.cpp |
| 10 | void | nativeSetVisible | `Java_cn_cycle_chart_api_layer_LayerGroup_nativeSetVisible` | feature_layer_ext_jni.cpp |
| 11 | jdouble | nativeGetOpacity | `Java_cn_cycle_chart_api_layer_LayerGroup_nativeGetOpacity` | feature_layer_ext_jni.cpp |
| 12 | void | nativeSetOpacity | `Java_cn_cycle_chart_api_layer_LayerGroup_nativeSetOpacity` | feature_layer_ext_jni.cpp |
| 13 | jint | nativeGetZOrder | `Java_cn_cycle_chart_api_layer_LayerGroup_nativeGetZOrder` | feature_layer_ext_jni.cpp |
| 14 | void | nativeSetZOrder | `Java_cn_cycle_chart_api_layer_LayerGroup_nativeSetZOrder` | feature_layer_ext_jni.cpp |

#### cn.cycle.chart.api.layer.LayerManager (16个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_layer_LayerManager_nativeCreate` | layer_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_layer_LayerManager_nativeDestroy` | layer_jni.cpp |
| 3 | jlong | nativeGetLayerCount | `Java_cn_cycle_chart_api_layer_LayerManager_nativeGetLayerCount` | layer_jni.cpp |
| 4 | jlong | nativeGetLayer | `Java_cn_cycle_chart_api_layer_LayerManager_nativeGetLayer` | layer_jni.cpp |
| 5 | jint | nativeGetLayerType | `Java_cn_cycle_chart_api_layer_LayerManager_nativeGetLayerType` | layer_jni.cpp |
| 6 | void | nativeAddLayer | `Java_cn_cycle_chart_api_layer_LayerManager_nativeAddLayer` | layer_jni.cpp |
| 7 | void | nativeRemoveLayer | `Java_cn_cycle_chart_api_layer_LayerManager_nativeRemoveLayer` | layer_jni.cpp |
| 8 | void | nativeMoveLayer | `Java_cn_cycle_chart_api_layer_LayerManager_nativeMoveLayer` | layer_jni.cpp |
| 9 | jboolean | nativeGetLayerVisible | `Java_cn_cycle_chart_api_layer_LayerManager_nativeGetLayerVisible` | layer_jni.cpp |
| 10 | void | nativeSetLayerVisible | `Java_cn_cycle_chart_api_layer_LayerManager_nativeSetLayerVisible` | layer_jni.cpp |
| 11 | jdouble | nativeGetLayerOpacity | `Java_cn_cycle_chart_api_layer_LayerManager_nativeGetLayerOpacity` | layer_jni.cpp |
| 12 | void | nativeSetLayerOpacity | `Java_cn_cycle_chart_api_layer_LayerManager_nativeSetLayerOpacity` | layer_jni.cpp |
| 13 | jlong | nativeGetLayerByName | `Java_cn_cycle_chart_api_layer_LayerManager_nativeGetLayerByName` | layer_jni.cpp |
| 14 | jint | nativeGetLayerZOrder | `Java_cn_cycle_chart_api_layer_LayerManager_nativeGetLayerZOrder` | layer_jni.cpp |
| 15 | void | nativeSetLayerZOrder | `Java_cn_cycle_chart_api_layer_LayerManager_nativeSetLayerZOrder` | layer_jni.cpp |
| 16 | void | nativeSortByZOrder | `Java_cn_cycle_chart_api_layer_LayerManager_nativeSortByZOrder` | layer_jni.cpp |

#### cn.cycle.chart.api.layer.MemoryLayer (6个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_layer_MemoryLayer_nativeCreate` | feature_layer_ext_jni.cpp |
| 2 | jboolean | nativeAddFeature | `Java_cn_cycle_chart_api_layer_MemoryLayer_nativeAddFeature` | feature_layer_ext_jni.cpp |
| 3 | jboolean | nativeRemoveFeature | `Java_cn_cycle_chart_api_layer_MemoryLayer_nativeRemoveFeature` | feature_layer_ext_jni.cpp |
| 4 | void | nativeClear | `Java_cn_cycle_chart_api_layer_MemoryLayer_nativeClear` | feature_layer_ext_jni.cpp |
| 5 | jlong | nativeGetFeatureCount | `Java_cn_cycle_chart_api_layer_MemoryLayer_nativeGetFeatureCount` | feature_layer_ext_jni.cpp |
| 6 | jlong | nativeCreateFromFeatures | `Java_cn_cycle_chart_api_layer_MemoryLayer_nativeCreateFromFeatures` | layer_jni.cpp |

#### cn.cycle.chart.api.layer.RasterLayer (6个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_layer_RasterLayer_nativeCreate` | feature_layer_ext_jni.cpp |
| 2 | jint | nativeGetWidth | `Java_cn_cycle_chart_api_layer_RasterLayer_nativeGetWidth` | feature_layer_ext_jni.cpp |
| 3 | jint | nativeGetHeight | `Java_cn_cycle_chart_api_layer_RasterLayer_nativeGetHeight` | feature_layer_ext_jni.cpp |
| 4 | jint | nativeGetBandCount | `Java_cn_cycle_chart_api_layer_RasterLayer_nativeGetBandCount` | feature_layer_ext_jni.cpp |
| 5 | jdouble | nativeGetNoDataValue | `Java_cn_cycle_chart_api_layer_RasterLayer_nativeGetNoDataValue` | feature_layer_ext_jni.cpp |
| 6 | jlong | nativeGetExtent | `Java_cn_cycle_chart_api_layer_RasterLayer_nativeGetExtent` | feature_layer_ext_jni.cpp |

#### cn.cycle.chart.api.layer.VectorLayer (20个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeCreate` | feature_layer_ext_jni.cpp |
| 2 | jlong | nativeCreateFromDatasource | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeCreateFromDatasource` | feature_layer_ext_jni.cpp |
| 3 | jlong | nativeGetFeatureById | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetFeatureById` | feature_layer_ext_jni.cpp |
| 4 | void | nativeSetSpatialFilter | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeSetSpatialFilter` | feature_layer_ext_jni.cpp |
| 5 | jlong | nativeGetFeatureDefn | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetFeatureDefn` | feature_layer_ext_jni.cpp |
| 6 | jlong | nativeGetSpatialFilter | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetSpatialFilter` | feature_layer_ext_jni.cpp |
| 7 | jstring | nativeGetAttributeFilter | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetAttributeFilter` | feature_layer_ext_jni.cpp |
| 8 | jboolean | nativeUpdateFeature | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeUpdateFeature` | feature_layer_ext_jni.cpp |
| 9 | jboolean | nativeDeleteFeature | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeDeleteFeature` | feature_layer_ext_jni.cpp |
| 10 | jboolean | nativeCreateFeature | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeCreateFeature` | feature_layer_ext_jni.cpp |
| 11 | jboolean | nativeSetStyle | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeSetStyle` | feature_layer_ext_jni.cpp |
| 12 | jlong | nativeGetFeatureCount | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetFeatureCount` | layer_jni.cpp |
| 13 | jlong | nativeGetFeature | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetFeature` | layer_jni.cpp |
| 14 | jlong | nativeGetNextFeature | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetNextFeature` | layer_jni.cpp |
| 15 | void | nativeSetSpatialFilterRect | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeSetSpatialFilterRect` | layer_jni.cpp |
| 16 | jboolean | nativeGetExtent | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetExtent` | layer_jni.cpp |
| 17 | void | nativeResetReading | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeResetReading` | layer_jni.cpp |
| 18 | jint | nativeAddFeature | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeAddFeature` | layer_jni.cpp |
| 19 | jlong | nativeCreateFromFeatures | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeCreateFromFeatures` | layer_jni.cpp |
| 20 | jlong | nativeGetExtentPtr | `Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetExtentPtr` | layer_jni.cpp |

### 2.16 Loader 模块

#### cn.cycle.chart.api.loader.LibraryLoader (6个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_loader_LibraryLoader_nativeCreate` | loader_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_loader_LibraryLoader_nativeDestroy` | loader_jni.cpp |
| 3 | jlong | nativeLoad | `Java_cn_cycle_chart_api_loader_LibraryLoader_nativeLoad` | loader_jni.cpp |
| 4 | void | nativeUnload | `Java_cn_cycle_chart_api_loader_LibraryLoader_nativeUnload` | loader_jni.cpp |
| 5 | jlong | nativeGetSymbol | `Java_cn_cycle_chart_api_loader_LibraryLoader_nativeGetSymbol` | loader_jni.cpp |
| 6 | jstring | nativeGetError | `Java_cn_cycle_chart_api_loader_LibraryLoader_nativeGetError` | loader_jni.cpp |

#### cn.cycle.chart.api.loader.SecureLibraryLoader (4个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_loader_SecureLibraryLoader_nativeCreate` | secure_loader_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_loader_SecureLibraryLoader_nativeDestroy` | secure_loader_jni.cpp |
| 3 | jlong | nativeLoad | `Java_cn_cycle_chart_api_loader_SecureLibraryLoader_nativeLoad` | secure_loader_jni.cpp |
| 4 | jint | nativeVerifySignature | `Java_cn_cycle_chart_api_loader_SecureLibraryLoader_nativeVerifySignature` | secure_loader_jni.cpp |

### 2.17 Navi 模块

#### cn.cycle.chart.api.navi.AisManager (11个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_navi_AisManager_nativeCreate` | ais_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_navi_AisManager_nativeDestroy` | ais_jni.cpp |
| 3 | jint | nativeInitialize | `Java_cn_cycle_chart_api_navi_AisManager_nativeInitialize` | ais_jni.cpp |
| 4 | void | nativeShutdown | `Java_cn_cycle_chart_api_navi_AisManager_nativeShutdown` | ais_jni.cpp |
| 5 | jlong | nativeGetTarget | `Java_cn_cycle_chart_api_navi_AisManager_nativeGetTarget` | ais_jni.cpp |
| 6 | jintArray | nativeGetAllTargetMmsi | `Java_cn_cycle_chart_api_navi_AisManager_nativeGetAllTargetMmsi` | ais_jni.cpp |
| 7 | jint | nativeGetTargetCount | `Java_cn_cycle_chart_api_navi_AisManager_nativeGetTargetCount` | ais_jni.cpp |
| 8 | jint | nativeAddTarget | `Java_cn_cycle_chart_api_navi_AisManager_nativeAddTarget` | ais_jni.cpp |
| 9 | jint | nativeRemoveTarget | `Java_cn_cycle_chart_api_navi_AisManager_nativeRemoveTarget` | ais_jni.cpp |
| 10 | void | nativeSetCallback | `Java_cn_cycle_chart_api_navi_AisManager_nativeSetCallback` | ais_jni.cpp |
| 11 | void | nativeFreeTargets | `Java_cn_cycle_chart_api_navi_AisManager_nativeFreeTargets` | ais_jni.cpp |

#### cn.cycle.chart.api.navi.AisTarget (12个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_navi_AisTarget_nativeCreate` | ais_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_navi_AisTarget_nativeDestroy` | ais_jni.cpp |
| 3 | jint | nativeGetMmsi | `Java_cn_cycle_chart_api_navi_AisTarget_nativeGetMmsi` | ais_jni.cpp |
| 4 | jstring | nativeGetName | `Java_cn_cycle_chart_api_navi_AisTarget_nativeGetName` | ais_jni.cpp |
| 5 | jdouble | nativeGetLatitude | `Java_cn_cycle_chart_api_navi_AisTarget_nativeGetLatitude` | ais_jni.cpp |
| 6 | jdouble | nativeGetLongitude | `Java_cn_cycle_chart_api_navi_AisTarget_nativeGetLongitude` | ais_jni.cpp |
| 7 | jdouble | nativeGetSpeed | `Java_cn_cycle_chart_api_navi_AisTarget_nativeGetSpeed` | ais_jni.cpp |
| 8 | jdouble | nativeGetCourse | `Java_cn_cycle_chart_api_navi_AisTarget_nativeGetCourse` | ais_jni.cpp |
| 9 | jdouble | nativeGetHeading | `Java_cn_cycle_chart_api_navi_AisTarget_nativeGetHeading` | ais_jni.cpp |
| 10 | jint | nativeGetNavStatus | `Java_cn_cycle_chart_api_navi_AisTarget_nativeGetNavStatus` | ais_jni.cpp |
| 11 | void | nativeUpdatePosition | `Java_cn_cycle_chart_api_navi_AisTarget_nativeUpdatePosition` | ais_jni.cpp |
| 12 | void | nativeArrayDestroy | `Java_cn_cycle_chart_api_navi_AisTarget_nativeArrayDestroy` | ais_jni.cpp |

#### cn.cycle.chart.api.navi.CollisionAssessor (7个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_navi_CollisionAssessor_nativeCreate` | collision_deviation_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_navi_CollisionAssessor_nativeDestroy` | collision_deviation_jni.cpp |
| 3 | jlong | nativeAssess | `Java_cn_cycle_chart_api_navi_CollisionAssessor_nativeAssess` | collision_deviation_jni.cpp |
| 4 | void | nativeDestroyRisk | `Java_cn_cycle_chart_api_navi_CollisionAssessor_nativeDestroyRisk` | collision_deviation_jni.cpp |
| 5 | jdouble | nativeGetRiskCpa | `Java_cn_cycle_chart_api_navi_CollisionAssessor_nativeGetRiskCpa` | collision_deviation_jni.cpp |
| 6 | jdouble | nativeGetRiskTcpa | `Java_cn_cycle_chart_api_navi_CollisionAssessor_nativeGetRiskTcpa` | collision_deviation_jni.cpp |
| 7 | jint | nativeGetRiskLevel | `Java_cn_cycle_chart_api_navi_CollisionAssessor_nativeGetRiskLevel` | collision_deviation_jni.cpp |

#### cn.cycle.chart.api.navi.NavigationEngine (15个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeCreate` | navigation_engine_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeDestroy` | navigation_engine_jni.cpp |
| 3 | jint | nativeInitialize | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeInitialize` | navigation_engine_jni.cpp |
| 4 | void | nativeShutdown | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeShutdown` | navigation_engine_jni.cpp |
| 5 | void | nativeSetRoute | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeSetRoute` | navigation_engine_jni.cpp |
| 6 | jlong | nativeGetRoute | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeGetRoute` | navigation_engine_jni.cpp |
| 7 | void | nativeStart | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeStart` | navigation_engine_jni.cpp |
| 8 | void | nativeStop | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeStop` | navigation_engine_jni.cpp |
| 9 | void | nativePause | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativePause` | navigation_engine_jni.cpp |
| 10 | void | nativeResume | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeResume` | navigation_engine_jni.cpp |
| 11 | jint | nativeGetStatus | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeGetStatus` | navigation_engine_jni.cpp |
| 12 | jlong | nativeGetCurrentWaypoint | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeGetCurrentWaypoint` | navigation_engine_jni.cpp |
| 13 | jdouble | nativeGetDistanceToWaypoint | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeGetDistanceToWaypoint` | navigation_engine_jni.cpp |
| 14 | jdouble | nativeGetBearingToWaypoint | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeGetBearingToWaypoint` | navigation_engine_jni.cpp |
| 15 | jdouble | nativeGetCrossTrackError | `Java_cn_cycle_chart_api_navi_NavigationEngine_nativeGetCrossTrackError` | navigation_engine_jni.cpp |

#### cn.cycle.chart.api.navi.NmeaParser (4个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_navi_NmeaParser_nativeCreate` | nmea_position_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_navi_NmeaParser_nativeDestroy` | nmea_position_jni.cpp |
| 3 | jint | nativeParse | `Java_cn_cycle_chart_api_navi_NmeaParser_nativeParse` | nmea_position_jni.cpp |
| 4 | jstring | nativeGetSentenceType | `Java_cn_cycle_chart_api_navi_NmeaParser_nativeGetSentenceType` | nmea_position_jni.cpp |

#### cn.cycle.chart.api.navi.OffCourseDetector (5个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_navi_OffCourseDetector_nativeCreate` | collision_deviation_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_navi_OffCourseDetector_nativeDestroy` | collision_deviation_jni.cpp |
| 3 | void | nativeSetThreshold | `Java_cn_cycle_chart_api_navi_OffCourseDetector_nativeSetThreshold` | collision_deviation_jni.cpp |
| 4 | jboolean | nativeCheck | `Java_cn_cycle_chart_api_navi_OffCourseDetector_nativeCheck` | collision_deviation_jni.cpp |
| 5 | jdouble | nativeGetCrossTrackError | `Java_cn_cycle_chart_api_navi_OffCourseDetector_nativeGetCrossTrackError` | collision_deviation_jni.cpp |

#### cn.cycle.chart.api.navi.PositionManager (14个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_navi_PositionManager_nativeCreate` | nmea_position_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_navi_PositionManager_nativeDestroy` | nmea_position_jni.cpp |
| 3 | jint | nativeInitialize | `Java_cn_cycle_chart_api_navi_PositionManager_nativeInitialize` | nmea_position_jni.cpp |
| 4 | void | nativeShutdown | `Java_cn_cycle_chart_api_navi_PositionManager_nativeShutdown` | nmea_position_jni.cpp |
| 5 | jdouble | nativeGetLatitude | `Java_cn_cycle_chart_api_navi_PositionManager_nativeGetLatitude` | nmea_position_jni.cpp |
| 6 | jdouble | nativeGetLongitude | `Java_cn_cycle_chart_api_navi_PositionManager_nativeGetLongitude` | nmea_position_jni.cpp |
| 7 | jdouble | nativeGetSpeed | `Java_cn_cycle_chart_api_navi_PositionManager_nativeGetSpeed` | nmea_position_jni.cpp |
| 8 | jdouble | nativeGetCourse | `Java_cn_cycle_chart_api_navi_PositionManager_nativeGetCourse` | nmea_position_jni.cpp |
| 9 | jdouble | nativeGetHeading | `Java_cn_cycle_chart_api_navi_PositionManager_nativeGetHeading` | nmea_position_jni.cpp |
| 10 | jint | nativeGetFixQuality | `Java_cn_cycle_chart_api_navi_PositionManager_nativeGetFixQuality` | nmea_position_jni.cpp |
| 11 | jint | nativeGetSatelliteCount | `Java_cn_cycle_chart_api_navi_PositionManager_nativeGetSatelliteCount` | nmea_position_jni.cpp |
| 12 | jint | nativeSetPosition | `Java_cn_cycle_chart_api_navi_PositionManager_nativeSetPosition` | nmea_position_jni.cpp |
| 13 | jdoubleArray | nativeGetPosition | `Java_cn_cycle_chart_api_navi_PositionManager_nativeGetPosition` | nmea_position_jni.cpp |
| 14 | void | nativeSetCallback | `Java_cn_cycle_chart_api_navi_PositionManager_nativeSetCallback` | nmea_position_jni.cpp |

#### cn.cycle.chart.api.navi.PositionProvider (6个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreateSerial | `Java_cn_cycle_chart_api_navi_PositionProvider_nativeCreateSerial` | navi_ext_jni.cpp |
| 2 | jlong | nativeCreateNetwork | `Java_cn_cycle_chart_api_navi_PositionProvider_nativeCreateNetwork` | navi_ext_jni.cpp |
| 3 | void | nativeDestroy | `Java_cn_cycle_chart_api_navi_PositionProvider_nativeDestroy` | navi_ext_jni.cpp |
| 4 | jboolean | nativeConnect | `Java_cn_cycle_chart_api_navi_PositionProvider_nativeConnect` | navi_ext_jni.cpp |
| 5 | void | nativeDisconnect | `Java_cn_cycle_chart_api_navi_PositionProvider_nativeDisconnect` | navi_ext_jni.cpp |
| 6 | jboolean | nativeIsConnected | `Java_cn_cycle_chart_api_navi_PositionProvider_nativeIsConnected` | navi_ext_jni.cpp |

#### cn.cycle.chart.api.navi.Route (17个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_navi_Route_nativeCreate` | route_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_navi_Route_nativeDestroy` | route_jni.cpp |
| 3 | jstring | nativeGetId | `Java_cn_cycle_chart_api_navi_Route_nativeGetId` | route_jni.cpp |
| 4 | jstring | nativeGetName | `Java_cn_cycle_chart_api_navi_Route_nativeGetName` | route_jni.cpp |
| 5 | void | nativeSetName | `Java_cn_cycle_chart_api_navi_Route_nativeSetName` | route_jni.cpp |
| 6 | jint | nativeGetStatus | `Java_cn_cycle_chart_api_navi_Route_nativeGetStatus` | route_jni.cpp |
| 7 | jdouble | nativeGetTotalDistance | `Java_cn_cycle_chart_api_navi_Route_nativeGetTotalDistance` | route_jni.cpp |
| 8 | jint | nativeGetWaypointCount | `Java_cn_cycle_chart_api_navi_Route_nativeGetWaypointCount` | route_jni.cpp |
| 9 | jlong | nativeGetWaypoint | `Java_cn_cycle_chart_api_navi_Route_nativeGetWaypoint` | route_jni.cpp |
| 10 | void | nativeAddWaypoint | `Java_cn_cycle_chart_api_navi_Route_nativeAddWaypoint` | route_jni.cpp |
| 11 | void | nativeRemoveWaypoint | `Java_cn_cycle_chart_api_navi_Route_nativeRemoveWaypoint` | route_jni.cpp |
| 12 | jlong | nativeGetCurrentWaypoint | `Java_cn_cycle_chart_api_navi_Route_nativeGetCurrentWaypoint` | route_jni.cpp |
| 13 | jint | nativeAdvanceToNextWaypoint | `Java_cn_cycle_chart_api_navi_Route_nativeAdvanceToNextWaypoint` | route_jni.cpp |
| 14 | void | nativeInsertWaypoint | `Java_cn_cycle_chart_api_navi_Route_nativeInsertWaypoint` | route_jni.cpp |
| 15 | void | nativeClear | `Java_cn_cycle_chart_api_navi_Route_nativeClear` | route_jni.cpp |
| 16 | void | nativeReverse | `Java_cn_cycle_chart_api_navi_Route_nativeReverse` | route_jni.cpp |
| 17 | jlong | nativeGetEta | `Java_cn_cycle_chart_api_navi_Route_nativeGetEta` | route_jni.cpp |

#### cn.cycle.chart.api.navi.RouteManager (9个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_navi_RouteManager_nativeCreate` | route_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_navi_RouteManager_nativeDestroy` | route_jni.cpp |
| 3 | jint | nativeGetRouteCount | `Java_cn_cycle_chart_api_navi_RouteManager_nativeGetRouteCount` | route_jni.cpp |
| 4 | jlong | nativeGetRoute | `Java_cn_cycle_chart_api_navi_RouteManager_nativeGetRoute` | route_jni.cpp |
| 5 | jlong | nativeGetRouteById | `Java_cn_cycle_chart_api_navi_RouteManager_nativeGetRouteById` | route_jni.cpp |
| 6 | void | nativeAddRoute | `Java_cn_cycle_chart_api_navi_RouteManager_nativeAddRoute` | route_jni.cpp |
| 7 | void | nativeRemoveRoute | `Java_cn_cycle_chart_api_navi_RouteManager_nativeRemoveRoute` | route_jni.cpp |
| 8 | jlong | nativeGetActiveRoute | `Java_cn_cycle_chart_api_navi_RouteManager_nativeGetActiveRoute` | route_jni.cpp |
| 9 | void | nativeSetActiveRoute | `Java_cn_cycle_chart_api_navi_RouteManager_nativeSetActiveRoute` | route_jni.cpp |

#### cn.cycle.chart.api.navi.RoutePlanner (3个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_navi_RoutePlanner_nativeCreate` | route_planner_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_navi_RoutePlanner_nativeDestroy` | route_planner_jni.cpp |
| 3 | jlong | nativePlanRoute | `Java_cn_cycle_chart_api_navi_RoutePlanner_nativePlanRoute` | route_planner_jni.cpp |

#### cn.cycle.chart.api.navi.Track (10个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_navi_Track_nativeCreate` | track_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_navi_Track_nativeDestroy` | track_jni.cpp |
| 3 | jstring | nativeGetId | `Java_cn_cycle_chart_api_navi_Track_nativeGetId` | track_jni.cpp |
| 4 | jstring | nativeGetName | `Java_cn_cycle_chart_api_navi_Track_nativeGetName` | track_jni.cpp |
| 5 | void | nativeSetName | `Java_cn_cycle_chart_api_navi_Track_nativeSetName` | track_jni.cpp |
| 6 | jlong | nativeGetPointCount | `Java_cn_cycle_chart_api_navi_Track_nativeGetPointCount` | track_jni.cpp |
| 7 | jlong | nativeGetPointPtr | `Java_cn_cycle_chart_api_navi_Track_nativeGetPointPtr` | track_jni.cpp |
| 8 | void | nativeAddPoint | `Java_cn_cycle_chart_api_navi_Track_nativeAddPoint` | track_jni.cpp |
| 9 | void | nativeClear | `Java_cn_cycle_chart_api_navi_Track_nativeClear` | track_jni.cpp |
| 10 | jlong | nativeToLineString | `Java_cn_cycle_chart_api_navi_Track_nativeToLineString` | track_jni.cpp |

#### cn.cycle.chart.api.navi.TrackRecorder (10个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_navi_TrackRecorder_nativeCreate` | track_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_navi_TrackRecorder_nativeDestroy` | track_jni.cpp |
| 3 | void | nativeStart | `Java_cn_cycle_chart_api_navi_TrackRecorder_nativeStart` | track_jni.cpp |
| 4 | void | nativeStop | `Java_cn_cycle_chart_api_navi_TrackRecorder_nativeStop` | track_jni.cpp |
| 5 | void | nativePause | `Java_cn_cycle_chart_api_navi_TrackRecorder_nativePause` | track_jni.cpp |
| 6 | void | nativeResume | `Java_cn_cycle_chart_api_navi_TrackRecorder_nativeResume` | track_jni.cpp |
| 7 | jboolean | nativeIsRecording | `Java_cn_cycle_chart_api_navi_TrackRecorder_nativeIsRecording` | track_jni.cpp |
| 8 | jlong | nativeGetCurrentTrack | `Java_cn_cycle_chart_api_navi_TrackRecorder_nativeGetCurrentTrack` | track_jni.cpp |
| 9 | void | nativeSetInterval | `Java_cn_cycle_chart_api_navi_TrackRecorder_nativeSetInterval` | track_jni.cpp |
| 10 | void | nativeSetMinDistance | `Java_cn_cycle_chart_api_navi_TrackRecorder_nativeSetMinDistance` | track_jni.cpp |

#### cn.cycle.chart.api.navi.Waypoint (15个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_navi_Waypoint_nativeCreate` | route_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_navi_Waypoint_nativeDestroy` | route_jni.cpp |
| 3 | jdouble | nativeGetLatitude | `Java_cn_cycle_chart_api_navi_Waypoint_nativeGetLatitude` | route_jni.cpp |
| 4 | jdouble | nativeGetLongitude | `Java_cn_cycle_chart_api_navi_Waypoint_nativeGetLongitude` | route_jni.cpp |
| 5 | jstring | nativeGetName | `Java_cn_cycle_chart_api_navi_Waypoint_nativeGetName` | route_jni.cpp |
| 6 | void | nativeSetName | `Java_cn_cycle_chart_api_navi_Waypoint_nativeSetName` | route_jni.cpp |
| 7 | jboolean | nativeIsArrival | `Java_cn_cycle_chart_api_navi_Waypoint_nativeIsArrival` | route_jni.cpp |
| 8 | void | nativeSetArrivalRadius | `Java_cn_cycle_chart_api_navi_Waypoint_nativeSetArrivalRadius` | route_jni.cpp |
| 9 | jdouble | nativeGetArrivalRadius | `Java_cn_cycle_chart_api_navi_Waypoint_nativeGetArrivalRadius` | route_jni.cpp |
| 10 | void | nativeSetTurnRadius | `Java_cn_cycle_chart_api_navi_Waypoint_nativeSetTurnRadius` | route_jni.cpp |
| 11 | jdouble | nativeGetTurnRadius | `Java_cn_cycle_chart_api_navi_Waypoint_nativeGetTurnRadius` | route_jni.cpp |
| 12 | void | nativeSetType | `Java_cn_cycle_chart_api_navi_Waypoint_nativeSetType` | route_jni.cpp |
| 13 | jint | nativeGetType | `Java_cn_cycle_chart_api_navi_Waypoint_nativeGetType` | route_jni.cpp |
| 14 | void | nativeSetDescription | `Java_cn_cycle_chart_api_navi_Waypoint_nativeSetDescription` | route_jni.cpp |
| 15 | jstring | nativeGetDescription | `Java_cn_cycle_chart_api_navi_Waypoint_nativeGetDescription` | route_jni.cpp |

### 2.18 Parser 模块

#### cn.cycle.chart.api.parser.ChartParser (4个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeGetInstance | `Java_cn_cycle_chart_api_parser_ChartParser_nativeGetInstance` | chart_parser_jni.cpp |
| 2 | jint | nativeInitialize | `Java_cn_cycle_chart_api_parser_ChartParser_nativeInitialize` | chart_parser_jni.cpp |
| 3 | void | nativeShutdown | `Java_cn_cycle_chart_api_parser_ChartParser_nativeShutdown` | chart_parser_jni.cpp |
| 4 | jintArray | nativeGetSupportedFormats | `Java_cn_cycle_chart_api_parser_ChartParser_nativeGetSupportedFormats` | chart_parser_jni.cpp |

#### cn.cycle.chart.api.parser.IParser (7个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_parser_IParser_nativeCreate` | chart_parser_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_parser_IParser_nativeDestroy` | chart_parser_jni.cpp |
| 3 | jint | nativeOpen | `Java_cn_cycle_chart_api_parser_IParser_nativeOpen` | chart_parser_jni.cpp |
| 4 | void | nativeClose | `Java_cn_cycle_chart_api_parser_IParser_nativeClose` | chart_parser_jni.cpp |
| 5 | jlong | nativeParse | `Java_cn_cycle_chart_api_parser_IParser_nativeParse` | chart_parser_jni.cpp |
| 6 | jlong | nativeParseIncremental | `Java_cn_cycle_chart_api_parser_IParser_nativeParseIncremental` | chart_parser_jni.cpp |
| 7 | jint | nativeGetFormat | `Java_cn_cycle_chart_api_parser_IParser_nativeGetFormat` | chart_parser_jni.cpp |

#### cn.cycle.chart.api.parser.IncrementalParser (12个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_parser_IncrementalParser_nativeCreate` | parser_ext_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_parser_IncrementalParser_nativeDestroy` | parser_ext_jni.cpp |
| 3 | void | nativeStart | `Java_cn_cycle_chart_api_parser_IncrementalParser_nativeStart` | parser_ext_jni.cpp |
| 4 | void | nativePause | `Java_cn_cycle_chart_api_parser_IncrementalParser_nativePause` | parser_ext_jni.cpp |
| 5 | void | nativeResume | `Java_cn_cycle_chart_api_parser_IncrementalParser_nativeResume` | parser_ext_jni.cpp |
| 6 | void | nativeCancel | `Java_cn_cycle_chart_api_parser_IncrementalParser_nativeCancel` | parser_ext_jni.cpp |
| 7 | jlong | nativeParseNext | `Java_cn_cycle_chart_api_parser_IncrementalParser_nativeParseNext` | parser_ext_jni.cpp |
| 8 | jboolean | nativeHasMore | `Java_cn_cycle_chart_api_parser_IncrementalParser_nativeHasMore` | parser_ext_jni.cpp |
| 9 | jfloat | nativeGetProgress | `Java_cn_cycle_chart_api_parser_IncrementalParser_nativeGetProgress` | parser_ext_jni.cpp |
| 10 | jboolean | nativeHasFileChanged | `Java_cn_cycle_chart_api_parser_IncrementalParser_nativeHasFileChanged` | parser_ext_jni.cpp |
| 11 | void | nativeMarkProcessed | `Java_cn_cycle_chart_api_parser_IncrementalParser_nativeMarkProcessed` | parser_ext_jni.cpp |
| 12 | void | nativeClearState | `Java_cn_cycle_chart_api_parser_IncrementalParser_nativeClearState` | parser_ext_jni.cpp |

#### cn.cycle.chart.api.parser.S57Parser (7个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_parser_S57Parser_nativeCreate` | parser_ext_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_parser_S57Parser_nativeDestroy` | parser_ext_jni.cpp |
| 3 | jboolean | nativeOpen | `Java_cn_cycle_chart_api_parser_S57Parser_nativeOpen` | parser_ext_jni.cpp |
| 4 | jlong | nativeParse | `Java_cn_cycle_chart_api_parser_S57Parser_nativeParse` | parser_ext_jni.cpp |
| 5 | void | nativeSetFeatureFilter | `Java_cn_cycle_chart_api_parser_S57Parser_nativeSetFeatureFilter` | parser_ext_jni.cpp |
| 6 | void | nativeSetSpatialFilter | `Java_cn_cycle_chart_api_parser_S57Parser_nativeSetSpatialFilter` | parser_ext_jni.cpp |
| 7 | jlong | nativeParseFile | `Java_cn_cycle_chart_api_parser_S57Parser_nativeParseFile` | parser_ext_jni.cpp |

### 2.19 Perf 模块

#### cn.cycle.chart.api.perf.PerformanceMonitor (7个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeGetInstance | `Java_cn_cycle_chart_api_perf_PerformanceMonitor_nativeGetInstance` | util_ext_jni.cpp |
| 2 | void | nativeStartFrame | `Java_cn_cycle_chart_api_perf_PerformanceMonitor_nativeStartFrame` | util_ext_jni.cpp |
| 3 | void | nativeEndFrame | `Java_cn_cycle_chart_api_perf_PerformanceMonitor_nativeEndFrame` | util_ext_jni.cpp |
| 4 | jdouble | nativeGetFPS | `Java_cn_cycle_chart_api_perf_PerformanceMonitor_nativeGetFPS` | util_ext_jni.cpp |
| 5 | jlong | nativeGetMemoryUsed | `Java_cn_cycle_chart_api_perf_PerformanceMonitor_nativeGetMemoryUsed` | util_ext_jni.cpp |
| 6 | void | nativeReset | `Java_cn_cycle_chart_api_perf_PerformanceMonitor_nativeReset` | util_ext_jni.cpp |
| 7 | void | nativeGetStats | `Java_cn_cycle_chart_api_perf_PerformanceMonitor_nativeGetStats` | util_ext_jni.cpp |

#### cn.cycle.chart.api.perf.RenderStats (1个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | void | nativeReset | `Java_cn_cycle_chart_api_perf_RenderStats_nativeReset` | util_ext_jni.cpp |

### 2.20 Plugin 模块

#### cn.cycle.chart.api.plugin.ChartPlugin (6个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_plugin_ChartPlugin_nativeCreate` | plugin_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_plugin_ChartPlugin_nativeDestroy` | plugin_jni.cpp |
| 3 | jstring | nativeGetName | `Java_cn_cycle_chart_api_plugin_ChartPlugin_nativeGetName` | plugin_jni.cpp |
| 4 | jstring | nativeGetVersion | `Java_cn_cycle_chart_api_plugin_ChartPlugin_nativeGetVersion` | plugin_jni.cpp |
| 5 | jint | nativeInitialize | `Java_cn_cycle_chart_api_plugin_ChartPlugin_nativeInitialize` | plugin_jni.cpp |
| 6 | void | nativeShutdown | `Java_cn_cycle_chart_api_plugin_ChartPlugin_nativeShutdown` | plugin_jni.cpp |

#### cn.cycle.chart.api.plugin.PluginManager (6个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeGetInstance | `Java_cn_cycle_chart_api_plugin_PluginManager_nativeGetInstance` | plugin_jni.cpp |
| 2 | jint | nativeLoadPlugin | `Java_cn_cycle_chart_api_plugin_PluginManager_nativeLoadPlugin` | plugin_jni.cpp |
| 3 | jint | nativeUnloadPlugin | `Java_cn_cycle_chart_api_plugin_PluginManager_nativeUnloadPlugin` | plugin_jni.cpp |
| 4 | jint | nativeGetPluginCount | `Java_cn_cycle_chart_api_plugin_PluginManager_nativeGetPluginCount` | plugin_jni.cpp |
| 5 | jstring | nativeGetPluginName | `Java_cn_cycle_chart_api_plugin_PluginManager_nativeGetPluginName` | plugin_jni.cpp |
| 6 | jlong | nativeGetPlugin | `Java_cn_cycle_chart_api_plugin_PluginManager_nativeGetPlugin` | plugin_jni.cpp |

### 2.21 Proj 模块

#### cn.cycle.chart.api.proj.CoordTransformer (10个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_proj_CoordTransformer_nativeCreate` | coord_transformer_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_proj_CoordTransformer_nativeDestroy` | coord_transformer_jni.cpp |
| 3 | jboolean | nativeIsValid | `Java_cn_cycle_chart_api_proj_CoordTransformer_nativeIsValid` | coord_transformer_jni.cpp |
| 4 | jobject | nativeTransform | `Java_cn_cycle_chart_api_proj_CoordTransformer_nativeTransform` | coord_transformer_jni.cpp |
| 5 | jobject | nativeTransformInverse | `Java_cn_cycle_chart_api_proj_CoordTransformer_nativeTransformInverse` | coord_transformer_jni.cpp |
| 6 | void | nativeTransformArray | `Java_cn_cycle_chart_api_proj_CoordTransformer_nativeTransformArray` | coord_transformer_jni.cpp |
| 7 | jobject | nativeTransformEnvelope | `Java_cn_cycle_chart_api_proj_CoordTransformer_nativeTransformEnvelope` | coord_transformer_jni.cpp |
| 8 | jlong | nativeTransformGeometry | `Java_cn_cycle_chart_api_proj_CoordTransformer_nativeTransformGeometry` | coord_transformer_jni.cpp |
| 9 | jstring | nativeGetSourceCrs | `Java_cn_cycle_chart_api_proj_CoordTransformer_nativeGetSourceCrs` | coord_transformer_jni.cpp |
| 10 | jstring | nativeGetTargetCrs | `Java_cn_cycle_chart_api_proj_CoordTransformer_nativeGetTargetCrs` | coord_transformer_jni.cpp |

#### cn.cycle.chart.api.proj.TransformMatrix (17个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_proj_TransformMatrix_nativeCreate` | transform_matrix_jni.cpp |
| 2 | jlong | nativeCreateIdentity | `Java_cn_cycle_chart_api_proj_TransformMatrix_nativeCreateIdentity` | transform_matrix_jni.cpp |
| 3 | jlong | nativeCreateTranslation | `Java_cn_cycle_chart_api_proj_TransformMatrix_nativeCreateTranslation` | transform_matrix_jni.cpp |
| 4 | jlong | nativeCreateScale | `Java_cn_cycle_chart_api_proj_TransformMatrix_nativeCreateScale` | transform_matrix_jni.cpp |
| 5 | jlong | nativeCreateRotation | `Java_cn_cycle_chart_api_proj_TransformMatrix_nativeCreateRotation` | transform_matrix_jni.cpp |
| 6 | void | nativeDestroy | `Java_cn_cycle_chart_api_proj_TransformMatrix_nativeDestroy` | transform_matrix_jni.cpp |
| 7 | jlong | nativeMultiply | `Java_cn_cycle_chart_api_proj_TransformMatrix_nativeMultiply` | transform_matrix_jni.cpp |
| 8 | jlong | nativeInverse | `Java_cn_cycle_chart_api_proj_TransformMatrix_nativeInverse` | transform_matrix_jni.cpp |
| 9 | void | nativeTransformPoint | `Java_cn_cycle_chart_api_proj_TransformMatrix_nativeTransformPoint` | transform_matrix_jni.cpp |
| 10 | jdoubleArray | nativeGetElements | `Java_cn_cycle_chart_api_proj_TransformMatrix_nativeGetElements` | transform_matrix_jni.cpp |
| 11 | void | nativeSetElements | `Java_cn_cycle_chart_api_proj_TransformMatrix_nativeSetElements` | transform_matrix_jni.cpp |
| 12 | void | nativeTranslate | `Java_cn_cycle_chart_api_proj_TransformMatrix_nativeTranslate` | transform_matrix_jni.cpp |
| 13 | void | nativeScale | `Java_cn_cycle_chart_api_proj_TransformMatrix_nativeScale` | transform_matrix_jni.cpp |
| 14 | void | nativeRotate | `Java_cn_cycle_chart_api_proj_TransformMatrix_nativeRotate` | transform_matrix_jni.cpp |
| 15 | void | nativeMultiplyInPlace | `Java_cn_cycle_chart_api_proj_TransformMatrix_nativeMultiplyInPlace` | transform_matrix_jni.cpp |
| 16 | jdoubleArray | nativeTransformCoord | `Java_cn_cycle_chart_api_proj_TransformMatrix_nativeTransformCoord` | transform_matrix_jni.cpp |
| 17 | jdoubleArray | nativeGetValues | `Java_cn_cycle_chart_api_proj_TransformMatrix_nativeGetValues` | transform_matrix_jni.cpp |

### 2.22 Recovery 模块

#### cn.cycle.chart.api.recovery.CircuitBreaker (7个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_recovery_CircuitBreaker_nativeCreate` | recovery_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_recovery_CircuitBreaker_nativeDestroy` | recovery_jni.cpp |
| 3 | jint | nativeGetState | `Java_cn_cycle_chart_api_recovery_CircuitBreaker_nativeGetState` | recovery_jni.cpp |
| 4 | jboolean | nativeAllowRequest | `Java_cn_cycle_chart_api_recovery_CircuitBreaker_nativeAllowRequest` | recovery_jni.cpp |
| 5 | void | nativeRecordSuccess | `Java_cn_cycle_chart_api_recovery_CircuitBreaker_nativeRecordSuccess` | recovery_jni.cpp |
| 6 | void | nativeRecordFailure | `Java_cn_cycle_chart_api_recovery_CircuitBreaker_nativeRecordFailure` | recovery_jni.cpp |
| 7 | void | nativeReset | `Java_cn_cycle_chart_api_recovery_CircuitBreaker_nativeReset` | recovery_jni.cpp |

#### cn.cycle.chart.api.recovery.ErrorRecoveryManager (7个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_recovery_ErrorRecoveryManager_nativeCreate` | recovery_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_recovery_ErrorRecoveryManager_nativeDestroy` | recovery_jni.cpp |
| 3 | jint | nativeInitialize | `Java_cn_cycle_chart_api_recovery_ErrorRecoveryManager_nativeInitialize` | recovery_jni.cpp |
| 4 | void | nativeShutdown | `Java_cn_cycle_chart_api_recovery_ErrorRecoveryManager_nativeShutdown` | recovery_jni.cpp |
| 5 | jint | nativeHandleError | `Java_cn_cycle_chart_api_recovery_ErrorRecoveryManager_nativeHandleError` | recovery_jni.cpp |
| 6 | void | nativeSetDegradationLevel | `Java_cn_cycle_chart_api_recovery_ErrorRecoveryManager_nativeSetDegradationLevel` | recovery_jni.cpp |
| 7 | void | nativeRegisterStrategy | `Java_cn_cycle_chart_api_recovery_ErrorRecoveryManager_nativeRegisterStrategy` | recovery_jni.cpp |

#### cn.cycle.chart.api.recovery.GracefulDegradation (7个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_recovery_GracefulDegradation_nativeCreate` | recovery_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_recovery_GracefulDegradation_nativeDestroy` | recovery_jni.cpp |
| 3 | jint | nativeGetLevel | `Java_cn_cycle_chart_api_recovery_GracefulDegradation_nativeGetLevel` | recovery_jni.cpp |
| 4 | void | nativeSetLevel | `Java_cn_cycle_chart_api_recovery_GracefulDegradation_nativeSetLevel` | recovery_jni.cpp |
| 5 | jboolean | nativeIsFeatureEnabled | `Java_cn_cycle_chart_api_recovery_GracefulDegradation_nativeIsFeatureEnabled` | recovery_jni.cpp |
| 6 | void | nativeDisableFeature | `Java_cn_cycle_chart_api_recovery_GracefulDegradation_nativeDisableFeature` | recovery_jni.cpp |
| 7 | void | nativeEnableFeature | `Java_cn_cycle_chart_api_recovery_GracefulDegradation_nativeEnableFeature` | recovery_jni.cpp |

### 2.23 Symbology 模块

#### cn.cycle.chart.api.symbology.ComparisonFilter (6个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | void | nativeDestroy | `Java_cn_cycle_chart_api_symbology_ComparisonFilter_nativeDestroy` | symbology_ext_jni.cpp |
| 2 | jboolean | nativeEvaluate | `Java_cn_cycle_chart_api_symbology_ComparisonFilter_nativeEvaluate` | symbology_ext_jni.cpp |
| 3 | jstring | nativeGetPropertyName | `Java_cn_cycle_chart_api_symbology_ComparisonFilter_nativeGetPropertyName` | symbology_ext_jni.cpp |
| 4 | jint | nativeGetOperator | `Java_cn_cycle_chart_api_symbology_ComparisonFilter_nativeGetOperator` | symbology_ext_jni.cpp |
| 5 | jstring | nativeGetValue | `Java_cn_cycle_chart_api_symbology_ComparisonFilter_nativeGetValue` | symbology_ext_jni.cpp |
| 6 | jlong | nativeCreate | `Java_cn_cycle_chart_api_symbology_ComparisonFilter_nativeCreate` | symbology_jni.cpp |

#### cn.cycle.chart.api.symbology.Filter (5个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_symbology_Filter_nativeCreate` | symbology_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_symbology_Filter_nativeDestroy` | symbology_jni.cpp |
| 3 | jint | nativeGetType | `Java_cn_cycle_chart_api_symbology_Filter_nativeGetType` | symbology_jni.cpp |
| 4 | jboolean | nativeEvaluate | `Java_cn_cycle_chart_api_symbology_Filter_nativeEvaluate` | symbology_jni.cpp |
| 5 | jstring | nativeToString | `Java_cn_cycle_chart_api_symbology_Filter_nativeToString` | symbology_jni.cpp |

#### cn.cycle.chart.api.symbology.LogicalFilter (2个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_symbology_LogicalFilter_nativeCreate` | symbology_jni.cpp |
| 2 | void | nativeAddFilter | `Java_cn_cycle_chart_api_symbology_LogicalFilter_nativeAddFilter` | symbology_jni.cpp |

#### cn.cycle.chart.api.symbology.Symbolizer (6个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jboolean | nativeSymbolize | `Java_cn_cycle_chart_api_symbology_Symbolizer_nativeSymbolize` | symbology_ext_jni.cpp |
| 2 | jlong | nativeCreate | `Java_cn_cycle_chart_api_symbology_Symbolizer_nativeCreate` | symbology_jni.cpp |
| 3 | void | nativeDestroy | `Java_cn_cycle_chart_api_symbology_Symbolizer_nativeDestroy` | symbology_jni.cpp |
| 4 | jint | nativeGetType | `Java_cn_cycle_chart_api_symbology_Symbolizer_nativeGetType` | symbology_jni.cpp |
| 5 | jlong | nativeGetStyle | `Java_cn_cycle_chart_api_symbology_Symbolizer_nativeGetStyle` | symbology_jni.cpp |
| 6 | void | nativeSetStyle | `Java_cn_cycle_chart_api_symbology_Symbolizer_nativeSetStyle` | symbology_jni.cpp |

#### cn.cycle.chart.api.symbology.SymbolizerRule (15个)

| # | 返回类型 | Java方法名 | JNI函数名 | C++源文件 |
|---|----------|------------|-----------|-----------|
| 1 | jlong | nativeCreate | `Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeCreate` | symbology_jni.cpp |
| 2 | void | nativeDestroy | `Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeDestroy` | symbology_jni.cpp |
| 3 | jstring | nativeGetName | `Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeGetName` | symbology_jni.cpp |
| 4 | void | nativeSetName | `Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeSetName` | symbology_jni.cpp |
| 5 | jlong | nativeGetFilter | `Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeGetFilter` | symbology_jni.cpp |
| 6 | void | nativeSetFilter | `Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeSetFilter` | symbology_jni.cpp |
| 7 | jlong | nativeGetSymbolizer | `Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeGetSymbolizer` | symbology_jni.cpp |
| 8 | void | nativeSetSymbolizer | `Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeSetSymbolizer` | symbology_jni.cpp |
| 9 | jdouble | nativeGetMinScale | `Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeGetMinScale` | symbology_jni.cpp |
| 10 | jdouble | nativeGetMaxScale | `Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeGetMaxScale` | symbology_jni.cpp |
| 11 | void | nativeSetMinScale | `Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeSetMinScale` | symbology_jni.cpp |
| 12 | void | nativeSetMaxScale | `Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeSetMaxScale` | symbology_jni.cpp |
| 13 | void | nativeAddSymbolizer | `Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeAddSymbolizer` | symbology_jni.cpp |
| 14 | jboolean | nativeIsApplicable | `Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeIsApplicable` | symbology_jni.cpp |
| 15 | void | nativeSetScaleRange | `Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeSetScaleRange` | symbology_jni.cpp |
