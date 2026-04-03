# 海图导航系统设计文档

> **文档版本**: v1.3  
> **创建日期**: 2026-04-03  
> **更新日期**: 2026-04-03  
> **需求文档**: [chart_navi_system.md](../../doc/yangzt/chart_navi_system.md)  
> **变更说明**: 补充RRT*算法、遗传算法多目标优化、轨迹分享服务、WGS-84转CGCS2000详细实现、大圆距离计算说明、轨迹导入服务接口

---

## 一、系统概述

### 1.1 系统定位

海图导航系统是海图导航平台的核心功能模块，负责航线规划、航行引导、定位追踪等导航相关功能。系统基于北斗定位信息，结合实时水深、航道条件等多因素进行智能航线规划，辅助船舶安全高效行驶。

### 1.2 设计目标

| 目标 | 具体指标 | 设计策略 |
|-----|---------|---------|
| 定位精度 | VIP用户<1m，普通用户5-10m | 多源定位融合，差分定位支持 |
| 定位频率 | VIP用户10s/次，普通用户30s/次 | 分级服务策略，动态调整频率 |
| 航线规划 | 多因素智能规划 | A*算法+约束满足，支持动态避障 |
| 动态避障 | 支持实时障碍物规避 | 实时AIS数据集成，动态重规划 |
| 离线导航 | 支持离线航线规划和导航 | 本地数据缓存，离线算法支持 |

### 1.3 系统边界

```
┌─────────────────────────────────────────────────────────────┐
│                    海图导航系统                               │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │  定位子系统   │  │  航线子系统   │  │  导航子系统   │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │  轨迹子系统   │  │  AIS子系统    │  │  数据管理     │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
└─────────────────────────────────────────────────────────────┘
         ↓                  ↓                  ↓
    北斗定位设备        海图数据库          AIS设备
```

---

## 二、系统架构设计

### 2.1 总体架构

```
┌─────────────────────────────────────────────────────────────────┐
│                        应用层 (Application Layer)                │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐       │
│  │ 航线规划  │  │ 航行引导  │  │ 轨迹管理  │  │ AIS显示   │       │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘       │
└─────────────────────────────────────────────────────────────────┘
                              ↕
┌─────────────────────────────────────────────────────────────────┐
│                        服务层 (Service Layer)                    │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐       │
│  │定位服务   │  │航线服务   │  │导航服务   │  │AIS服务    │       │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘       │
└─────────────────────────────────────────────────────────────────┘
                              ↕
┌─────────────────────────────────────────────────────────────────┐
│                        核心层 (Core Layer)                       │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐       │
│  │NMEA解析   │  │路径规划   │  │导航计算   │  │AIS解析    │       │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘       │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐       │
│  │坐标转换   │  │UKC计算    │  │碰撞评估   │  │数据存储   │       │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘       │
└─────────────────────────────────────────────────────────────────┘
                              ↕
┌─────────────────────────────────────────────────────────────────┐
│                        接口层 (Interface Layer)                  │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐       │
│  │串口接口   │  │蓝牙接口   │  │网络接口   │  │系统API    │       │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘       │
└─────────────────────────────────────────────────────────────────┘
```

### 2.2 模块划分

| 模块名称 | 职责 | 依赖模块 |
|---------|------|---------|
| 定位模块 | 定位数据接收、解析、处理 | 接口层 |
| 航线模块 | 航线规划、编辑、管理 | 核心层、数据管理 |
| 导航模块 | 航行引导、偏航检测、航点提示 | 定位模块、航线模块 |
| 轨迹模块 | 轨迹记录、回放、分析 | 定位模块、数据管理 |
| AIS模块 | AIS数据接收、解析、显示 | 接口层、导航模块 |
| 数据管理 | 数据存储、缓存、同步 | 核心层 |

### 2.3 数据流设计

```
┌─────────────┐
│ 北斗定位设备 │
└──────┬──────┘
       │ NMEA数据流
       ↓
┌─────────────┐     ┌─────────────┐
│  接口层      │────→│  NMEA解析   │
└─────────────┘     └──────┬──────┘
                           │ 定位数据
                           ↓
                    ┌─────────────┐
                    │  定位服务    │
                    └──────┬──────┘
                           │
          ┌────────────────┼────────────────┐
          ↓                ↓                ↓
   ┌─────────────┐  ┌─────────────┐  ┌─────────────┐
   │  导航服务    │  │  轨迹服务    │  │  应用层      │
   └─────────────┘  └─────────────┘  └─────────────┘
```

---

## 三、核心模块详细设计

### 3.1 定位模块设计

#### 3.1.1 模块结构

```
PositioningModule/
├── include/
│   ├── positioning/
│   │   ├── position_types.h          # 定位数据类型定义
│   │   ├── nmea_parser.h             # NMEA解析器接口
│   │   ├── position_provider.h       # 定位数据提供者接口
│   │   ├── position_filter.h         # 定位数据过滤器
│   │   ├── coordinate_converter.h    # 坐标转换器
│   │   └── position_manager.h        # 定位管理器
│   └── positioning_export.h          # 导出宏定义
├── src/
│   ├── nmea_parser.cpp               # NMEA解析实现
│   ├── serial_position_provider.cpp  # 串口定位提供者
│   ├── bluetooth_position_provider.cpp # 蓝牙定位提供者
│   ├── network_position_provider.cpp  # 网络定位提供者
│   ├── system_position_provider.cpp   # 系统定位提供者
│   ├── position_filter.cpp            # 定位过滤实现
│   ├── coordinate_converter.cpp       # 坐标转换实现
│   └── position_manager.cpp           # 定位管理实现
└── tests/
    ├── test_nmea_parser.cpp
    ├── test_position_filter.cpp
    └── test_coordinate_converter.cpp
```

#### 3.1.2 核心类设计

**PositionData - 定位数据结构**:
```cpp
struct PositionData {
    double timestamp;           // UTC时间戳
    double longitude;           // 经度 (度)
    double latitude;            // 纬度 (度)
    double altitude;            // 海拔高度 (米)
    double heading;             // 航向 (度)
    double speed;               // 速度 (节)
    double hdop;                // 水平精度因子
    double vdop;                // 垂直精度因子
    int satellite_count;        // 使用卫星数
    PositionQuality quality;    // 定位质量
    PositionSource source;      // 数据来源
};

enum class PositionQuality {
    Invalid = 0,      // 无效
    Gps = 1,          // GPS定位
    DGps = 2,         // 差分GPS
    Pps = 3,          // PPS定位
    Rtk = 4,          // RTK固定解
    FloatRtk = 5      // RTK浮动解
};

enum class PositionSource {
    System,           // 系统定位
    Serial,           // 串口设备
    Bluetooth,        // 蓝牙设备
    Network           // 网络设备
};
```

**NmeaParser - NMEA解析器**:
```cpp
class NmeaParser {
public:
    static NmeaParser& Instance();
    
    bool Parse(const std::string& nmea_sentence, PositionData& data);
    bool ParseGGA(const std::string& sentence, GgaData& data);
    bool ParseRMC(const std::string& sentence, RmcData& data);
    bool ParseVTG(const std::string& sentence, VtgData& data);
    bool ParseGSA(const std::string& sentence, GsaData& data);
    bool ParseGSV(const std::string& sentence, GsvData& data);
    
    bool ValidateChecksum(const std::string& sentence);
    std::string CalculateChecksum(const std::string& sentence);
    
private:
    NmeaParser() = default;
    bool ParseField(const std::vector<std::string>& fields, size_t index, 
                    std::string& value);
    bool ParseField(const std::vector<std::string>& fields, size_t index, 
                    double& value);
    bool ParseField(const std::vector<std::string>& fields, size_t index, 
                    int& value);
    double ParseCoordinate(const std::string& coord, const std::string& hemisphere);
    double ParseTime(const std::string& time_str);
    double ParseDate(const std::string& date_str);
};
```

**PositionProvider - 定位数据提供者接口**:
```cpp
class IPositionProvider {
public:
    virtual ~IPositionProvider() = default;
    
    virtual bool Initialize(const ProviderConfig& config) = 0;
    virtual void Shutdown() = 0;
    virtual bool IsConnected() const = 0;
    virtual PositionData GetLastPosition() const = 0;
    
    virtual void SetPositionCallback(PositionCallback callback) = 0;
    virtual void SetErrorCallback(ErrorCallback callback) = 0;
    
protected:
    PositionCallback position_callback_;
    ErrorCallback error_callback_;
};

using PositionCallback = std::function<void(const PositionData&)>;
using ErrorCallback = std::function<void(const std::string&)>;
```

**PositionFilter - 定位数据过滤器**:
```cpp
class PositionFilter {
public:
    PositionFilter();
    
    void SetMaxSpeed(double max_speed_knots);
    void SetMaxAcceleration(double max_acceleration);
    void SetSmoothWindowSize(int window_size);
    void SetMaxHdop(double max_hdop);
    void SetMinSatellites(int min_count);
    
    PositionData Filter(const PositionData& raw_data);
    void Reset();
    
    FilterStatistics GetStatistics() const;
    
private:
    bool IsValid(const PositionData& data);
    bool IsOutlier(const PositionData& data);
    PositionData Smooth(const PositionData& data);
    PositionData Extrapolate(double timestamp);
    bool ShouldRecord(const PositionData& data);
    
    std::deque<PositionData> history_;
    double max_speed_;
    double max_acceleration_;
    int smooth_window_size_;
    double max_hdop_;
    int min_satellites_;
    PositionData last_valid_;
    
    FilterStatistics stats_;
};

struct FilterStatistics {
    int total_received;
    int valid_count;
    int outlier_count;
    int extrapolated_count;
    double outlier_rate;
};

class PositionFilter {
public:
    PositionData Filter(const PositionData& raw_data) {
        stats_.total_received++;
        
        // 1. 有效性检查
        if (!IsValid(raw_data)) {
            stats_.extrapolated_count++;
            return Extrapolate(raw_data.timestamp);
        }
        
        // 2. 跳点检测
        if (IsOutlier(raw_data)) {
            stats_.outlier_count++;
            return Extrapolate(raw_data.timestamp);
        }
        
        // 3. 平滑处理
        PositionData smoothed = Smooth(raw_data);
        
        // 4. 更新历史
        history_.push_back(smoothed);
        if (history_.size() > smooth_window_size_) {
            history_.pop_front();
        }
        
        last_valid_ = smoothed;
        stats_.valid_count++;
        stats_.outlier_rate = 
            static_cast<double>(stats_.outlier_count) / stats_.total_received;
        
        return smoothed;
    }
    
private:
    bool IsValid(const PositionData& data) {
        // 检查坐标范围
        if (data.longitude < -180 || data.longitude > 180) return false;
        if (data.latitude < -90 || data.latitude > 90) return false;
        
        // 检查HDOP (水平精度因子)
        if (data.hdop > max_hdop_) return false;
        
        // 检查卫星数
        if (data.satellite_count < min_satellites_) return false;
        
        // 检查定位质量
        if (data.quality == PositionQuality::Invalid) return false;
        
        return true;
    }
    
    bool IsOutlier(const PositionData& data) {
        if (history_.empty()) return false;
        
        // 计算与上一个有效点的距离
        double dist = CoordinateConverter::Instance().CalculateGreatCircleDistance(
            last_valid_.latitude, last_valid_.longitude,
            data.latitude, data.longitude);
        
        // 计算理论最大移动距离
        double time_diff = data.timestamp - last_valid_.timestamp;
        double max_dist = max_speed_ * time_diff / 3600.0;
        
        // 如果实际距离超过理论最大距离的3倍，判定为跳点
        if (dist > max_dist * 3.0) {
            return true;
        }
        
        // 加速度检查
        if (time_diff > 0) {
            double speed_change = std::abs(data.speed - last_valid_.speed);
            double acceleration = speed_change / time_diff * 3600.0;
            if (acceleration > max_acceleration_) {
                return true;
            }
        }
        
        return false;
    }
    
    PositionData Extrapolate(double timestamp) {
        if (history_.size() < 2) {
            return last_valid_;
        }
        
        // 线性外推
        PositionData extrapolated = last_valid_;
        double time_diff = timestamp - last_valid_.timestamp;
        
        extrapolated.longitude += last_valid_.speed * 
            std::sin(last_valid_.heading * M_PI / 180) * 
            time_diff / 3600.0 / 60.0;
        extrapolated.latitude += last_valid_.speed * 
            std::cos(last_valid_.heading * M_PI / 180) * 
            time_diff / 3600.0 / 60.0;
        extrapolated.timestamp = timestamp;
        
        return extrapolated;
    }
    
    PositionData Smooth(const PositionData& data) {
        if (history_.size() < 2) {
            return data;
        }
        
        // 加权移动平均
        PositionData smoothed = data;
        double weight_sum = 0;
        double lon_sum = 0;
        double lat_sum = 0;
        
        for (size_t i = 0; i < history_.size(); ++i) {
            double weight = i + 1;
            lon_sum += history_[i].longitude * weight;
            lat_sum += history_[i].latitude * weight;
            weight_sum += weight;
        }
        
        smoothed.longitude = lon_sum / weight_sum;
        smoothed.latitude = lat_sum / weight_sum;
        
        return smoothed;
    }
};
```

**CoordinateConverter - 坐标转换器**:
```cpp
class CoordinateConverter {
public:
    static CoordinateConverter& Instance();
    
    // 经纬度转平面坐标 (墨卡托投影)
    void LatLonToMercator(double lat, double lon, double& x, double& y);
    void MercatorToLatLon(double x, double y, double& lat, double& lon);
    
    // WGS-84转CGCS2000
    void Wgs84ToCgcs2000(double wgs_lat, double wgs_lon, 
                         double& cgcs_lat, double& cgcs_lon);
    
    // 大地坐标转屏幕坐标
    void GeoToScreen(double lat, double lon, double scale, 
                     double origin_x, double origin_y,
                     double& screen_x, double& screen_y);
    void ScreenToGeo(double screen_x, double screen_y, double scale,
                     double origin_x, double origin_y,
                     double& lat, double& lon);
    
    // 大圆距离计算
    double CalculateGreatCircleDistance(double lat1, double lon1, 
                                        double lat2, double lon2);
    
    // 方位角计算
    double CalculateBearing(double lat1, double lon1, 
                           double lat2, double lon2);
    
    // 目标点计算
    void CalculateDestination(double lat1, double lon1, 
                             double bearing, double distance,
                             double& lat2, double& lon2);
};
```

**WGS-84转CGCS2000详细实现**:

WGS-84与CGCS2000坐标系差异微小（厘米级），但在高精度应用中需要进行转换。两者主要差异在于椭球参数和定位基准。

```cpp
struct EllipsoidParams {
    double a;           // 长半轴 (米)
    double f;           // 扁率
    double b;           // 短半轴
    double e2;          // 第一偏心率的平方
    double ep2;         // 第二偏心率的平方
};

class CoordinateConverter {
public:
    // WGS-84椭球参数
    static constexpr EllipsoidParams WGS84 = {
        6378137.0,                          // a
        1.0 / 298.257223563,                // f
        6356752.314245,                     // b
        0.00669437999014,                   // e2
        0.00673949674228                    // ep2
    };
    
    // CGCS2000椭球参数 (与WGS-84几乎相同)
    static constexpr EllipsoidParams CGCS2000 = {
        6378137.0,                          // a
        1.0 / 298.257222101,                // f
        6356752.314140,                     // b
        0.00669438002290,                   // e2
        0.00673949677548                    // ep2
    };
    
    // 七参数转换模型
    struct SevenParameters {
        double dx;          // X平移 (米)
        double dy;          // Y平移 (米)
        double dz;          // Z平移 (米)
        double rx;          // X旋转 (弧度)
        double ry;          // Y旋转 (弧度)
        double rz;          // Z旋转 (弧度)
        double scale;       // 尺度因子
    };
    
    void Wgs84ToCgcs2000(double wgs_lat, double wgs_lon, 
                         double& cgcs_lat, double& cgcs_lon) {
        // 方法1：直接转换（适用于一般应用）
        // WGS-84与CGCS2000差异在厘米级，一般应用可直接使用
        
        // 方法2：七参数转换（适用于高精度应用）
        // 需要当地转换参数，以下为中国区域近似参数
        SevenParameters params = GetChinaRegionParams();
        
        // 大地坐标转空间直角坐标
        double X_wgs, Y_wgs, Z_wgs;
        GeodeticToCartesian(wgs_lat, wgs_lon, 0, WGS84, X_wgs, Y_wgs, Z_wgs);
        
        // 七参数转换
        double X_cgcs, Y_cgcs, Z_cgcs;
        TransformBySevenParams(X_wgs, Y_wgs, Z_wgs, params, 
                              X_cgcs, Y_cgcs, Z_cgcs);
        
        // 空间直角坐标转大地坐标
        CartesianToGeodetic(X_cgcs, Y_cgcs, Z_cgcs, CGCS2000, 
                           cgcs_lat, cgcs_lon);
    }
    
private:
    SevenParameters GetChinaRegionParams() {
        // 中国区域WGS-84转CGCS2000近似七参数
        // 实际应用应使用当地测绘部门提供的精确参数
        SevenParameters params;
        params.dx = 0.0;        // X平移
        params.dy = 0.0;        // Y平移
        params.dz = 0.0;        // Z平移
        params.rx = 0.0;        // X旋转
        params.ry = 0.0;        // Y旋转
        params.rz = 0.0;        // Z旋转
        params.scale = 0.0;     // 尺度因子
        return params;
    }
    
    void GeodeticToCartesian(double lat, double lon, double h,
                            const EllipsoidParams& ellip,
                            double& X, double& Y, double& Z) {
        double lat_rad = lat * M_PI / 180.0;
        double lon_rad = lon * M_PI / 180.0;
        
        double sin_lat = std::sin(lat_rad);
        double cos_lat = std::cos(lat_rad);
        double sin_lon = std::sin(lon_rad);
        double cos_lon = std::cos(lon_rad);
        
        // 卯酉圈曲率半径
        double N = ellip.a / std::sqrt(1 - ellip.e2 * sin_lat * sin_lat);
        
        X = (N + h) * cos_lat * cos_lon;
        Y = (N + h) * cos_lat * sin_lon;
        Z = (N * (1 - ellip.e2) + h) * sin_lat;
    }
    
    void CartesianToGeodetic(double X, double Y, double Z,
                            const EllipsoidParams& ellip,
                            double& lat, double& lon) {
        // 迭代法计算大地坐标
        lon = std::atan2(Y, X) * 180.0 / M_PI;
        
        double p = std::sqrt(X * X + Y * Y);
        double lat0 = std::atan2(Z, p);
        double lat_rad = lat0;
        
        // 迭代计算纬度
        for (int i = 0; i < 10; ++i) {
            double sin_lat = std::sin(lat_rad);
            double N = ellip.a / std::sqrt(1 - ellip.e2 * sin_lat * sin_lat);
            double lat_new = std::atan2(Z + ellip.e2 * N * sin_lat, p);
            
            if (std::abs(lat_new - lat_rad) < 1e-12) {
                break;
            }
            lat_rad = lat_new;
        }
        
        lat = lat_rad * 180.0 / M_PI;
    }
    
    void TransformBySevenParams(double X_in, double Y_in, double Z_in,
                               const SevenParameters& params,
                               double& X_out, double& Y_out, double& Z_out) {
        // 七参数转换公式
        X_out = params.dx + (1 + params.scale) * 
                (X_in + params.rz * Y_in - params.ry * Z_in);
        Y_out = params.dy + (1 + params.scale) * 
                (-params.rz * X_in + Y_in + params.rx * Z_in);
        Z_out = params.dz + (1 + params.scale) * 
                (params.ry * X_in - params.rx * Y_in + Z_in);
    }
};
```

**大圆距离计算详细说明**:

大圆距离是球面上两点间的最短路径距离，用于远洋航行航线计算。

```cpp
class GreatCircleCalculator {
public:
    static constexpr double EARTH_RADIUS = 6371.0;  // 地球平均半径 (km)
    static constexpr double NAUTICAL_MILE = 1.852;  // 1海里 = 1.852 km
    
    // 大圆距离计算 (Haversine公式)
    double CalculateGreatCircleDistance(double lat1, double lon1,
                                        double lat2, double lon2) {
        // 转换为弧度
        double phi1 = lat1 * M_PI / 180.0;
        double phi2 = lat2 * M_PI / 180.0;
        double delta_phi = (lat2 - lat1) * M_PI / 180.0;
        double delta_lambda = (lon2 - lon1) * M_PI / 180.0;
        
        // Haversine公式
        double a = std::sin(delta_phi / 2) * std::sin(delta_phi / 2) +
                   std::cos(phi1) * std::cos(phi2) *
                   std::sin(delta_lambda / 2) * std::sin(delta_lambda / 2);
        
        double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
        
        double distance_km = EARTH_RADIUS * c;
        return distance_km / NAUTICAL_MILE;  // 返回海里
    }
    
    // 大圆航线初始方位角
    double CalculateInitialBearing(double lat1, double lon1,
                                   double lat2, double lon2) {
        double phi1 = lat1 * M_PI / 180.0;
        double phi2 = lat2 * M_PI / 180.0;
        double delta_lambda = (lon2 - lon1) * M_PI / 180.0;
        
        double y = std::sin(delta_lambda) * std::cos(phi2);
        double x = std::cos(phi1) * std::sin(phi2) -
                   std::sin(phi1) * std::cos(phi2) * std::cos(delta_lambda);
        
        double bearing = std::atan2(y, x) * 180.0 / M_PI;
        
        // 转换为0-360度
        return std::fmod(bearing + 360.0, 360.0);
    }
    
    // 大圆航线中间点计算
    struct GreatCircleWaypoint {
        double latitude;
        double longitude;
        double distance_from_start;
        double bearing;
    };
    
    std::vector<GreatCircleWaypoint> CalculateGreatCircleRoute(
        double lat1, double lon1, double lat2, double lon2,
        double interval_nm = 60.0) {
        
        std::vector<GreatCircleWaypoint> waypoints;
        
        double total_distance = CalculateGreatCircleDistance(
            lat1, lon1, lat2, lon2);
        
        int num_points = static_cast<int>(total_distance / interval_nm);
        
        double phi1 = lat1 * M_PI / 180.0;
        double lambda1 = lon1 * M_PI / 180.0;
        double phi2 = lat2 * M_PI / 180.0;
        double lambda2 = lon2 * M_PI / 180.0;
        
        // 起点加入
        waypoints.push_back({lat1, lon1, 0, 
            CalculateInitialBearing(lat1, lon1, lat2, lon2)});
        
        for (int i = 1; i <= num_points; ++i) {
            double fraction = static_cast<double>(i) / (num_points + 1);
            
            // 球面插值计算中间点
            double A = std::sin((1 - fraction) * total_distance * 
                       NAUTICAL_MILE / EARTH_RADIUS) / 
                       std::sin(total_distance * NAUTICAL_MILE / EARTH_RADIUS);
            double B = std::sin(fraction * total_distance * 
                       NAUTICAL_MILE / EARTH_RADIUS) / 
                       std::sin(total_distance * NAUTICAL_MILE / EARTH_RADIUS);
            
            double x = A * std::cos(phi1) * std::cos(lambda1) +
                       B * std::cos(phi2) * std::cos(lambda2);
            double y = A * std::cos(phi1) * std::sin(lambda1) +
                       B * std::cos(phi2) * std::sin(lambda2);
            double z = A * std::sin(phi1) + B * std::sin(phi2);
            
            double phi = std::atan2(z, std::sqrt(x * x + y * y));
            double lambda = std::atan2(y, x);
            
            double waypoint_lat = phi * 180.0 / M_PI;
            double waypoint_lon = lambda * 180.0 / M_PI;
            
            waypoints.push_back({
                waypoint_lat,
                waypoint_lon,
                fraction * total_distance,
                CalculateInitialBearing(waypoint_lat, waypoint_lon, lat2, lon2)
            });
        }
        
        // 终点加入
        waypoints.push_back({lat2, lon2, total_distance, 
            CalculateInitialBearing(lat2, lon2, lat2, lon2)});
        
        return waypoints;
    }
    
    // 恒向线距离 (Rhumb Line)
    double CalculateRhumbLineDistance(double lat1, double lon1,
                                      double lat2, double lon2) {
        double phi1 = lat1 * M_PI / 180.0;
        double phi2 = lat2 * M_PI / 180.0;
        double delta_phi = phi2 - phi1;
        double delta_lambda = (lon2 - lon1) * M_PI / 180.0;
        
        // 处理跨越180度经线
        if (std::abs(delta_lambda) > M_PI) {
            delta_lambda = delta_lambda > 0 ? 
                           -(2 * M_PI - std::abs(delta_lambda)) :
                           (2 * M_PI - std::abs(delta_lambda));
        }
        
        // 等角航线的纬度修正
        double delta_psi = std::log(std::tan(M_PI / 4 + phi2 / 2) /
                                    std::tan(M_PI / 4 + phi1 / 2));
        
        double q = std::abs(delta_psi) > 1e-12 ? 
                   delta_phi / delta_psi : std::cos(phi1);
        
        double distance = std::sqrt(delta_phi * delta_phi + 
                                   q * q * delta_lambda * delta_lambda) * 
                          EARTH_RADIUS;
        
        return distance / NAUTICAL_MILE;
    }
    
    // 比较大圆航线与恒向线
    struct RouteComparison {
        double great_circle_distance;
        double rhumb_line_distance;
        double saving_distance;
        double saving_percentage;
        double initial_bearing_gc;
        double initial_bearing_rl;
    };
    
    RouteComparison CompareRoutes(double lat1, double lon1,
                                  double lat2, double lon2) {
        RouteComparison result;
        result.great_circle_distance = CalculateGreatCircleDistance(
            lat1, lon1, lat2, lon2);
        result.rhumb_line_distance = CalculateRhumbLineDistance(
            lat1, lon1, lat2, lon2);
        result.saving_distance = result.rhumb_line_distance - 
                                 result.great_circle_distance;
        result.saving_percentage = (result.saving_distance / 
                                   result.rhumb_line_distance) * 100;
        result.initial_bearing_gc = CalculateInitialBearing(
            lat1, lon1, lat2, lon2);
        result.initial_bearing_rl = CalculateRhumbLineBearing(
            lat1, lon1, lat2, lon2);
        return result;
    }
    
private:
    double CalculateRhumbLineBearing(double lat1, double lon1,
                                     double lat2, double lon2) {
        double phi1 = lat1 * M_PI / 180.0;
        double phi2 = lat2 * M_PI / 180.0;
        double delta_lambda = (lon2 - lon1) * M_PI / 180.0;
        
        if (std::abs(delta_lambda) > M_PI) {
            delta_lambda = delta_lambda > 0 ? 
                           -(2 * M_PI - std::abs(delta_lambda)) :
                           (2 * M_PI - std::abs(delta_lambda));
        }
        
        double delta_psi = std::log(std::tan(M_PI / 4 + phi2 / 2) /
                                    std::tan(M_PI / 4 + phi1 / 2));
        
        double bearing = std::atan2(delta_lambda, delta_psi) * 180.0 / M_PI;
        return std::fmod(bearing + 360.0, 360.0);
    }
};
```

**PositionManager - 定位管理器**:
```cpp
class PositionManager {
public:
    static PositionManager& Instance();
    
    bool Initialize(const PositionConfig& config);
    void Shutdown();
    
    void SetUpdateFrequency(int seconds);
    void SetPositionSource(PositionSource source);
    
    PositionData GetCurrentPosition() const;
    PositionQuality GetPositionQuality() const;
    
    void SetPositionCallback(PositionCallback callback);
    void SetQualityChangeCallback(QualityChangeCallback callback);
    
private:
    PositionManager();
    ~PositionManager();
    
    void OnPositionReceived(const PositionData& data);
    void AdjustUpdateFrequency(const PositionData& data);
    
    std::unique_ptr<IPositionProvider> provider_;
    PositionFilter filter_;
    PositionData current_position_;
    int update_frequency_;
    PositionSource current_source_;
    PositionCallback position_callback_;
    QualityChangeCallback quality_callback_;
};
```

#### 3.1.3 定位更新策略实现

```cpp
class UpdateStrategy {
public:
    virtual int CalculateUpdateInterval(const PositionData& data,
                                        const UserLevel& level) = 0;
};

class VipUpdateStrategy : public UpdateStrategy {
public:
    int CalculateUpdateInterval(const PositionData& data,
                                const UserLevel& level) override {
        if (data.speed < 0.5) {
            return 60;
        }
        switch (level) {
            case UserLevel::Normal: return 30;
            case UserLevel::Vip: return 10;
            case UserLevel::Advanced: return 1;
            default: return 30;
        }
    }
};
```

### 3.2 航线模块设计

#### 3.2.1 模块结构

```
RouteModule/
├── include/
│   ├── route/
│   │   ├── route_types.h             # 航线数据类型定义
│   │   ├── waypoint.h                # 航路点定义
│   │   ├── route.h                   # 航线定义
│   │   ├── route_planner.h           # 航线规划器
│   │   ├── route_manager.h           # 航线管理器
│   │   ├── ukc_calculator.h          # UKC计算器
│   │   └── route_optimizer.h         # 航线优化器
│   └── route_export.h                # 导出宏定义
├── src/
│   ├── waypoint.cpp                  # 航路点实现
│   ├── route.cpp                     # 航线实现
│   ├── route_planner.cpp             # 航线规划实现
│   ├── route_manager.cpp             # 航线管理实现
│   ├── ukc_calculator.cpp            # UKC计算实现
│   └── route_optimizer.cpp           # 航线优化实现
└── tests/
    ├── test_route_planner.cpp
    ├── test_ukc_calculator.cpp
    └── test_route_optimizer.cpp
```

#### 3.2.2 核心类设计

**Waypoint - 航路点**:
```cpp
struct Waypoint {
    std::string id;              // 航路点ID
    std::string name;            // 航路点名称
    double longitude;            // 经度
    double latitude;             // 纬度
    WaypointType type;           // 航路点类型
    double turn_radius;          // 转弯半径 (米)
    double xtd_limit;            // 偏航限制 (米)
    std::string notes;           // 备注
    
    // 计算属性
    double distance_from_prev;   // 距上一航点距离
    double bearing_from_prev;    // 从上一航点方位
    double time_to_next;         // 到下一航点时间
};

enum class WaypointType {
    Turn,          // 转向点
    Report,        // 报告点
    Check,         // 检查点
    Anchor         // 锚泊点
};
```

**Route - 航线**:
```cpp
class Route {
public:
    Route();
    explicit Route(const std::string& name);
    
    // 航路点管理
    void AddWaypoint(const Waypoint& waypoint);
    void InsertWaypoint(size_t index, const Waypoint& waypoint);
    void RemoveWaypoint(size_t index);
    void UpdateWaypoint(size_t index, const Waypoint& waypoint);
    
    Waypoint GetWaypoint(size_t index) const;
    size_t GetWaypointCount() const;
    std::vector<Waypoint> GetAllWaypoints() const;
    
    // 航线属性
    std::string GetName() const;
    void SetName(const std::string& name);
    
    double GetTotalDistance() const;
    double GetTotalTime(double speed) const;
    double GetEstimatedFuel(double consumption_rate) const;
    
    // 航线操作
    Route Split(size_t index) const;
    static Route Merge(const Route& route1, const Route& route2);
    Route Reverse() const;
    
    // 航线验证
    bool Validate() const;
    std::vector<std::string> GetValidationErrors() const;
    
    // 序列化
    std::string ToJson() const;
    static Route FromJson(const std::string& json);
    
    // 导出
    bool ExportToGpx(const std::string& filepath) const;
    bool ExportToKml(const std::string& filepath) const;
    static Route ImportFromGpx(const std::string& filepath);
    static Route ImportFromKml(const std::string& filepath);
    
private:
    std::string name_;
    std::vector<Waypoint> waypoints_;
    std::string departure_port_;
    std::string arrival_port_;
    std::string notes_;
    double created_time_;
    double modified_time_;
    
    void RecalculateDistances();
};
```

**RoutePlanner - 航线规划器**:
```cpp
struct PlanningConstraints {
    double min_depth;           // 最小水深 (米)
    double min_width;           // 最小航道宽度 (米)
    double max_height;          // 最大高度限制 (米)
    double min_turn_radius;     // 最小转弯半径 (米)
    std::vector<Polygon> restricted_areas;  // 禁航区
    std::vector<Polygon> caution_areas;     // 警戒区
};

struct PlanningFactors {
    // 静态因素
    std::vector<DepthData> depth_data;
    std::vector<ChannelData> channel_data;
    std::vector<BridgeData> bridge_data;
    std::vector<AidToNavigation> aids;
    std::vector<Obstacle> obstacles;
    
    // 动态因素
    double current_water_level;
    WeatherCondition weather;
    std::vector<AisTarget> ais_targets;
    
    // 船舶参数
    ShipParameters ship_params;
};

class RoutePlanner {
public:
    RoutePlanner();
    
    void SetAlgorithm(PlanningAlgorithm algorithm);
    void SetConstraints(const PlanningConstraints& constraints);
    void SetFactors(const PlanningFactors& factors);
    
    Route Plan(const Coordinate& start, const Coordinate& end);
    Route PlanWithWaypoints(const std::vector<Coordinate>& waypoints);
    
    void SetProgressCallback(ProgressCallback callback);
    void Cancel();
    
private:
    Route PlanWithAStar(const Coordinate& start, const Coordinate& end);
    Route PlanWithRRT(const Coordinate& start, const Coordinate& end);
    Route PlanWithGenetic(const Coordinate& start, const Coordinate& end);
    
    bool IsConstraintSatisfied(const Coordinate& point);
    double CalculateCost(const Coordinate& from, const Coordinate& to);
    std::vector<Coordinate> GetNeighbors(const Coordinate& point);
    
    PlanningAlgorithm algorithm_;
    PlanningConstraints constraints_;
    PlanningFactors factors_;
    ProgressCallback progress_callback_;
    bool cancelled_;
};

enum class PlanningAlgorithm {
    AStar,       // A*算法
    RRT,         // RRT*算法
    Genetic,     // 遗传算法
    AntColony    // 蚁群算法
};
```

**UkcCalculator - UKC计算器**:
```cpp
struct UkcInput {
    // 船舶参数
    double ship_draft;        // 船舶吃水 (米)
    double ship_length;       // 船长 (米)
    double ship_beam;         // 船宽 (米)
    double block_coefficient; // 方形系数
    
    // 航行参数
    double speed;             // 船速 (节)
    double water_depth;       // 水深 (米)
    double water_level;       // 水位 (米)
    double turn_radius;       // 转弯半径 (米)
    
    // 环境参数
    double wave_height;       // 波高 (米)
    double wave_period;       // 波周期 (秒)
    double swell_height;      // 涌高 (米)
    double wind_speed;        // 风速 (米/秒)
    
    // 航道参数
    double channel_width;     // 航道宽度 (米)
    double channel_depth;     // 航道深度 (米)
    ChannelType channel_type; // 航道类型
};

struct UkcComponents {
    double static_ukc;              // 静态UKC
    double squat;                   // 下沉量
    double wave_response;           // 波浪响应
    double heel_effect;             // 横摇影响
    double trim_change;             // 纵倾变化
    double water_level_correction;  // 水位修正
    double measurement_error;       // 测量误差
    double total_ukc;               // 总UKC
    double safe_depth;              // 安全水深
};

class UkcCalculator {
public:
    UkcCalculator();
    
    void SetShipParameters(const ShipParameters& params);
    void SetChannelType(ChannelType type);
    
    UkcComponents Calculate(const UkcInput& input);
    UkcComponents Calculate(double water_depth, double ship_draft, 
                           double speed, double water_level = 0.0);
    
    bool IsSafe(double water_depth, double ship_draft, 
                double speed, double water_level = 0.0);
    
    double GetMinUkcRequirement() const;
    
private:
    double CalculateStaticUkc(ChannelType type);
    double CalculateSquat(const UkcInput& input);
    double CalculateWaveResponse(double wave_height, double wave_period, 
                                 double ship_length);
    double CalculateHeelEffect(const UkcInput& input);
    double CalculateTrimChange(const UkcInput& input);
    double CalculateMeasurementError(double water_depth);
    
    ShipParameters ship_params_;
    ChannelType channel_type_;
    
    // UKC标准参数
    std::map<ChannelType, double> static_ukc_standards_;
};

class UkcCalculator {
public:
    UkcComponents Calculate(const UkcInput& input) {
        UkcComponents ukc;
        
        // 1. 静态UKC (根据航道类型和船舶大小)
        ukc.static_ukc = CalculateStaticUkc(input.channel_type);
        
        // 2. 下沉量 (Squat) - 使用Barrass公式
        ukc.squat = CalculateSquat(input);
        
        // 3. 波浪响应
        ukc.wave_response = CalculateWaveResponse(
            input.wave_height, input.wave_period, input.ship_length);
        
        // 4. 横摇影响 (转弯时)
        ukc.heel_effect = CalculateHeelEffect(input);
        
        // 5. 纵倾变化
        ukc.trim_change = CalculateTrimChange(input);
        
        // 6. 水位修正
        ukc.water_level_correction = input.water_level;
        
        // 7. 测量误差
        ukc.measurement_error = CalculateMeasurementError(input.water_depth);
        
        // 8. 计算总UKC
        ukc.total_ukc = ukc.static_ukc + ukc.squat + ukc.wave_response +
                        ukc.heel_effect + ukc.trim_change + 
                        ukc.measurement_error - ukc.water_level_correction;
        
        // 9. 计算安全水深
        ukc.safe_depth = input.ship_draft + ukc.total_ukc;
        
        return ukc;
    }
    
private:
    double CalculateStaticUkc(ChannelType type) {
        // 根据IMO和IHO标准
        switch (type) {
            case ChannelType::OpenSea:
                return 2.0;  // 外海航道
            case ChannelType::Harbor:
                return 1.5;  // 港口航道
            case ChannelType::Inland:
                return 1.0;  // 内河航道
            case ChannelType::Restricted:
                return 0.5;  // 受限水域
            default:
                return 1.5;
        }
    }
    
    double CalculateSquat(const UkcInput& input) {
        // Barrass公式: Squat = Cb * V^2 / 100 (米)
        double cb = input.block_coefficient;
        double v = input.speed;
        double h = input.water_depth;
        double t = input.ship_draft;
        
        // 基础下沉量
        double squat = cb * v * v / 100.0;
        
        // 浅水效应修正 (Froude深度数)
        double depth_froude = v * 0.5144 / std::sqrt(9.81 * h);
        if (depth_froude > 0.7) {
            squat *= (1.0 + (depth_froude - 0.7) * 2.0);
        }
        
        // 航道宽度修正
        if (input.channel_width > 0) {
            double blockage = (input.ship_beam * t) / 
                             (input.channel_width * h);
            if (blockage > 0.1) {
                squat *= (1.0 + blockage);
            }
        }
        
        return squat;
    }
    
    double CalculateWaveResponse(double wave_height, double wave_period,
                                 double ship_length) {
        if (wave_height <= 0) return 0;
        
        // 波浪引起的垂荡
        double wave_response = wave_height * 0.5;
        
        // 波长与船长比的影响
        double wave_length = 1.56 * wave_period * wave_period;
        double length_ratio = wave_length / ship_length;
        
        if (length_ratio > 0.8 && length_ratio < 1.2) {
            // 谐振区域，影响加倍
            wave_response *= 2.0;
        }
        
        return wave_response;
    }
    
    double CalculateHeelEffect(const UkcInput& input) {
        if (input.turn_radius <= 0) return 0;
        
        // 转弯时的横倾角
        double v = input.speed * 0.5144;  // 转换为m/s
        double heel_angle = std::atan(v * v / (9.81 * input.turn_radius));
        
        // 横倾引起的吃水增加
        double heel_effect = input.ship_beam / 2.0 * std::sin(heel_angle);
        
        return heel_effect;
    }
    
    double CalculateTrimChange(const UkcInput& input) {
        // 加减速引起的纵倾变化
        // 简化计算，实际应根据船型详细计算
        return input.speed * 0.01;
    }
    
    double CalculateMeasurementError(double water_depth) {
        // 测量误差通常为水深的1-2%
        return water_depth * 0.02;
    }
};

enum class ChannelType {
    OpenSea,      // 外海航道
    Harbor,       // 港口航道
    Inland,       // 内河航道
    Restricted    // 受限水域
};
```

**RouteManager - 航线管理器**:
```cpp
class RouteManager {
public:
    static RouteManager& Instance();
    
    bool Initialize(const std::string& db_path);
    void Shutdown();
    
    // 航线CRUD操作
    std::string CreateRoute(const Route& route);
    bool UpdateRoute(const std::string& route_id, const Route& route);
    bool DeleteRoute(const std::string& route_id);
    Route GetRoute(const std::string& route_id) const;
    std::vector<Route> GetAllRoutes() const;
    std::vector<Route> SearchRoutes(const std::string& keyword) const;
    
    // 航线模板
    std::string SaveAsTemplate(const Route& route, const std::string& name);
    Route LoadTemplate(const std::string& template_id) const;
    std::vector<RouteTemplate> GetAllTemplates() const;
    
    // 历史航线
    std::vector<Route> GetHistoryRoutes(int limit = 100) const;
    bool SaveToHistory(const Route& route);
    
private:
    RouteManager();
    ~RouteManager();
    
    std::unique_ptr<RouteDatabase> database_;
    std::map<std::string, Route> route_cache_;
};
```

#### 3.2.3 航线规划算法实现

**A*算法实现**:
```cpp
class AStarPlanner {
public:
    struct Node {
        Coordinate coord;
        double g_cost;  // 从起点到当前节点代价
        double h_cost;  // 从当前节点到终点启发代价
        double f_cost;  // g + h
        Node* parent;
        
        bool operator>(const Node& other) const {
            return f_cost > other.f_cost;
        }
    };
    
    void SetChartDatabase(IChartDatabase* chart_db) {
        chart_db_ = chart_db;
    }
    
    void SetShipParameters(double draft, double min_ukc) {
        ship_draft_ = draft;
        min_ukc_ = min_ukc;
    }
    
    Route FindPath(const Coordinate& start, const Coordinate& end,
                   const PlanningConstraints& constraints,
                   const PlanningFactors& factors) {
        std::priority_queue<Node, std::vector<Node>, std::greater<Node>> open_set;
        std::unordered_set<Coordinate> closed_set;
        std::unordered_map<Coordinate, Node> all_nodes;
        
        Node start_node{start, 0, Heuristic(start, end), 0, nullptr};
        open_set.push(start_node);
        all_nodes[start] = start_node;
        
        while (!open_set.empty()) {
            Node current = open_set.top();
            open_set.pop();
            
            if (IsGoal(current.coord, end)) {
                return ReconstructPath(current);
            }
            
            closed_set.insert(current.coord);
            
            for (const auto& neighbor : GetNeighbors(current.coord)) {
                if (closed_set.count(neighbor)) continue;
                if (!constraints.IsSatisfied(neighbor, factors)) continue;
                if (!IsNavigable(neighbor)) continue;
                
                double tentative_g = current.g_cost + 
                    Distance(current.coord, neighbor);
                
                if (!all_nodes.count(neighbor) || 
                    tentative_g < all_nodes[neighbor].g_cost) {
                    Node neighbor_node{
                        neighbor,
                        tentative_g,
                        Heuristic(neighbor, end),
                        tentative_g + Heuristic(neighbor, end),
                        &all_nodes[current.coord]
                    };
                    all_nodes[neighbor] = neighbor_node;
                    open_set.push(neighbor_node);
                }
            }
        }
        
        return Route();
    }
    
private:
    double Heuristic(const Coordinate& from, const Coordinate& to) {
        return CoordinateConverter::Instance().CalculateGreatCircleDistance(
            from.latitude, from.longitude,
            to.latitude, to.longitude);
    }
    
    std::vector<Coordinate> GetNeighbors(const Coordinate& coord) {
        std::vector<Coordinate> neighbors;
        
        if (!chart_db_) {
            return GenerateDefaultNeighbors(coord);
        }
        
        ChartData chart_data = chart_db_->Query(coord);
        double safe_depth = ship_draft_ + min_ukc_;
        
        for (int angle = 0; angle < 360; angle += 45) {
            Coordinate neighbor = CalculateDestination(coord, angle, step_);
            
            double depth = chart_data.GetDepth(neighbor);
            if (depth >= safe_depth && !chart_data.HasObstacle(neighbor)) {
                if (chart_data.IsInChannel(neighbor) || 
                    !constraints_.require_channel) {
                    neighbors.push_back(neighbor);
                }
            }
        }
        
        return neighbors;
    }
    
    bool IsNavigable(const Coordinate& coord) {
        if (!chart_db_) return true;
        
        ChartData chart_data = chart_db_->Query(coord);
        double depth = chart_data.GetDepth(coord);
        double safe_depth = ship_draft_ + min_ukc_;
        
        return depth >= safe_depth && 
               !chart_data.HasObstacle(coord) &&
               !chart_data.IsRestrictedArea(coord);
    }
    
    std::vector<Coordinate> GenerateDefaultNeighbors(const Coordinate& coord) {
        std::vector<Coordinate> neighbors;
        double step = 0.001;
        
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;
                neighbors.emplace_back(
                    coord.longitude + dx * step,
                    coord.latitude + dy * step
                );
            }
        }
        
        return neighbors;
    }
    
    Route ReconstructPath(const Node& goal_node) {
        Route route;
        std::vector<Coordinate> path;
        
        const Node* current = &goal_node;
        while (current != nullptr) {
            path.push_back(current->coord);
            current = current->parent;
        }
        
        std::reverse(path.begin(), path.end());
        
        for (const auto& coord : path) {
            Waypoint wp;
            wp.longitude = coord.longitude;
            wp.latitude = coord.latitude;
            route.AddWaypoint(wp);
        }
        
        return route;
    }
    
    IChartDatabase* chart_db_ = nullptr;
    double ship_draft_ = 5.0;
    double min_ukc_ = 1.0;
    double step_ = 0.001;
    PlanningConstraints constraints_;
};

class IChartDatabase {
public:
    virtual ~IChartDatabase() = default;
    
    virtual ChartData Query(const Coordinate& coord) = 0;
    virtual std::vector<Coordinate> QueryChannelCenterline(
        const Coordinate& start, const Coordinate& end) = 0;
    virtual std::vector<Obstacle> QueryObstacles(
        const Envelope& bounds) = 0;
    virtual std::vector<DepthArea> QueryDepthAreas(
        const Envelope& bounds, double min_depth) = 0;
};

struct ChartData {
    double depth;
    bool has_obstacle;
    bool is_in_channel;
    bool is_restricted;
    std::string channel_name;
    
    double GetDepth(const Coordinate& coord) const;
    bool HasObstacle(const Coordinate& coord) const;
    bool IsInChannel(const Coordinate& coord) const;
    bool IsRestrictedArea(const Coordinate& coord) const;
};
```

### 3.3 导航模块设计

#### 3.3.1 模块结构

```
NavigationModule/
├── include/
│   ├── navigation/
│   │   ├── navigation_types.h        # 导航数据类型定义
│   │   ├── navigation_engine.h       # 导航引擎
│   │   ├── cross_track_error.h       # 偏航计算
│   │   ├── waypoint_approach.h       # 航点接近检测
│   │   ├── navigation_calculator.h   # 导航计算
│   │   └── navigation_alerter.h      # 导航告警
│   └── navigation_export.h           # 导出宏定义
├── src/
│   ├── navigation_engine.cpp         # 导航引擎实现
│   ├── cross_track_error.cpp         # 偏航计算实现
│   ├── waypoint_approach.cpp         # 航点接近实现
│   ├── navigation_calculator.cpp     # 导航计算实现
│   └── navigation_alerter.cpp        # 导航告警实现
└── tests/
    ├── test_navigation_engine.cpp
    ├── test_cross_track_error.cpp
    └── test_waypoint_approach.cpp
```

#### 3.3.2 核心类设计

**NavigationData - 导航数据结构**:
```cpp
struct NavigationData {
    // 当前状态
    PositionData current_position;     // 当前位置
    double current_leg_index;          // 当前航段索引
    
    // 航向航速
    double course_over_ground;         // 对地航向 (COG)
    double speed_over_ground;          // 对地速度 (SOG)
    double heading;                    // 船首向
    
    // 偏航信息
    double cross_track_error;          // 偏航距离 (米)
    XteDirection xte_direction;        // 偏航方向
    XteLevel xte_level;                // 偏航级别
    
    // 下一航点信息
    Waypoint next_waypoint;            // 下一航点
    double distance_to_waypoint;       // 到下一航点距离
    double bearing_to_waypoint;        // 到下一航点方位
    double time_to_waypoint;           // 到下一航点时间
    double eta;                        // 预计到达时间
    
    // 航线信息
    double distance_remaining;         // 剩余航程
    double time_remaining;             // 剩余时间
    
    // 告警状态
    std::vector<NavigationAlert> alerts;
};

enum class XteDirection {
    Port,       // 左偏
    Starboard   // 右偏
};

enum class XteLevel {
    Normal,     // 正常 (<50m)
    Caution,    // 注意 (50-100m)
    Warning,    // 警告 (100-200m)
    Danger      // 危险 (>200m)
};

struct NavigationAlert {
    AlertType type;
    AlertLevel level;
    std::string message;
    double timestamp;
};

enum class AlertType {
    XteWarning,           // 偏航警告
    WaypointApproach,     // 航点接近
    WaypointPassed,       // 航点通过
    OffCourse,            // 偏离航线
    SpeedWarning,         // 速度警告
    DepthWarning          // 水深警告
};

enum class AlertLevel {
    Info,
    Caution,
    Warning,
    Danger
};
```

**CrossTrackError - 偏航计算**:
```cpp
class CrossTrackError {
public:
    CrossTrackError();
    
    void SetXteLimits(double caution, double warning, double danger);
    
    double Calculate(const Coordinate& current,
                    const Coordinate& leg_start,
                    const Coordinate& leg_end);
    
    XteDirection GetDirection(const Coordinate& current,
                             const Coordinate& leg_start,
                             const Coordinate& leg_end);
    
    XteLevel GetLevel(double xte);
    
private:
    double caution_limit_;
    double warning_limit_;
    double danger_limit_;
};
```

**WaypointApproach - 航点接近检测**:
```cpp
class WaypointApproach {
public:
    WaypointApproach();
    
    void SetApproachRadius(double radius_nm);
    void SetArrivalRadius(double radius_nm);
    
    ApproachStatus Check(const Coordinate& current,
                        const Waypoint& waypoint,
                        double speed);
    
private:
    double approach_radius_;
    double arrival_radius_;
};

enum class ApproachStatus {
    NotApproaching,    // 未接近
    Approaching,       // 接近中 (<1海里)
    PrepareTurn,       // 准备转向 (<0.5海里)
    Arrived            // 已到达
};
```

**NavigationCalculator - 导航计算**:
```cpp
class NavigationCalculator {
public:
    static NavigationCalculator& Instance();
    
    // 距离计算
    double CalculateDistance(const Coordinate& from, const Coordinate& to);
    double CalculateLegDistance(const Waypoint& from, const Waypoint& to);
    double CalculateRouteDistance(const Route& route, size_t from_index = 0);
    
    // 方位计算
    double CalculateBearing(const Coordinate& from, const Coordinate& to);
    double CalculateCourseToSteer(double current_bearing, double xte,
                                 XteDirection direction);
    
    // 时间计算
    double CalculateTime(double distance, double speed);
    double CalculateEta(double current_time, double distance, double speed);
    
    // 燃油计算
    double CalculateFuelConsumption(double time, double rate);
    
    // 转向计算
    double CalculateTurnRadius(double speed, double rudder_angle);
    double CalculateWheelOverPoint(const Coordinate& current,
                                   const Coordinate& next,
                                   double turn_radius);
};
```

**NavigationEngine - 导航引擎**:
```cpp
struct NavigationState {
    std::string route_id;           // 当前航线ID
    size_t leg_index;               // 当前航段索引
    bool is_navigating;             // 是否正在导航
    double navigation_start_time;   // 导航开始时间
    double last_position_time;      // 最后位置更新时间
    Coordinate last_position;       // 最后位置
    double total_distance_traveled; // 已行驶距离
};

class NavigationEngine {
public:
    static NavigationEngine& Instance();
    
    bool Initialize();
    void Shutdown();
    
    void SetRoute(const Route& route);
    void ClearRoute();
    Route GetCurrentRoute() const;
    
    void StartNavigation();
    void StopNavigation();
    bool IsNavigating() const;
    
    void UpdatePosition(const PositionData& position);
    NavigationData GetNavigationData() const;
    
    void SetNavigationCallback(NavigationCallback callback);
    void SetAlertCallback(AlertCallback callback);
    
    void SkipWaypoint();
    void ReverseRoute();
    
    // 状态持久化
    bool SaveState();
    bool RestoreState();
    bool HasSavedState() const;
    void ClearSavedState();
    
private:
    NavigationEngine();
    ~NavigationEngine();
    
    void UpdateNavigationData();
    void CheckAlerts();
    void AdvanceToNextLeg();
    void PersistState();
    
    Route current_route_;
    PositionData current_position_;
    NavigationData nav_data_;
    size_t current_leg_index_;
    bool is_navigating_;
    
    CrossTrackError xte_calculator_;
    WaypointApproach approach_detector_;
    NavigationCalculator calculator_;
    NavigationAlerter alerter_;
    
    NavigationCallback nav_callback_;
    AlertCallback alert_callback_;
    
    // 状态持久化
    std::unique_ptr<IStateDatabase> state_db_;
    NavigationState saved_state_;
    bool auto_save_enabled_;
    int auto_save_interval_;
};

class NavigationEngine {
public:
    bool SaveState() {
        if (!is_navigating_) {
            return false;
        }
        
        NavigationState state;
        state.route_id = current_route_.GetId();
        state.leg_index = current_leg_index_;
        state.is_navigating = is_navigating_;
        state.navigation_start_time = nav_data_.navigation_start_time;
        state.last_position_time = current_position_.timestamp;
        state.last_position = Coordinate(current_position_.longitude, 
                                         current_position_.latitude);
        state.total_distance_traveled = nav_data_.distance_traveled;
        
        // 保存到数据库
        bool success = state_db_->Save(state);
        if (success) {
            saved_state_ = state;
        }
        
        return success;
    }
    
    bool RestoreState() {
        NavigationState state;
        if (!state_db_->Load(state)) {
            return false;
        }
        
        // 恢复航线
        current_route_ = RouteManager::Instance().GetRoute(state.route_id);
        if (current_route_.GetWaypointCount() == 0) {
            return false;
        }
        
        // 恢复导航状态
        current_leg_index_ = state.leg_index;
        is_navigating_ = state.is_navigating;
        
        // 恢复位置
        current_position_.longitude = state.last_position.longitude;
        current_position_.latitude = state.last_position.latitude;
        current_position_.timestamp = state.last_position_time;
        
        // 恢复导航数据
        nav_data_.navigation_start_time = state.navigation_start_time;
        nav_data_.distance_traveled = state.total_distance_traveled;
        
        saved_state_ = state;
        
        return true;
    }
    
    void PersistState() {
        if (!auto_save_enabled_) {
            return;
        }
        
        // 自动保存策略：
        // 1. 航段变化时保存
        // 2. 定时间隔保存
        // 3. 偏航警告时保存
        
        static double last_save_time = 0;
        double current_time = GetCurrentTime();
        
        if (current_time - last_save_time >= auto_save_interval_) {
            SaveState();
            last_save_time = current_time;
        }
    }
    
private:
    void OnLegChanged() {
        // 航段变化时自动保存
        if (auto_save_enabled_) {
            SaveState();
        }
    }
    
    void OnXteWarning(double xte) {
        // 偏航警告时自动保存
        if (auto_save_enabled_ && xte > 100.0) {
            SaveState();
        }
    }
};

// 状态数据库接口
class IStateDatabase {
public:
    virtual ~IStateDatabase() = default;
    
    virtual bool Save(const NavigationState& state) = 0;
    virtual bool Load(NavigationState& state) = 0;
    virtual bool Clear() = 0;
    virtual bool Exists() = 0;
};

// SQLite实现
class SqliteStateDatabase : public IStateDatabase {
public:
    bool Initialize(const std::string& db_path) {
        // 创建状态表
        std::string sql = 
            "CREATE TABLE IF NOT EXISTS navigation_state ("
            "  id INTEGER PRIMARY KEY,"
            "  route_id TEXT,"
            "  leg_index INTEGER,"
            "  is_navigating INTEGER,"
            "  navigation_start_time REAL,"
            "  last_position_time REAL,"
            "  last_longitude REAL,"
            "  last_latitude REAL,"
            "  total_distance_traveled REAL,"
            "  saved_time REAL"
            ")";
        // 执行SQL...
        return true;
    }
    
    bool Save(const NavigationState& state) override {
        std::string sql = 
            "INSERT OR REPLACE INTO navigation_state "
            "(id, route_id, leg_index, is_navigating, navigation_start_time, "
            " last_position_time, last_longitude, last_latitude, "
            " total_distance_traveled, saved_time) "
            "VALUES (1, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
        // 执行SQL...
        return true;
    }
    
    bool Load(NavigationState& state) override {
        std::string sql = "SELECT * FROM navigation_state WHERE id = 1";
        // 执行查询并填充state...
        return true;
    }
};

using NavigationCallback = std::function<void(const NavigationData&)>;
using AlertCallback = std::function<void(const NavigationAlert&)>;
```

**NavigationAlerter - 导航告警器**:
```cpp
class NavigationAlerter {
public:
    NavigationAlerter();
    
    void SetXteThresholds(double caution, double warning, double danger);
    void SetWaypointAlertDistance(double distance_nm);
    
    std::vector<NavigationAlert> CheckAlerts(const NavigationData& nav_data);
    
    void SetSoundEnabled(bool enabled);
    void PlayAlertSound(AlertLevel level);
    
private:
    double xte_caution_;
    double xte_warning_;
    double xte_danger_;
    double waypoint_alert_distance_;
    bool sound_enabled_;
    
    std::map<AlertLevel, std::string> alert_sounds_;
};
```

### 3.4 轨迹模块设计

#### 3.4.1 模块结构

```
TrackModule/
├── include/
│   ├── track/
│   │   ├── track_types.h             # 轨迹数据类型定义
│   │   ├── track_point.h             # 轨迹点定义
│   │   ├── track.h                   # 轨迹定义
│   │   ├── track_recorder.h          # 轨迹记录器
│   │   ├── track_player.h            # 轨迹回放器
│   │   ├── track_analyzer.h          # 轨迹分析器
│   │   └── track_manager.h           # 轨迹管理器
│   └── track_export.h                # 导出宏定义
├── src/
│   ├── track_point.cpp               # 轨迹点实现
│   ├── track.cpp                     # 轨迹实现
│   ├── track_recorder.cpp            # 轨迹记录实现
│   ├── track_player.cpp              # 轨迹回放实现
│   ├── track_analyzer.cpp            # 轨迹分析实现
│   └── track_manager.cpp             # 轨迹管理实现
└── tests/
    ├── test_track_recorder.cpp
    ├── test_track_player.cpp
    └── test_track_analyzer.cpp
```

#### 3.4.2 核心类设计

**TrackPoint - 轨迹点**:
```cpp
struct TrackPoint {
    double timestamp;           // 时间戳
    double longitude;           // 经度
    double latitude;            // 纬度
    double heading;             // 航向 (度)
    double speed;               // 速度 (节)
    double depth;               // 水深 (米)
    int status;                 // 状态码
    
    // 序列化
    std::string ToJson() const;
    static TrackPoint FromJson(const std::string& json);
};
```

**Track - 轨迹**:
```cpp
class Track {
public:
    Track();
    explicit Track(const std::string& name);
    
    // 轨迹点管理
    void AddPoint(const TrackPoint& point);
    void AddPoints(const std::vector<TrackPoint>& points);
    TrackPoint GetPoint(size_t index) const;
    std::vector<TrackPoint> GetPoints() const;
    size_t GetPointCount() const;
    
    // 轨迹属性
    std::string GetName() const;
    void SetName(const std::string& name);
    
    double GetStartTime() const;
    double GetEndTime() const;
    double GetDuration() const;
    
    double GetTotalDistance() const;
    double GetAverageSpeed() const;
    double GetMaxSpeed() const;
    
    // 轨迹操作
    Track SubTrack(double start_time, double end_time) const;
    Track Simplify(double tolerance) const;
    
    // 序列化
    std::string ToJson() const;
    static Track FromJson(const std::string& json);
    
    // 导出
    bool ExportToGpx(const std::string& filepath) const;
    bool ExportToKml(const std::string& filepath) const;
    bool ExportToCsv(const std::string& filepath) const;
    static Track ImportFromGpx(const std::string& filepath);
    static Track ImportFromKml(const std::string& filepath);
    
private:
    std::string name_;
    std::vector<TrackPoint> points_;
    double start_time_;
    double end_time_;
    
    void UpdateStatistics();
};
```

**TrackRecorder - 轨迹记录器**:
```cpp
struct TrackRecorderConfig {
    int interval_seconds = 10;       // 记录间隔 (秒)
    double min_distance = 50.0;      // 最小距离 (米)
    bool enable_compression = true;  // 启用压缩
    int compression_interval = 100;  // 压缩间隔 (点数)
    double compression_tolerance = 10.0;  // 压缩容差 (米)
    int flush_interval = 1000;       // 刷新间隔 (点数)
    int flush_time_interval = 600;   // 刷新时间间隔 (秒)
};

class TrackRecorder {
public:
    TrackRecorder();
    
    void SetConfig(const TrackRecorderConfig& config);
    
    void Start(const std::string& track_name = "");
    void Stop();
    void Pause();
    void Resume();
    
    bool IsRecording() const;
    Track GetCurrentTrack() const;
    
    void SetRecordingInterval(int seconds);
    void SetMinDistance(double meters);
    
    void OnPositionReceived(const PositionData& position);
    
    RecorderStatistics GetStatistics() const;
    
private:
    bool ShouldRecord(const PositionData& position);
    bool ShouldCompress();
    void CompressTrack();
    bool ShouldFlush();
    void FlushToStorage();
    
    Track current_track_;
    bool is_recording_;
    bool is_paused_;
    TrackRecorderConfig config_;
    TrackPoint last_point_;
    double last_record_time_;
    double last_flush_time_;
    RecorderStatistics stats_;
};

struct RecorderStatistics {
    int total_points_received;
    int points_recorded;
    int points_compressed;
    int flush_count;
    double compression_ratio;
};

class TrackRecorder {
public:
    void OnPositionReceived(const PositionData& position) {
        if (!is_recording_ || is_paused_) {
            return;
        }
        
        stats_.total_points_received++;
        
        // 1. 检查是否应该记录
        if (!ShouldRecord(position)) {
            return;
        }
        
        // 2. 创建轨迹点
        TrackPoint point;
        point.timestamp = position.timestamp;
        point.longitude = position.longitude;
        point.latitude = position.latitude;
        point.heading = position.heading;
        point.speed = position.speed;
        point.depth = 0;  // 从海图获取
        point.status = static_cast<int>(position.quality);
        
        // 3. 添加到轨迹
        current_track_.AddPoint(point);
        stats_.points_recorded++;
        
        // 4. 检查是否需要压缩
        if (config_.enable_compression && ShouldCompress()) {
            CompressTrack();
        }
        
        // 5. 检查是否需要刷新到存储
        if (ShouldFlush()) {
            FlushToStorage();
        }
        
        last_point_ = point;
        last_record_time_ = position.timestamp;
    }
    
private:
    bool ShouldRecord(const PositionData& position) {
        // 时间间隔检查
        double time_diff = position.timestamp - last_record_time_;
        if (time_diff < config_.interval_seconds) {
            return false;
        }
        
        // 距离检查
        if (last_point_.timestamp > 0) {
            double dist = CoordinateConverter::Instance().CalculateGreatCircleDistance(
                last_point_.latitude, last_point_.longitude,
                position.latitude, position.longitude);
            if (dist * 1852.0 < config_.min_distance) {  // 转换为米
                return false;
            }
        }
        
        return true;
    }
    
    bool ShouldCompress() {
        return current_track_.GetPointCount() % config_.compression_interval == 0;
    }
    
    void CompressTrack() {
        int points_before = current_track_.GetPointCount();
        
        // 使用道格拉斯-普克算法压缩轨迹
        current_track_ = current_track_.Simplify(config_.compression_tolerance);
        
        int points_after = current_track_.GetPointCount();
        stats_.points_compressed += points_before - points_after;
        stats_.compression_ratio = 
            static_cast<double>(stats_.points_compressed) / stats_.points_recorded;
    }
    
    bool ShouldFlush() {
        // 点数检查
        if (current_track_.GetPointCount() >= config_.flush_interval) {
            return true;
        }
        
        // 时间检查
        double current_time = GetCurrentTime();
        if (current_time - last_flush_time_ >= config_.flush_time_interval) {
            return true;
        }
        
        return false;
    }
    
    void FlushToStorage() {
        // 异步写入数据库
        std::async(std::launch::async, [this]() {
            TrackManager::Instance().SaveTrack(current_track_);
        });
        
        stats_.flush_count++;
        last_flush_time_ = GetCurrentTime();
    }
};
```

**TrackPlayer - 轨迹回放器**:
```cpp
class TrackPlayer {
public:
    TrackPlayer();
    
    void LoadTrack(const Track& track);
    void UnloadTrack();
    
    void Play();
    void Pause();
    void Stop();
    void Seek(double timestamp);
    
    void SetSpeed(double multiplier);
    double GetSpeed() const;
    
    bool IsPlaying() const;
    double GetCurrentTime() const;
    double GetDuration() const;
    
    void SetPlaybackCallback(PlaybackCallback callback);
    
private:
    void PlaybackThread();
    
    Track track_;
    bool is_playing_;
    double current_time_;
    double speed_multiplier_;
    PlaybackCallback playback_callback_;
    std::thread playback_thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

using PlaybackCallback = std::function<void(const TrackPoint&, double progress)>;
```

**TrackAnalyzer - 轨迹分析器**:
```cpp
struct TrackStatistics {
    double total_distance;      // 总距离 (海里)
    double total_time;          // 总时间 (小时)
    double average_speed;       // 平均速度 (节)
    double max_speed;           // 最大速度 (节)
    double min_speed;           // 最小速度 (节)
    double average_heading;     // 平均航向
    double heading_variation;   // 航向变化
    int point_count;            // 轨迹点数
};

struct TrackSegment {
    double start_time;
    double end_time;
    double distance;
    double average_speed;
    std::string description;
};

class TrackAnalyzer {
public:
    TrackAnalyzer();
    
    TrackStatistics Analyze(const Track& track);
    std::vector<TrackSegment> SegmentBySpeed(const Track& track, 
                                             double threshold);
    std::vector<TrackSegment> SegmentByTime(const Track& track,
                                           double interval);
    
    // 轨迹比较
    double CalculateSimilarity(const Track& track1, const Track& track2);
    double CalculateDeviation(const Track& track, const Route& route);
    
    // 轨迹预测
    Track Predict(const Track& history, double duration);
};
```

**TrackManager - 轨迹管理器**:
```cpp
class TrackManager {
public:
    static TrackManager& Instance();
    
    bool Initialize(const std::string& db_path);
    void Shutdown();
    
    // 轨迹CRUD操作
    std::string SaveTrack(const Track& track);
    bool UpdateTrack(const std::string& track_id, const Track& track);
    bool DeleteTrack(const std::string& track_id);
    Track GetTrack(const std::string& track_id) const;
    std::vector<Track> GetAllTracks() const;
    std::vector<Track> GetTracksByTimeRange(double start, double end) const;
    
    // 轨迹搜索
    std::vector<Track> SearchTracks(const std::string& keyword) const;
    
    // 轨迹清理
    void CleanOldTracks(int days_to_keep = 365);
    void CleanAllTracks();
    
private:
    TrackManager();
    ~TrackManager();
    
    std::unique_ptr<TrackDatabase> database_;
    std::map<std::string, Track> track_cache_;
};
```

**TrackSharingService - 轨迹分享服务**:
```cpp
struct ShareConfig {
    bool anonymize_position = true;
    double position_precision = 0.001;
    bool include_speed = true;
    bool include_depth = false;
    bool include_heading = true;
    int expiry_hours = 24;
    bool require_password = false;
    std::string password;
};

struct SharedTrack {
    std::string share_id;
    std::string track_id;
    std::string owner_id;
    std::string share_code;
    double created_time;
    double expiry_time;
    int view_count;
    int download_count;
    ShareConfig config;
    std::vector<std::string> allowed_users;
    bool is_public;
};

struct ShareResult {
    bool success;
    std::string share_id;
    std::string share_url;
    std::string share_code;
    std::string error_message;
};

class TrackSharingService {
public:
    static TrackSharingService& Instance();
    
    bool Initialize(const std::string& server_url);
    void Shutdown();
    
    ShareResult ShareTrack(const Track& track, const ShareConfig& config);
    ShareResult ShareTrackById(const std::string& track_id, 
                               const ShareConfig& config);
    
    bool RevokeShare(const std::string& share_id);
    bool UpdateShareConfig(const std::string& share_id, 
                          const ShareConfig& config);
    
    Track AccessSharedTrack(const std::string& share_id, 
                           const std::string& access_code = "");
    Track AccessSharedTrackByCode(const std::string& share_code);
    
    std::vector<SharedTrack> GetMySharedTracks();
    std::vector<SharedTrack> GetPublicTracks(int page = 1, int page_size = 20);
    
    bool DownloadSharedTrack(const std::string& share_id, 
                            const std::string& filepath,
                            ExportFormat format);
    
    void SetShareCallback(ShareCallback callback);
    void SetAccessCallback(AccessCallback callback);
    
private:
    TrackSharingService();
    ~TrackSharingService();
    
    Track AnonymizeTrack(const Track& track, const ShareConfig& config);
    std::string GenerateShareCode();
    std::string GenerateShareId();
    bool ValidateAccess(const SharedTrack& share, 
                       const std::string& access_code);
    bool IsExpired(const SharedTrack& share);
    
    std::string server_url_;
    std::unique_ptr<HttpClient> http_client_;
    std::map<std::string, SharedTrack> share_cache_;
    ShareCallback share_callback_;
    AccessCallback access_callback_;
};

using ShareCallback = std::function<void(const ShareResult&)>;
using AccessCallback = std::function<void(const std::string& share_id)>;

class TrackSharingService {
public:
    ShareResult ShareTrack(const Track& track, const ShareConfig& config) {
        ShareResult result;
        
        Track track_to_share = track;
        if (config.anonymize_position) {
            track_to_share = AnonymizeTrack(track, config);
        }
        
        SharedTrack share;
        share.share_id = GenerateShareId();
        share.track_id = track.GetName();
        share.share_code = GenerateShareCode();
        share.created_time = GetCurrentTime();
        share.expiry_time = share.created_time + config.expiry_hours * 3600;
        share.config = config;
        share.is_public = config.require_password ? false : true;
        
        std::string json_data = track_to_share.ToJson();
        
        bool upload_success = UploadToServer(share, json_data);
        if (!upload_success) {
            result.success = false;
            result.error_message = "上传分享数据失败";
            return result;
        }
        
        share_cache_[share.share_id] = share;
        
        result.success = true;
        result.share_id = share.share_id;
        result.share_url = server_url_ + "/share/" + share.share_id;
        result.share_code = share.share_code;
        
        if (share_callback_) {
            share_callback_(result);
        }
        
        return result;
    }
    
    Track AccessSharedTrack(const std::string& share_id, 
                           const std::string& access_code) {
        auto it = share_cache_.find(share_id);
        SharedTrack share;
        
        if (it != share_cache_.end()) {
            share = it->second;
        } else {
            share = DownloadShareInfo(share_id);
            if (share.share_id.empty()) {
                return Track();
            }
            share_cache_[share_id] = share;
        }
        
        if (IsExpired(share)) {
            return Track();
        }
        
        if (!ValidateAccess(share, access_code)) {
            return Track();
        }
        
        Track track = DownloadTrackData(share_id);
        
        share.view_count++;
        share_cache_[share_id] = share;
        
        if (access_callback_) {
            access_callback_(share_id);
        }
        
        return track;
    }
    
private:
    Track AnonymizeTrack(const Track& track, const ShareConfig& config) {
        Track anonymized = track;
        auto points = track.GetPoints();
        
        for (auto& point : points) {
            if (config.position_precision > 0) {
                point.longitude = std::round(point.longitude / 
                    config.position_precision) * config.position_precision;
                point.latitude = std::round(point.latitude / 
                    config.position_precision) * config.position_precision;
            }
            
            if (!config.include_speed) {
                point.speed = 0;
            }
            if (!config.include_depth) {
                point.depth = 0;
            }
            if (!config.include_heading) {
                point.heading = 0;
            }
        }
        
        anonymized.AddPoints(points);
        return anonymized;
    }
    
    std::string GenerateShareCode() {
        std::string chars = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";
        std::uniform_int_distribution<int> dist(0, chars.size() - 1);
        
        std::string code;
        for (int i = 0; i < 6; ++i) {
            code += chars[dist(rng_)];
        }
        return code;
    }
    
    std::string GenerateShareId() {
        std::string chars = "abcdefghijklmnopqrstuvwxyz0123456789";
        std::uniform_int_distribution<int> dist(0, chars.size() - 1);
        
        std::string id;
        for (int i = 0; i < 16; ++i) {
            id += chars[dist(rng_)];
        }
        return id;
    }
    
    bool ValidateAccess(const SharedTrack& share, 
                       const std::string& access_code) {
        if (!share.config.require_password) {
            return true;
        }
        
        return access_code == share.config.password || 
               access_code == share.share_code;
    }
    
    bool IsExpired(const SharedTrack& share) {
        return GetCurrentTime() > share.expiry_time;
    }
    
    std::mt19937 rng_{std::random_device{}()};
};
```

**TrackImportService - 轨迹导入服务**:
```cpp
enum class ImportFormat {
    GPX,
    KML,
    KMZ,
    CSV,
    JSON,
    TCX,
    FIT,
    NMEA
};

struct ImportOptions {
    ImportFormat format = ImportFormat::GPX;
    bool validate_data = true;
    bool skip_invalid_points = true;
    bool merge_segments = false;
    bool apply_smoothing = false;
    double smoothing_factor = 0.5;
    std::string time_format = "ISO8601";
    std::string coordinate_system = "WGS84";
    int timezone_offset = 0;
    std::map<std::string, std::string> field_mapping;
};

struct ImportResult {
    bool success;
    Track track;
    int total_points;
    int valid_points;
    int skipped_points;
    std::vector<std::string> warnings;
    std::string error_message;
};

struct ValidationResult {
    bool is_valid;
    std::vector<ValidationError> errors;
    std::vector<std::string> warnings;
    double data_quality_score;
};

struct ValidationError {
    int line_number;
    std::string field;
    std::string message;
    std::string original_value;
};

class TrackImportService {
public:
    static TrackImportService& Instance();
    
    bool Initialize();
    void Shutdown();
    
    ImportResult ImportFromFile(const std::string& filepath,
                                const ImportOptions& options = ImportOptions());
    ImportResult ImportFromData(const std::string& data,
                                ImportFormat format,
                                const ImportOptions& options = ImportOptions());
    ImportResult ImportFromUrl(const std::string& url,
                               const ImportOptions& options = ImportOptions());
    
    bool IsFormatSupported(ImportFormat format) const;
    std::vector<ImportFormat> GetSupportedFormats() const;
    std::string GetFileExtension(ImportFormat format) const;
    ImportFormat DetectFormat(const std::string& filepath) const;
    
    ValidationResult ValidateFile(const std::string& filepath,
                                  ImportFormat format);
    ValidationResult ValidateData(const std::string& data,
                                 ImportFormat format);
    
    void SetProgressCallback(ImportProgressCallback callback);
    void SetCompletionCallback(ImportCompletionCallback callback);
    
private:
    TrackImportService();
    ~TrackImportService();
    
    ImportResult ImportGpx(const std::string& data, const ImportOptions& options);
    ImportResult ImportKml(const std::string& data, const ImportOptions& options);
    ImportResult ImportCsv(const std::string& data, const ImportOptions& options);
    ImportResult ImportJson(const std::string& data, const ImportOptions& options);
    ImportResult ImportTcx(const std::string& data, const ImportOptions& options);
    ImportResult ImportNmea(const std::string& data, const ImportOptions& options);
    
    bool ValidatePoint(const TrackPoint& point, std::vector<std::string>& warnings);
    Track ApplySmoothing(const Track& track, double factor);
    Track MergeSegments(const std::vector<Track>& segments);
    
    std::string ReadFileContent(const std::string& filepath);
    std::string DownloadFromUrl(const std::string& url);
    
    std::map<ImportFormat, std::unique_ptr<IFormatImporter>> importers_;
    ImportProgressCallback progress_callback_;
    ImportCompletionCallback completion_callback_;
};

using ImportProgressCallback = std::function<void(int current, int total, 
                                                  const std::string& status)>;
using ImportCompletionCallback = std::function<void(const ImportResult& result)>;

class IFormatImporter {
public:
    virtual ~IFormatImporter() = default;
    virtual ImportResult Import(const std::string& data, 
                               const ImportOptions& options) = 0;
    virtual ValidationResult Validate(const std::string& data) = 0;
    virtual std::vector<std::string> GetSupportedExtensions() = 0;
};

class GpxImporter : public IFormatImporter {
public:
    ImportResult Import(const std::string& data, 
                       const ImportOptions& options) override {
        ImportResult result;
        result.success = false;
        result.total_points = 0;
        result.valid_points = 0;
        result.skipped_points = 0;
        
        std::vector<TrackPoint> points;
        
        // 解析GPX XML
        // <trkpt lat="xx.xxx" lon="xx.xxx">
        //   <ele>xx</ele>
        //   <time>2024-01-01T00:00:00Z</time>
        //   <speed>xx</speed>
        //   <course>xx</course>
        // </trkpt>
        
        // 简化解析逻辑示例
        size_t pos = 0;
        while ((pos = data.find("<trkpt", pos)) != std::string::npos) {
            TrackPoint point;
            
            // 解析纬度
            size_t lat_pos = data.find("lat=\"", pos);
            if (lat_pos != std::string::npos) {
                size_t lat_end = data.find("\"", lat_pos + 5);
                point.latitude = std::stod(data.substr(lat_pos + 5, 
                    lat_end - lat_pos - 5));
            }
            
            // 解析经度
            size_t lon_pos = data.find("lon=\"", pos);
            if (lon_pos != std::string::npos) {
                size_t lon_end = data.find("\"", lon_pos + 5);
                point.longitude = std::stod(data.substr(lon_pos + 5, 
                    lon_end - lon_pos - 5));
            }
            
            // 解析时间戳
            size_t time_pos = data.find("<time>", pos);
            if (time_pos != std::string::npos) {
                size_t time_end = data.find("</time>", time_pos);
                std::string time_str = data.substr(time_pos + 6, 
                    time_end - time_pos - 6);
                point.timestamp = ParseIsoTime(time_str);
            }
            
            // 解析高度/深度
            size_t ele_pos = data.find("<ele>", pos);
            if (ele_pos != std::string::npos) {
                size_t ele_end = data.find("</ele>", ele_pos);
                point.depth = std::stod(data.substr(ele_pos + 5, 
                    ele_end - ele_pos - 5));
            }
            
            // 解析速度
            size_t speed_pos = data.find("<speed>", pos);
            if (speed_pos != std::string::npos) {
                size_t speed_end = data.find("</speed>", speed_pos);
                point.speed = std::stod(data.substr(speed_pos + 7, 
                    speed_end - speed_pos - 7));
            }
            
            // 解析航向
            size_t course_pos = data.find("<course>", pos);
            if (course_pos != std::string::npos) {
                size_t course_end = data.find("</course>", course_pos);
                point.heading = std::stod(data.substr(course_pos + 8, 
                    course_end - course_pos - 8));
            }
            
            result.total_points++;
            
            if (ValidatePoint(point, result.warnings)) {
                points.push_back(point);
                result.valid_points++;
            } else {
                result.skipped_points++;
            }
            
            pos = data.find("</trkpt>", pos) + 8;
        }
        
        if (!points.empty()) {
            result.track.AddPoints(points);
            result.success = true;
        }
        
        return result;
    }
    
    ValidationResult Validate(const std::string& data) override {
        ValidationResult result;
        result.is_valid = true;
        result.data_quality_score = 100.0;
        
        // 检查GPX格式有效性
        if (data.find("<gpx") == std::string::npos) {
            result.is_valid = false;
            result.errors.push_back({0, "format", "缺少GPX根元素", ""});
        }
        
        if (data.find("<trk>") == std::string::npos) {
            result.warnings.push_back("未找到轨迹数据");
            result.data_quality_score -= 20;
        }
        
        return result;
    }
    
    std::vector<std::string> GetSupportedExtensions() override {
        return {".gpx", ".GPX"};
    }
    
private:
    double ParseIsoTime(const std::string& time_str) {
        // ISO 8601格式: 2024-01-01T00:00:00Z
        // 转换为Unix时间戳
        // 简化实现
        return 0.0;
    }
    
    bool ValidatePoint(const TrackPoint& point, 
                      std::vector<std::string>& warnings) {
        if (point.latitude < -90 || point.latitude > 90) {
            warnings.push_back("纬度超出范围: " + 
                std::to_string(point.latitude));
            return false;
        }
        if (point.longitude < -180 || point.longitude > 180) {
            warnings.push_back("经度超出范围: " + 
                std::to_string(point.longitude));
            return false;
        }
        return true;
    }
};

class CsvImporter : public IFormatImporter {
public:
    ImportResult Import(const std::string& data,
                       const ImportOptions& options) override {
        ImportResult result;
        result.success = false;
        
        std::vector<TrackPoint> points;
        std::istringstream stream(data);
        std::string line;
        
        // 跳过标题行
        std::getline(stream, line);
        
        // 解析字段映射
        std::map<std::string, int> field_index;
        std::istringstream header_stream(line);
        std::string field;
        int index = 0;
        while (std::getline(header_stream, field, ',')) {
            field_index[field] = index++;
        }
        
        // 解析数据行
        while (std::getline(stream, line)) {
            std::vector<std::string> fields;
            std::istringstream line_stream(line);
            std::string value;
            
            while (std::getline(line_stream, value, ',')) {
                fields.push_back(value);
            }
            
            TrackPoint point;
            
            auto lat_it = field_index.find("latitude");
            if (lat_it != field_index.end() && lat_it->second < fields.size()) {
                point.latitude = std::stod(fields[lat_it->second]);
            }
            
            auto lon_it = field_index.find("longitude");
            if (lon_it != field_index.end() && lon_it->second < fields.size()) {
                point.longitude = std::stod(fields[lon_it->second]);
            }
            
            auto time_it = field_index.find("timestamp");
            if (time_it != field_index.end() && time_it->second < fields.size()) {
                point.timestamp = std::stod(fields[time_it->second]);
            }
            
            auto speed_it = field_index.find("speed");
            if (speed_it != field_index.end() && speed_it->second < fields.size()) {
                point.speed = std::stod(fields[speed_it->second]);
            }
            
            auto heading_it = field_index.find("heading");
            if (heading_it != field_index.end() && 
                heading_it->second < fields.size()) {
                point.heading = std::stod(fields[heading_it->second]);
            }
            
            result.total_points++;
            if (ValidatePoint(point, result.warnings)) {
                points.push_back(point);
                result.valid_points++;
            } else {
                result.skipped_points++;
            }
        }
        
        if (!points.empty()) {
            result.track.AddPoints(points);
            result.success = true;
        }
        
        return result;
    }
    
    ValidationResult Validate(const std::string& data) override {
        ValidationResult result;
        result.is_valid = true;
        
        // 检查CSV格式
        std::istringstream stream(data);
        std::string line;
        int line_count = 0;
        
        while (std::getline(stream, line)) {
            line_count++;
        }
        
        if (line_count < 2) {
            result.is_valid = false;
            result.errors.push_back({0, "format", "CSV文件至少需要标题行和数据行", ""});
        }
        
        return result;
    }
    
    std::vector<std::string> GetSupportedExtensions() override {
        return {".csv", ".CSV"};
    }
    
private:
    bool ValidatePoint(const TrackPoint& point,
                      std::vector<std::string>& warnings) {
        if (point.latitude < -90 || point.latitude > 90) {
            return false;
        }
        if (point.longitude < -180 || point.longitude > 180) {
            return false;
        }
        return true;
    }
};
```

### 3.5 AIS模块设计

#### 3.5.1 模块结构

```
AisModule/
├── include/
│   ├── ais/
│   │   ├── ais_types.h               # AIS数据类型定义
│   │   ├── ais_parser.h              # AIS解析器
│   │   ├── ais_target.h              # AIS目标
│   │   ├── ais_receiver.h            # AIS接收器
│   │   ├── collision_assessor.h      # 碰撞评估器
│   │   └── ais_manager.h             # AIS管理器
│   └── ais_export.h                  # 导出宏定义
├── src/
│   ├── ais_parser.cpp                # AIS解析实现
│   ├── ais_target.cpp                # AIS目标实现
│   ├── ais_receiver.cpp              # AIS接收实现
│   ├── collision_assessor.cpp        # 碰撞评估实现
│   └── ais_manager.cpp               # AIS管理实现
└── tests/
    ├── test_ais_parser.cpp
    ├── test_collision_assessor.cpp
    └── test_ais_manager.cpp
```

#### 3.5.2 核心类设计

**AisMessage - AIS消息**:
```cpp
struct AisMessage {
    int message_type;
    int repeat_indicator;
    int mmsi;
    double timestamp;
    
    // 消息类型特定字段
    std::map<std::string, std::variant<int, double, std::string>> fields;
};

struct AisPositionReport {
    int message_type;          // 1, 2, 3, 18, 19, 27
    int mmsi;
    double longitude;
    double latitude;
    double heading;            // 航向 (度)
    double speed;              // 速度 (节)
    double course;             // 对地航向
    int nav_status;            // 航行状态
    double timestamp;
};

struct AisStaticData {
    int message_type;          // 5
    int mmsi;
    std::string callsign;
    std::string ship_name;
    int ship_type;
    int dimension_a;
    int dimension_b;
    int dimension_c;
    int dimension_d;
    std::string destination;
    double timestamp;
};
```

**AisTarget - AIS目标**:
```cpp
class AisTarget {
public:
    AisTarget(int mmsi);
    
    int GetMmsi() const;
    std::string GetShipName() const;
    void SetShipName(const std::string& name);
    
    void UpdatePosition(const AisPositionReport& report);
    void UpdateStaticData(const AisStaticData& data);
    
    Coordinate GetPosition() const;
    double GetHeading() const;
    double GetSpeed() const;
    double GetCourse() const;
    
    double GetLastUpdate() const;
    bool IsExpired(double timeout_seconds = 60) const;
    
    // CPA/TCPA计算
    double CalculateCpa(const Coordinate& own_position,
                       double own_course, double own_speed);
    double CalculateTcpa(const Coordinate& own_position,
                        double own_course, double own_speed);
    
private:
    int mmsi_;
    std::string ship_name_;
    std::string callsign_;
    int ship_type_;
    
    Coordinate position_;
    double heading_;
    double speed_;
    double course_;
    int nav_status_;
    
    double last_update_;
};
```

**AisParser - AIS解析器**:
```cpp
class AisParser {
public:
    static AisParser& Instance();
    
    bool Parse(const std::string& ais_message, AisMessage& message);
    bool ParsePositionReport(const AisMessage& message, AisPositionReport& report);
    bool ParseStaticData(const AisMessage& message, AisStaticData& data);
    
private:
    AisParser() = default;
    
    std::vector<int> DecodePayload(const std::string& payload);
    int ExtractInt(const std::vector<int>& bits, int start, int len);
    std::string ExtractString(const std::vector<int>& bits, int start, int len);
    double ExtractCoord(const std::vector<int>& bits, int start, int len);
};
```

**CollisionAssessor - 碰撞评估器**:
```cpp
struct CollisionRisk {
    int target_mmsi;
    double cpa;                // 最近会遇距离 (海里)
    double tcpa;               // 到达CPA时间 (分钟)
    RiskLevel level;           // 风险等级
    std::string description;
};

struct MultiTargetRisk {
    std::vector<int> involved_targets;  // 涉及的目标MMSI列表
    double combined_cpa;                 // 综合CPA
    double min_tcpa;                     // 最小TCPA
    RiskLevel level;                     // 综合风险等级
    std::string scenario;                // 会遇场景描述
    std::vector<CollisionRisk> individual_risks;  // 单目标风险详情
};

enum class RiskLevel {
    Safe,       // 安全 (CPA>2nm, TCPA>30min)
    Caution,    // 注意 (CPA 1-2nm, TCPA 15-30min)
    Warning,    // 警告 (CPA 0.5-1nm, TCPA 6-15min)
    Danger      // 危险 (CPA<0.5nm, TCPA<6min)
};

class CollisionAssessor {
public:
    CollisionAssessor();
    
    void SetCpaThresholds(double safe, double caution, double warning, double danger);
    void SetTcpaThresholds(double safe, double caution, double warning, double danger);
    
    // 单目标评估
    std::vector<CollisionRisk> AssessAllTargets(
        const std::vector<AisTarget>& targets,
        const Coordinate& own_position,
        double own_course,
        double own_speed);
    
    CollisionRisk AssessTarget(
        const AisTarget& target,
        const Coordinate& own_position,
        double own_course,
        double own_speed);
    
    // 多目标综合评估
    std::vector<MultiTargetRisk> AssessMultiTargetScenario(
        const std::vector<AisTarget>& targets,
        const Coordinate& own_position,
        double own_course,
        double own_speed);
    
private:
    double CalculateCpa(const Coordinate& pos1, double course1, double speed1,
                       const Coordinate& pos2, double course2, double speed2);
    double CalculateTcpa(const Coordinate& pos1, double course1, double speed1,
                        const Coordinate& pos2, double course2, double speed2);
    
    std::vector<std::vector<int>> IdentifyEncounterGroups(
        const std::vector<AisTarget>& targets,
        const Coordinate& own_position);
    
    double CalculateCombinedCpa(const std::vector<int>& group,
                                const Coordinate& own_position,
                                double own_course, double own_speed);
    
    double FindMinTcpa(const std::vector<int>& group,
                       const Coordinate& own_position,
                       double own_course, double own_speed);
    
    std::string DescribeScenario(const std::vector<int>& group,
                                 const Coordinate& own_position);
    
    double cpa_safe_;
    double cpa_caution_;
    double cpa_warning_;
    double cpa_danger_;
    double tcpa_safe_;
    double tcpa_caution_;
    double tcpa_warning_;
    double tcpa_danger_;
    
    std::map<int, AisTarget> targets_map_;
};

class CollisionAssessor {
public:
    std::vector<MultiTargetRisk> AssessMultiTargetScenario(
        const std::vector<AisTarget>& targets,
        const Coordinate& own_position,
        double own_course,
        double own_speed) {
        
        std::vector<MultiTargetRisk> risks;
        
        // 1. 构建目标映射
        targets_map_.clear();
        for (const auto& target : targets) {
            targets_map_[target.GetMmsi()] = target;
        }
        
        // 2. 识别会遇群组 (距离<5海里的目标群)
        auto groups = IdentifyEncounterGroups(targets, own_position);
        
        // 3. 对每个群组进行综合评估
        for (const auto& group : groups) {
            MultiTargetRisk risk;
            risk.involved_targets = group;
            
            // 计算群组内所有目标的综合风险
            risk.combined_cpa = CalculateCombinedCpa(
                group, own_position, own_course, own_speed);
            risk.min_tcpa = FindMinTcpa(
                group, own_position, own_course, own_speed);
            risk.level = DetermineRiskLevel(risk.combined_cpa, risk.min_tcpa);
            risk.scenario = DescribeScenario(group, own_position);
            
            // 计算单目标风险详情
            for (int mmsi : group) {
                auto& target = targets_map_[mmsi];
                CollisionRisk individual = AssessTarget(
                    target, own_position, own_course, own_speed);
                risk.individual_risks.push_back(individual);
            }
            
            risks.push_back(risk);
        }
        
        // 4. 按风险等级排序
        std::sort(risks.begin(), risks.end(), 
            [](const MultiTargetRisk& a, const MultiTargetRisk& b) {
                return static_cast<int>(a.level) > static_cast<int>(b.level);
            });
        
        return risks;
    }
    
private:
    std::vector<std::vector<int>> IdentifyEncounterGroups(
        const std::vector<AisTarget>& targets,
        const Coordinate& own_position) {
        
        std::vector<std::vector<int>> groups;
        std::set<int> processed;
        
        for (const auto& target : targets) {
            if (processed.count(target.GetMmsi())) continue;
            
            // 检查与本船的距离
            double dist = CoordinateConverter::Instance().CalculateGreatCircleDistance(
                own_position.latitude, own_position.longitude,
                target.GetPosition().latitude, target.GetPosition().longitude);
            
            if (dist < 5.0) {  // 5海里范围内
                std::vector<int> group;
                group.push_back(target.GetMmsi());
                processed.insert(target.GetMmsi());
                
                // 查找群组内的其他目标
                for (const auto& other : targets) {
                    if (processed.count(other.GetMmsi())) continue;
                    
                    double dist_to_group = CoordinateConverter::Instance()
                        .CalculateGreatCircleDistance(
                            target.GetPosition().latitude, 
                            target.GetPosition().longitude,
                            other.GetPosition().latitude, 
                            other.GetPosition().longitude);
                    
                    if (dist_to_group < 2.0) {  // 2海里内视为同一群组
                        group.push_back(other.GetMmsi());
                        processed.insert(other.GetMmsi());
                    }
                }
                
                groups.push_back(group);
            }
        }
        
        return groups;
    }
    
    double CalculateCombinedCpa(const std::vector<int>& group,
                                const Coordinate& own_position,
                                double own_course, double own_speed) {
        double min_cpa = std::numeric_limits<double>::max();
        
        for (int mmsi : group) {
            auto& target = targets_map_[mmsi];
            double cpa = CalculateCpa(
                own_position, own_course, own_speed,
                target.GetPosition(), target.GetCourse(), target.GetSpeed());
            min_cpa = std::min(min_cpa, cpa);
        }
        
        return min_cpa;
    }
    
    std::string DescribeScenario(const std::vector<int>& group,
                                 const Coordinate& own_position) {
        if (group.size() == 1) {
            return "单目标会遇";
        } else if (group.size() == 2) {
            return "双目标会遇";
        } else {
            return "多目标复杂会遇 (" + std::to_string(group.size()) + "艘)";
        }
    }
    
    RiskLevel DetermineRiskLevel(double cpa, double tcpa) {
        if (cpa < cpa_danger_ && tcpa < tcpa_danger_) return RiskLevel::Danger;
        if (cpa < cpa_warning_ && tcpa < tcpa_warning_) return RiskLevel::Warning;
        if (cpa < cpa_caution_ && tcpa < tcpa_caution_) return RiskLevel::Caution;
        return RiskLevel::Safe;
    }
};
```

**AisManager - AIS管理器**:
```cpp
class AisManager {
public:
    static AisManager& Instance();
    
    bool Initialize();
    void Shutdown();
    
    void SetAisSource(AisSource source, const std::string& config);
    void StartReceiving();
    void StopReceiving();
    
    std::vector<AisTarget> GetAllTargets() const;
    AisTarget GetTarget(int mmsi) const;
    size_t GetTargetCount() const;
    
    void SetTargetUpdateCallback(TargetUpdateCallback callback);
    void SetCollisionAlertCallback(CollisionAlertCallback callback);
    
    std::vector<CollisionRisk> GetCollisionRisks(
        const Coordinate& own_position,
        double own_course,
        double own_speed);
    
private:
    AisManager();
    ~AisManager();
    
    void OnAisMessageReceived(const AisMessage& message);
    void UpdateTarget(const AisMessage& message);
    void RemoveExpiredTargets();
    
    std::map<int, AisTarget> targets_;
    std::unique_ptr<AisReceiver> receiver_;
    CollisionAssessor collision_assessor_;
    
    TargetUpdateCallback target_callback_;
    CollisionAlertCallback collision_callback_;
};

using TargetUpdateCallback = std::function<void(const AisTarget&)>;
using CollisionAlertCallback = std::function<void(const CollisionRisk&)>;
```

---

## 四、数据存储设计

### 4.1 数据库设计

#### 4.1.1 航线数据表

```sql
CREATE TABLE routes (
    id TEXT PRIMARY KEY,
    name TEXT NOT NULL,
    departure_port TEXT,
    arrival_port TEXT,
    total_distance REAL,
    created_time REAL,
    modified_time REAL,
    notes TEXT,
    data BLOB
);

CREATE TABLE waypoints (
    id TEXT PRIMARY KEY,
    route_id TEXT NOT NULL,
    sequence INTEGER NOT NULL,
    name TEXT,
    longitude REAL NOT NULL,
    latitude REAL NOT NULL,
    type INTEGER,
    turn_radius REAL,
    xtd_limit REAL,
    notes TEXT,
    FOREIGN KEY (route_id) REFERENCES routes(id) ON DELETE CASCADE
);

CREATE INDEX idx_waypoints_route ON waypoints(route_id);
```

#### 4.1.2 轨迹数据表

```sql
CREATE TABLE tracks (
    id TEXT PRIMARY KEY,
    name TEXT NOT NULL,
    start_time REAL,
    end_time REAL,
    total_distance REAL,
    point_count INTEGER,
    data BLOB
);

CREATE TABLE track_points (
    id TEXT PRIMARY KEY,
    track_id TEXT NOT NULL,
    timestamp REAL NOT NULL,
    longitude REAL NOT NULL,
    latitude REAL NOT NULL,
    heading REAL,
    speed REAL,
    depth REAL,
    status INTEGER,
    FOREIGN KEY (track_id) REFERENCES tracks(id) ON DELETE CASCADE
);

CREATE INDEX idx_track_points_track ON track_points(track_id);
CREATE INDEX idx_track_points_time ON track_points(timestamp);
```

#### 4.1.3 AIS数据表

```sql
CREATE TABLE ais_targets (
    mmsi INTEGER PRIMARY KEY,
    ship_name TEXT,
    callsign TEXT,
    ship_type INTEGER,
    longitude REAL,
    latitude REAL,
    heading REAL,
    speed REAL,
    course REAL,
    nav_status INTEGER,
    last_update REAL
);

CREATE INDEX idx_ais_targets_update ON ais_targets(last_update);
```

### 4.2 缓存设计

```cpp
template<typename Key, typename Value>
class DataCache {
public:
    DataCache(size_t max_size = 1000);
    
    void Put(const Key& key, const Value& value);
    Value Get(const Key& key);
    bool Contains(const Key& key);
    void Remove(const Key& key);
    void Clear();
    
    void SetMaxSize(size_t size);
    size_t GetSize() const;
    
private:
    std::map<Key, Value> cache_;
    std::list<Key> lru_list_;
    size_t max_size_;
    std::mutex mutex_;
    
    void EvictOldest();
};
```

---

## 五、接口设计

### 5.1 定位接口

```cpp
class IPositionService {
public:
    virtual ~IPositionService() = default;
    
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    
    virtual PositionData GetCurrentPosition() = 0;
    virtual void SetUpdateFrequency(int seconds) = 0;
    
    virtual void SetPositionCallback(PositionCallback callback) = 0;
};
```

### 5.2 航线接口

```cpp
class IRouteService {
public:
    virtual ~IRouteService() = default;
    
    virtual Route Plan(const Coordinate& start, const Coordinate& end,
                      const PlanningConstraints& constraints) = 0;
    
    virtual std::string SaveRoute(const Route& route) = 0;
    virtual Route LoadRoute(const std::string& route_id) = 0;
    virtual bool DeleteRoute(const std::string& route_id) = 0;
    virtual std::vector<Route> GetAllRoutes() = 0;
    
    virtual bool ExportRoute(const Route& route, const std::string& filepath,
                            ExportFormat format) = 0;
    virtual Route ImportRoute(const std::string& filepath,
                             ExportFormat format) = 0;
};
```

### 5.3 导航接口

```cpp
class INavigationService {
public:
    virtual ~INavigationService() = default;
    
    virtual void SetRoute(const Route& route) = 0;
    virtual void StartNavigation() = 0;
    virtual void StopNavigation() = 0;
    
    virtual NavigationData GetNavigationData() = 0;
    
    virtual void SetNavigationCallback(NavigationCallback callback) = 0;
    virtual void SetAlertCallback(AlertCallback callback) = 0;
};
```

### 5.4 轨迹接口

```cpp
class ITrackService {
public:
    virtual ~ITrackService() = default;
    
    virtual void StartRecording() = 0;
    virtual void StopRecording() = 0;
    virtual Track GetCurrentTrack() = 0;
    
    virtual std::string SaveTrack(const Track& track) = 0;
    virtual Track LoadTrack(const std::string& track_id) = 0;
    virtual bool DeleteTrack(const std::string& track_id) = 0;
    virtual std::vector<Track> GetAllTracks() = 0;
    
    virtual bool ExportTrack(const Track& track, const std::string& filepath,
                            ExportFormat format) = 0;
};
```

### 5.5 AIS接口

```cpp
class IAisService {
public:
    virtual ~IAisService() = default;
    
    virtual void StartReceiving() = 0;
    virtual void StopReceiving() = 0;
    
    virtual std::vector<AisTarget> GetAllTargets() = 0;
    virtual AisTarget GetTarget(int mmsi) = 0;
    
    virtual std::vector<CollisionRisk> GetCollisionRisks(
        const Coordinate& own_position,
        double own_course,
        double own_speed) = 0;
    
    virtual void SetTargetCallback(TargetUpdateCallback callback) = 0;
    virtual void SetCollisionCallback(CollisionAlertCallback callback) = 0;
};
```

---

## 六、性能设计

### 6.1 性能指标

| 操作 | 目标性能 | 优化策略 |
|-----|---------|---------|
| 定位更新 | <1s | 异步处理，数据缓存 |
| 航线规划 | <5s | 算法优化，并行计算 |
| 航点计算 | <100ms | 预计算，缓存结果 |
| 轨迹记录 | <50ms | 批量写入，异步存储 |
| AIS目标更新 | <2s | 增量更新，过期清理 |

### 6.2 性能优化策略

#### 6.2.1 算法优化

```cpp
class OptimizedPathPlanner {
public:
    // 使用空间索引加速邻居搜索
    std::vector<Coordinate> GetNeighbors(const Coordinate& point) {
        if (!spatial_index_) {
            BuildSpatialIndex();
        }
        return spatial_index_->Query(point, search_radius_);
    }
    
    // 使用双向A*加速搜索
    Route FindPathBidirectional(const Coordinate& start, const Coordinate& end) {
        // 从起点和终点同时搜索
        // 相遇时合并路径
    }
    
private:
    std::unique_ptr<SpatialIndex> spatial_index_;
    double search_radius_;
};
```

#### 6.2.2 数据缓存

```cpp
class RouteCache {
public:
    Route GetCachedRoute(const std::string& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            UpdateLru(it->first);
            return it->second;
        }
        return Route();
    }
    
    void CacheRoute(const std::string& key, const Route& route) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (cache_.size() >= max_size_) {
            EvictOldest();
        }
        cache_[key] = route;
        lru_list_.push_front(key);
    }
    
private:
    std::map<std::string, Route> cache_;
    std::list<std::string> lru_list_;
    size_t max_size_;
    std::mutex mutex_;
};
```

#### 6.2.3 并行计算

```cpp
class ParallelRouteOptimizer {
public:
    Route Optimize(const Route& route) {
        std::vector<std::future<RouteSegment>> futures;
        
        // 并行优化各航段
        for (size_t i = 0; i < route.GetWaypointCount() - 1; ++i) {
            futures.push_back(std::async(std::launch::async,
                [this, &route, i]() {
                    return OptimizeSegment(route.GetWaypoint(i),
                                          route.GetWaypoint(i + 1));
                }));
        }
        
        // 合并结果
        Route optimized;
        for (auto& future : futures) {
            auto segment = future.get();
            for (const auto& wp : segment.waypoints) {
                optimized.AddWaypoint(wp);
            }
        }
        
        return optimized;
    }
};
```

---

## 七、安全设计

### 7.1 数据安全

- **数据加密**: 敏感数据加密存储
- **访问控制**: 基于角色的访问控制
- **数据备份**: 定期数据备份机制

### 7.2 通信安全

- **数据验证**: NMEA数据校验和验证
- **异常处理**: 异常数据过滤和告警
- **连接监控**: 设备连接状态监控

### 7.3 安全策略

```cpp
class SecurityManager {
public:
    static SecurityManager& Instance();
    
    bool ValidatePositionData(const PositionData& data);
    bool ValidateAisMessage(const AisMessage& message);
    
    void SetUserLevel(UserLevel level);
    UserLevel GetUserLevel() const;
    
    bool HasPermission(const std::string& permission);
    
private:
    SecurityManager();
    UserLevel user_level_;
    std::set<std::string> permissions_;
};
```

---

## 八、测试设计

### 8.1 单元测试

| 模块 | 测试文件 | 测试用例数 |
|-----|---------|-----------|
| 定位模块 | test_nmea_parser.cpp | 50+ |
| 航线模块 | test_route_planner.cpp | 40+ |
| 导航模块 | test_navigation_engine.cpp | 35+ |
| 轨迹模块 | test_track_recorder.cpp | 30+ |
| AIS模块 | test_ais_parser.cpp | 45+ |

### 8.2 集成测试

```cpp
class NavigationIntegrationTest : public Test {
protected:
    void SetUp() override {
        PositionManager::Instance().Initialize(config_);
        RouteManager::Instance().Initialize(db_path_);
        NavigationEngine::Instance().Initialize();
    }
    
    void TearDown() override {
        NavigationEngine::Instance().Shutdown();
        RouteManager::Instance().Shutdown();
        PositionManager::Instance().Shutdown();
    }
    
    PositionConfig config_;
    std::string db_path_;
};

TEST_F(NavigationIntegrationTest, FullNavigationFlow) {
    // 1. 规划航线
    Route route = RouteManager::Instance().Plan(start, end, constraints);
    ASSERT_GT(route.GetWaypointCount(), 0);
    
    // 2. 开始导航
    NavigationEngine::Instance().SetRoute(route);
    NavigationEngine::Instance().StartNavigation();
    
    // 3. 模拟定位更新
    PositionData position;
    position.longitude = start.longitude;
    position.latitude = start.latitude;
    NavigationEngine::Instance().UpdatePosition(position);
    
    // 4. 验证导航数据
    NavigationData nav_data = NavigationEngine::Instance().GetNavigationData();
    EXPECT_EQ(nav_data.current_leg_index, 0);
    
    // 5. 停止导航
    NavigationEngine::Instance().StopNavigation();
}
```

### 8.3 性能测试

```cpp
class PerformanceTest : public Test {
protected:
    void MeasureRoutePlanningTime() {
        auto start = std::chrono::high_resolution_clock::now();
        
        Route route = planner_.Plan(start_coord_, end_coord_);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end - start);
        
        EXPECT_LT(duration.count(), 5000);  // <5秒
    }
    
    void MeasurePositionUpdateLatency() {
        PositionData position;
        auto start = std::chrono::high_resolution_clock::now();
        
        NavigationEngine::Instance().UpdatePosition(position);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end - start);
        
        EXPECT_LT(duration.count(), 100);  // <100ms
    }
    
    void MeasureTrackPlaybackPerformance() {
        Track track = CreateLargeTrack(10000);
        
        TrackPlayer player;
        player.LoadTrack(track);
        
        auto start = std::chrono::high_resolution_clock::now();
        player.Play();
        
        std::this_thread::sleep_for(std::chrono::seconds(10));
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end - start);
        
        EXPECT_LT(duration.count(), 11000);
    }
    
    void MeasureAisTargetUpdatePerformance() {
        std::vector<AisTarget> targets;
        for (int i = 0; i < 100; ++i) {
            targets.push_back(CreateAisTarget(i));
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        AisManager::Instance().UpdateTargets(targets);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end - start);
        
        EXPECT_LT(duration.count(), 2000);
    }
    
    void MeasureDatabaseQueryPerformance() {
        for (int i = 0; i < 1000; ++i) {
            Route route = CreateTestRoute(i);
            RouteManager::Instance().CreateRoute(route);
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        auto routes = RouteManager::Instance().SearchRoutes("test");
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end - start);
        
        EXPECT_LT(duration.count(), 500);
    }
    
    void MeasureMemoryUsage() {
        NavigationEngine::Instance().StartNavigation();
        
        long initial_memory = GetMemoryUsage();
        
        for (int hour = 0; hour < 24; ++hour) {
            for (int min = 0; min < 60; ++min) {
                PositionData pos = GeneratePosition();
                NavigationEngine::Instance().UpdatePosition(pos);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        
        long final_memory = GetMemoryUsage();
        long memory_growth = final_memory - initial_memory;
        
        EXPECT_LT(memory_growth, 50 * 1024 * 1024);
        
        NavigationEngine::Instance().StopNavigation();
    }
};
```

### 8.4 性能测试场景汇总

| 测试场景 | 性能指标 | 测试方法 |
|---------|---------|---------|
| 航线规划 | <5秒 | MeasureRoutePlanningTime |
| 定位更新 | <100ms | MeasurePositionUpdateLatency |
| 轨迹回放 | 10000点/10秒 | MeasureTrackPlaybackPerformance |
| AIS目标更新 | 100目标/2秒 | MeasureAisTargetUpdatePerformance |
| 数据库查询 | <500ms | MeasureDatabaseQueryPerformance |
| 内存稳定性 | <50MB增长/24小时 | MeasureMemoryUsage |

---

## 九、离线导航设计

### 9.1 离线数据缓存策略

```cpp
struct OfflineCacheConfig {
    double cache_radius_nm = 50.0;
    int max_cached_charts = 100;
    int max_cached_routes = 50;
    long max_cache_size_mb = 500;
    int cache_expiry_days = 30;
};

class OfflineCacheManager {
public:
    static OfflineCacheManager& Instance();
    
    bool Initialize(const OfflineCacheConfig& config);
    void Shutdown();
    
    bool CacheChartArea(const Envelope& bounds);
    bool CacheChartAlongRoute(const Route& route, double width_nm);
    bool IsChartCached(const Coordinate& coord);
    ChartData GetCachedChart(const Coordinate& coord);
    
    bool CacheRoute(const Route& route);
    std::vector<Route> GetCachedRoutes();
    
    void ClearExpiredCache();
    void ClearAllCache();
    CacheStatistics GetStatistics();
    
    void SetSyncCallback(SyncCallback callback);
    void SyncWhenOnline();
    
private:
    std::unique_ptr<OfflineChartStorage> chart_storage_;
    std::unique_ptr<OfflineRouteStorage> route_storage_;
    OfflineCacheConfig config_;
    CacheStatistics stats_;
    SyncCallback sync_callback_;
};
```

### 9.2 离线航线规划

```cpp
class OfflineRoutePlanner {
public:
    Route PlanOffline(const Coordinate& start, const Coordinate& end,
                     const PlanningConstraints& constraints) {
        if (!offline_cache_->IsChartCached(start) || 
            !offline_cache_->IsChartCached(end)) {
            throw OfflineException("起点或终点不在缓存范围内");
        }
        
        RoutePlanner planner;
        planner.SetChartDatabase(CreateOfflineChartDb());
        planner.SetConstraints(constraints);
        
        return planner.Plan(start, end);
    }
    
private:
    std::unique_ptr<IChartDatabase> CreateOfflineChartDb();
};
```

### 9.3 离线数据同步

```cpp
class OfflineSyncManager {
public:
    struct SyncTask {
        std::string task_id;
        SyncType type;
        SyncStatus status;
        double progress;
        std::string error_message;
    };
    
    void StartSync();
    void CancelSync();
    SyncTask GetSyncStatus();
    
private:
    void SyncCharts();
    void SyncRoutes();
    void SyncTracks();
};
```

---

## 十、部署设计

### 9.1 部署架构

```
┌─────────────────────────────────────────────────────────────┐
│                      客户端应用                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │              海图导航系统                              │  │
│  │  ┌────────────┐  ┌────────────┐  ┌────────────┐    │  │
│  │  │ 定位模块    │  │ 航线模块    │  │ 导航模块    │    │  │
│  │  └────────────┘  └────────────┘  └────────────┘    │  │
│  │  ┌────────────┐  ┌────────────┐  ┌────────────┐    │  │
│  │  │ 轨迹模块    │  │ AIS模块     │  │ 数据管理    │    │  │
│  │  └────────────┘  └────────────┘  └────────────┘    │  │
│  └──────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
         ↓                  ↓                  ↓
    北斗定位设备        海图数据库          AIS设备
```

### 9.2 配置管理

```cpp
struct NavigationSystemConfig {
    // 定位配置
    PositionConfig position_config;
    
    // 航线配置
    RouteConfig route_config;
    
    // 导航配置
    NavigationConfig navigation_config;
    
    // 轨迹配置
    TrackConfig track_config;
    
    // AIS配置
    AisConfig ais_config;
    
    // 数据库配置
    DatabaseConfig database_config;
    
    static NavigationSystemConfig Load(const std::string& config_file);
    void Save(const std::string& config_file);
};
```

---

## 十、验收标准

### 10.1 功能验收

| 验收项 | 验收标准 | 测试方法 |
|-------|---------|---------|
| 定位功能 | 正确接收和解析北斗定位数据 | 单元测试+集成测试 |
| 航线规划 | 生成的航线满足安全约束 | 算法测试+场景测试 |
| 导航引导 | 正确显示导航信息 | UI测试+功能测试 |
| 偏航警示 | 偏航时正确触发警示 | 场景测试 |
| 轨迹记录 | 正确记录和回放轨迹 | 功能测试 |
| AIS集成 | 正确接收和显示AIS目标 | 集成测试 |

### 10.2 性能验收

| 验收项 | 验收标准 | 测试方法 |
|-------|---------|---------|
| 定位延迟 | <1s | 性能测试 |
| 航线规划 | <5s | 性能测试 |
| 导航响应 | <100ms | 性能测试 |
| 内存占用 | <200MB | 资源监控 |

### 10.3 质量验收

| 指标 | 目标值 | 测试方法 |
|-----|-------|---------|
| 代码覆盖率 | ≥80% | 单元测试 |
| 静态分析 | 0 critical | 静态分析工具 |
| 内存泄漏 | 0 leak | 内存检测工具 |

---

## 十一、算法优化设计

### 11.1 A*算法回退机制

```cpp
enum class PlanningFailureReason {
    SUCCESS,
    NO_PATH_FOUND,
    CONSTRAINT_VIOLATION,
    TIMEOUT,
    MEMORY_LIMIT_EXCEEDED,
    CHART_DATA_UNAVAILABLE
};

struct PlanningResult {
    Route route;
    PlanningFailureReason reason;
    std::string message;
    std::vector<Coordinate> partial_path;
    std::map<std::string, std::string> diagnostics;
};

class AStarPlanner {
public:
    PlanningResult FindPathWithFallback(
        const Coordinate& start, const Coordinate& end,
        const PlanningConstraints& constraints,
        const PlanningFactors& factors) {
        
        PlanningResult result = FindPathInternal(start, end, constraints, factors);
        
        if (result.reason != PlanningFailureReason::SUCCESS) {
            result = ApplyFallbackStrategy(result, start, end, constraints, factors);
        }
        
        return result;
    }
    
private:
    PlanningResult ApplyFallbackStrategy(
        const PlanningResult& failed_result,
        const Coordinate& start, const Coordinate& end,
        PlanningConstraints constraints,
        const PlanningFactors& factors) {
        
        PlanningResult result = failed_result;
        
        switch (failed_result.reason) {
            case PlanningFailureReason::NO_PATH_FOUND:
                result = TryRelaxedConstraints(start, end, constraints, factors);
                break;
                
            case PlanningFailureReason::CONSTRAINT_VIOLATION:
                result = TrySegmentedPlanning(start, end, constraints, factors);
                break;
                
            case PlanningFailureReason::TIMEOUT:
                result.route = CreatePartialRoute(failed_result.partial_path);
                result.message = "规划超时，已提供部分路径";
                break;
                
            case PlanningFailureReason::MEMORY_LIMIT_EXCEEDED:
                result = TryMemoryOptimizedPlanning(start, end, constraints, factors);
                break;
                
            case PlanningFailureReason::CHART_DATA_UNAVAILABLE:
                result = TryOfflinePlanning(start, end, constraints, factors);
                break;
        }
        
        return result;
    }
    
    PlanningResult TryRelaxedConstraints(
        const Coordinate& start, const Coordinate& end,
        PlanningConstraints constraints,
        const PlanningFactors& factors) {
        
        constraints.min_depth *= 0.9;
        constraints.min_turn_radius *= 0.8;
        
        return FindPathInternal(start, end, constraints, factors);
    }
    
    PlanningResult TrySegmentedPlanning(
        const Coordinate& start, const Coordinate& end,
        const PlanningConstraints& constraints,
        const PlanningFactors& factors) {
        
        std::vector<Coordinate> intermediate_points = 
            FindIntermediateWaypoints(start, end);
        
        Route combined_route;
        Coordinate current = start;
        
        for (const auto& waypoint : intermediate_points) {
            PlanningResult segment = FindPathInternal(
                current, waypoint, constraints, factors);
            if (segment.reason == PlanningFailureReason::SUCCESS) {
                combined_route.Merge(segment.route);
                current = waypoint;
            }
        }
        
        PlanningResult final_segment = FindPathInternal(
            current, end, constraints, factors);
        if (final_segment.reason == PlanningFailureReason::SUCCESS) {
            combined_route.Merge(final_segment.route);
            PlanningResult result;
            result.route = combined_route;
            result.reason = PlanningFailureReason::SUCCESS;
            return result;
        }
        
        PlanningResult result;
        result.reason = PlanningFailureReason::NO_PATH_FOUND;
        result.message = "分段规划失败，建议人工干预";
        return result;
    }
};
```

### 11.2 A*算法内存优化

```cpp
class MemoryOptimizedAStarPlanner {
public:
    struct Node {
        Coordinate coord;
        double g_cost;
        double h_cost;
        double f_cost;
        Node* parent;
        bool in_closed_set;
    };
    
    MemoryOptimizedAStarPlanner(size_t max_nodes = 100000) 
        : max_nodes_(max_nodes), node_pool_(max_nodes) {
        node_allocator_.reserve(max_nodes);
    }
    
    Route FindPath(const Coordinate& start, const Coordinate& end,
                   const PlanningConstraints& constraints,
                   const PlanningFactors& factors) {
        
        std::priority_queue<Node*, std::vector<Node*>, NodeComparator> open_set;
        std::unordered_map<Coordinate, Node*, CoordinateHash> node_map;
        
        Node* start_node = AllocateNode();
        start_node->coord = start;
        start_node->g_cost = 0;
        start_node->h_cost = Heuristic(start, end);
        start_node->f_cost = start_node->h_cost;
        start_node->parent = nullptr;
        start_node->in_closed_set = false;
        
        open_set.push(start_node);
        node_map[start] = start_node;
        
        size_t nodes_used = 1;
        
        while (!open_set.empty() && nodes_used < max_nodes_) {
            Node* current = open_set.top();
            open_set.pop();
            
            if (current->in_closed_set) continue;
            
            if (IsGoal(current->coord, end)) {
                return ReconstructPath(current);
            }
            
            current->in_closed_set = true;
            
            for (const auto& neighbor_coord : GetNeighbors(current->coord)) {
                auto it = node_map.find(neighbor_coord);
                Node* neighbor_node;
                
                if (it != node_map.end()) {
                    neighbor_node = it->second;
                    if (neighbor_node->in_closed_set) continue;
                } else {
                    if (nodes_used >= max_nodes_) {
                        return HandleMemoryLimitExceeded(current, end);
                    }
                    neighbor_node = AllocateNode();
                    neighbor_node->coord = neighbor_coord;
                    neighbor_node->h_cost = Heuristic(neighbor_coord, end);
                    neighbor_node->in_closed_set = false;
                    node_map[neighbor_coord] = neighbor_node;
                    nodes_used++;
                }
                
                double tentative_g = current->g_cost + 
                    Distance(current->coord, neighbor_coord);
                
                if (tentative_g < neighbor_node->g_cost || 
                    neighbor_node->g_cost == 0) {
                    neighbor_node->g_cost = tentative_g;
                    neighbor_node->f_cost = tentative_g + neighbor_node->h_cost;
                    neighbor_node->parent = current;
                    open_set.push(neighbor_node);
                }
            }
        }
        
        return Route();
    }
    
private:
    Node* AllocateNode() {
        if (node_pool_index_ < node_pool_.size()) {
            return &node_pool_[node_pool_index_++];
        }
        node_allocator_.emplace_back();
        return &node_allocator_.back();
    }
    
    void RecycleNode(Node* node) {
        node->in_closed_set = false;
        node->g_cost = 0;
        node->parent = nullptr;
    }
    
    Route HandleMemoryLimitExceeded(Node* current, const Coordinate& end) {
        Route partial_route = ReconstructPath(current);
        partial_route.SetProperty("incomplete", "true");
        partial_route.SetProperty("reason", "memory_limit_exceeded");
        return partial_route;
    }
    
    size_t max_nodes_;
    std::vector<Node> node_pool_;
    std::deque<Node> node_allocator_;
    size_t node_pool_index_ = 0;
};
```

### 11.3 空间索引优化

```cpp
class SpatialIndexInterface {
public:
    virtual ~SpatialIndexInterface() = default;
    
    virtual void Insert(const Coordinate& coord, const ChartData& data) = 0;
    virtual void Remove(const Coordinate& coord) = 0;
    virtual std::vector<ChartData> Query(const Envelope& bounds) = 0;
    virtual std::vector<Coordinate> QueryNeighbors(
        const Coordinate& center, double radius_nm) = 0;
    virtual ChartData QueryNearest(const Coordinate& coord) = 0;
    
    virtual size_t GetSize() const = 0;
    virtual void Clear() = 0;
    virtual void Build(const std::vector<std::pair<Coordinate, ChartData>>& data) = 0;
};

class RTreeIndex : public SpatialIndexInterface {
public:
    void Insert(const Coordinate& coord, const ChartData& data) override {
        rtree_.insert(std::make_pair(Point(coord.longitude, coord.latitude), data));
    }
    
    std::vector<ChartData> Query(const Envelope& bounds) override {
        std::vector<ChartData> results;
        Box query_box(Point(bounds.min_x, bounds.min_y),
                     Point(bounds.max_x, bounds.max_y));
        
        std::vector<Value> query_results;
        rtree_.query(bgi::intersects(query_box), std::back_inserter(query_results));
        
        for (const auto& result : query_results) {
            results.push_back(result.second);
        }
        
        return results;
    }
    
    std::vector<Coordinate> QueryNeighbors(
        const Coordinate& center, double radius_nm) override {
        
        std::vector<Coordinate> neighbors;
        double radius_deg = radius_nm / 60.0;
        
        std::vector<Value> query_results;
        rtree_.query(
            bgi::satisfies([&center, radius_deg](const Value& v) {
                double dist = std::sqrt(
                    std::pow(v.first.get<0>() - center.longitude, 2) +
                    std::pow(v.first.get<1>() - center.latitude, 2));
                return dist <= radius_deg;
            }),
            std::back_inserter(query_results));
        
        for (const auto& result : query_results) {
            neighbors.emplace_back(result.first.get<0>(), result.first.get<1>());
        }
        
        return neighbors;
    }
    
private:
    using Point = bg::model::point<double, 2, bg::cs::cartesian>;
    using Box = bg::model::box<Point>;
    using Value = std::pair<Point, ChartData>;
    using RTree = bgi::rtree<Value, bgi::quadratic<16>>;
    
    RTree rtree_;
};

class ChartDatabaseWithIndex : public IChartDatabase {
public:
    ChartDatabaseWithIndex() {
        spatial_index_ = std::make_unique<RTreeIndex>();
    }
    
    ChartData Query(const Coordinate& coord) override {
        ChartData cached = cache_.Get(coord);
        if (cached.IsValid()) {
            return cached;
        }
        
        ChartData data = spatial_index_->QueryNearest(coord);
        cache_.Put(coord, data);
        return data;
    }
    
    void BuildIndex(const std::vector<std::pair<Coordinate, ChartData>>& data) {
        spatial_index_->Clear();
        spatial_index_->Build(data);
    }
    
private:
    std::unique_ptr<SpatialIndexInterface> spatial_index_;
    DataCache<Coordinate, ChartData> cache_;
};
```

### 11.4 RRT*算法设计

RRT*（Rapidly-exploring Random Tree Star）算法是一种渐进最优的采样型路径规划算法，适用于复杂障碍环境下的航线规划。

#### 11.4.1 算法语义说明

| 算法名称 | 全称 | 适用场景 | 特点 |
|---------|------|---------|------|
| **A*** | A-Star Algorithm | 网格化环境、已知地图 | 启发式搜索，保证最优解，但内存消耗大 |
| **RRT*** | Optimal Rapidly-exploring Random Tree | 复杂障碍环境、连续空间 | 渐进最优，适应动态环境，无需网格化 |
- A*算法 : 明确适用于开阔海域、简单约束场景，提供最短路径规划
- RRT*算法 : 明确适用于复杂障碍环境、动态障碍场景，提供渐进最优路径

#### 11.4.2 RRT*算法实现

```cpp
struct RrtNode {
    Coordinate position;
    RrtNode* parent;
    std::vector<RrtNode*> children;
    double cost;
    int depth;
};

struct RrtConfig {
    int max_iterations = 10000;
    double step_size = 0.005;
    double goal_bias = 0.1;
    double search_radius = 0.02;
    double goal_threshold = 0.001;
    int rewiring_interval = 100;
};

class RrtStarPlanner {
public:
    RrtStarPlanner() {
        config_.max_iterations = 10000;
        config_.step_size = 0.005;
        config_.goal_bias = 0.1;
        config_.search_radius = 0.02;
    }
    
    void SetChartDatabase(IChartDatabase* chart_db) {
        chart_db_ = chart_db;
    }
    
    void SetConfig(const RrtConfig& config) {
        config_ = config;
    }
    
    Route FindPath(const Coordinate& start, const Coordinate& end,
                   const PlanningConstraints& constraints,
                   const PlanningFactors& factors) {
        nodes_.clear();
        node_pool_.clear();
        
        RrtNode* root = CreateNode(start, nullptr, 0.0);
        nodes_.push_back(root);
        
        RrtNode* best_goal_node = nullptr;
        double best_cost = std::numeric_limits<double>::max();
        
        for (int i = 0; i < config_.max_iterations; ++i) {
            Coordinate random_point = SampleRandomPoint(end);
            
            RrtNode* nearest = FindNearestNode(random_point);
            Coordinate new_pos = Steer(nearest->position, random_point);
            
            if (!IsCollisionFree(nearest->position, new_pos, constraints)) {
                continue;
            }
            
            std::vector<RrtNode*> near_nodes = FindNearNodes(new_pos);
            RrtNode* best_parent = ChooseBestParent(new_pos, near_nodes);
            
            if (best_parent) {
                RrtNode* new_node = CreateNode(new_pos, best_parent,
                    best_parent->cost + Distance(best_parent->position, new_pos));
                nodes_.push_back(new_node);
                
                Rewire(new_node, near_nodes);
                
                if (IsGoalReached(new_pos, end)) {
                    if (new_node->cost < best_cost) {
                        best_cost = new_node->cost;
                        best_goal_node = new_node;
                    }
                }
            }
        }
        
        if (best_goal_node) {
            return ExtractPath(best_goal_node);
        }
        
        return Route();
    }
    
private:
    RrtNode* CreateNode(const Coordinate& pos, RrtNode* parent, double cost) {
        node_pool_.emplace_back();
        RrtNode* node = &node_pool_.back();
        node->position = pos;
        node->parent = parent;
        node->cost = cost;
        node->depth = parent ? parent->depth + 1 : 0;
        if (parent) {
            parent->children.push_back(node);
        }
        return node;
    }
    
    Coordinate SampleRandomPoint(const Coordinate& goal) {
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        if (dist(rng_) < config_.goal_bias) {
            return goal;
        }
        
        std::uniform_real_distribution<double> lon_dist(
            bounds_.min_x, bounds_.max_x);
        std::uniform_real_distribution<double> lat_dist(
            bounds_.min_y, bounds_.max_y);
        
        return Coordinate(lon_dist(rng_), lat_dist(rng_));
    }
    
    RrtNode* FindNearestNode(const Coordinate& point) {
        RrtNode* nearest = nullptr;
        double min_dist = std::numeric_limits<double>::max();
        
        for (RrtNode* node : nodes_) {
            double dist = Distance(node->position, point);
            if (dist < min_dist) {
                min_dist = dist;
                nearest = node;
            }
        }
        
        return nearest;
    }
    
    Coordinate Steer(const Coordinate& from, const Coordinate& to) {
        double dist = Distance(from, to);
        if (dist <= config_.step_size) {
            return to;
        }
        
        double ratio = config_.step_size / dist;
        return Coordinate(
            from.longitude + (to.longitude - from.longitude) * ratio,
            from.latitude + (to.latitude - from.latitude) * ratio
        );
    }
    
    bool IsCollisionFree(const Coordinate& from, const Coordinate& to,
                        const PlanningConstraints& constraints) {
        if (!chart_db_) return true;
        
        int steps = static_cast<int>(Distance(from, to) / 0.0001);
        for (int i = 0; i <= steps; ++i) {
            double t = static_cast<double>(i) / steps;
            Coordinate check_point(
                from.longitude + t * (to.longitude - from.longitude),
                from.latitude + t * (to.latitude - from.latitude)
            );
            
            ChartData data = chart_db_->Query(check_point);
            if (data.has_obstacle || data.is_restricted) {
                return false;
            }
            
            double safe_depth = constraints.min_depth + constraints.ukc_margin;
            if (data.depth < safe_depth) {
                return false;
            }
        }
        
        return true;
    }
    
    std::vector<RrtNode*> FindNearNodes(const Coordinate& point) {
        std::vector<RrtNode*> near_nodes;
        double radius = config_.search_radius * 
            std::sqrt(std::log(nodes_.size() + 1) / nodes_.size());
        
        for (RrtNode* node : nodes_) {
            if (Distance(node->position, point) <= radius) {
                near_nodes.push_back(node);
            }
        }
        
        return near_nodes;
    }
    
    RrtNode* ChooseBestParent(const Coordinate& point,
                             const std::vector<RrtNode*>& near_nodes) {
        RrtNode* best = nullptr;
        double min_cost = std::numeric_limits<double>::max();
        
        for (RrtNode* node : near_nodes) {
            double cost = node->cost + Distance(node->position, point);
            if (cost < min_cost && 
                IsCollisionFree(node->position, point, constraints_)) {
                min_cost = cost;
                best = node;
            }
        }
        
        return best;
    }
    
    void Rewire(RrtNode* new_node, const std::vector<RrtNode*>& near_nodes) {
        for (RrtNode* node : near_nodes) {
            double new_cost = new_node->cost + 
                Distance(new_node->position, node->position);
            
            if (new_cost < node->cost &&
                IsCollisionFree(new_node->position, node->position, constraints_)) {
                node->parent->children.erase(
                    std::remove(node->parent->children.begin(),
                               node->parent->children.end(), node),
                    node->parent->children.end());
                
                node->parent = new_node;
                node->cost = new_cost;
                new_node->children.push_back(node);
                
                UpdateChildrenCosts(node);
            }
        }
    }
    
    void UpdateChildrenCosts(RrtNode* node) {
        for (RrtNode* child : node->children) {
            child->cost = node->cost + Distance(node->position, child->position);
            UpdateChildrenCosts(child);
        }
    }
    
    bool IsGoalReached(const Coordinate& point, const Coordinate& goal) {
        return Distance(point, goal) <= config_.goal_threshold;
    }
    
    Route ExtractPath(RrtNode* goal_node) {
        Route route;
        std::vector<Coordinate> path;
        
        RrtNode* current = goal_node;
        while (current != nullptr) {
            path.push_back(current->position);
            current = current->parent;
        }
        
        std::reverse(path.begin(), path.end());
        
        for (const auto& coord : path) {
            Waypoint wp;
            wp.longitude = coord.longitude;
            wp.latitude = coord.latitude;
            route.AddWaypoint(wp);
        }
        
        return route;
    }
    
    double Distance(const Coordinate& a, const Coordinate& b) {
        return CoordinateConverter::Instance().CalculateGreatCircleDistance(
            a.latitude, a.longitude, b.latitude, b.longitude);
    }
    
    IChartDatabase* chart_db_ = nullptr;
    RrtConfig config_;
    Envelope bounds_;
    PlanningConstraints constraints_;
    std::vector<RrtNode*> nodes_;
    std::deque<RrtNode> node_pool_;
    std::mt19937 rng_{std::random_device{}()};
};
```

### 11.5 遗传算法多目标优化

遗传算法（Genetic Algorithm）用于多目标航线优化，可同时优化安全性、经济性、时间性等多个目标。

#### 11.5.1 算法语义说明

| 算法名称 | 全称 | 适用场景 | 特点 |
|---------|------|---------|------|
| **GA** | Genetic Algorithm | 多目标优化、复杂约束 | 全局搜索能力强，可优化多个目标 |

#### 11.5.2 多目标优化目标定义

```cpp
enum class OptimizationObjective {
    SAFETY,         // 安全性：最大化安全裕度
    ECONOMY,        // 经济性：最小化航程/油耗
    TIME,           // 时间性：最小化航行时间
    COMFORT,        // 舒适性：最小化转弯次数
    ENVIRONMENT     // 环保性：最小化排放
};

struct ObjectiveWeight {
    OptimizationObjective objective;
    double weight;
    bool enabled;
};

struct MultiObjectiveConfig {
    std::vector<ObjectiveWeight> objectives;
    int population_size = 100;
    int max_generations = 200;
    double crossover_rate = 0.8;
    double mutation_rate = 0.1;
    double elite_ratio = 0.1;
    double convergence_threshold = 0.001;
};
```

#### 11.5.3 遗传算法实现

```cpp
struct Chromosome {
    std::vector<Coordinate> genes;
    std::vector<double> fitness_values;
    double dominated_count;
    std::vector<Chromosome*> dominated_set;
    int rank;
    double crowding_distance;
    bool is_valid;
};

class GeneticAlgorithmPlanner {
public:
    GeneticAlgorithmPlanner() {
        InitializeDefaultObjectives();
    }
    
    void SetConfig(const MultiObjectiveConfig& config) {
        config_ = config;
    }
    
    void SetChartDatabase(IChartDatabase* chart_db) {
        chart_db_ = chart_db;
    }
    
    std::vector<Route> FindParetoOptimalRoutes(
        const Coordinate& start, const Coordinate& end,
        const PlanningConstraints& constraints,
        const PlanningFactors& factors) {
        
        std::vector<Chromosome> population = InitializePopulation(start, end);
        
        for (int gen = 0; gen < config_.max_generations; ++gen) {
            EvaluateFitness(population, constraints, factors);
            
            std::vector<Chromosome> fronts = NonDominatedSort(population);
            CalculateCrowdingDistance(fronts);
            
            if (CheckConvergence(fronts)) {
                break;
            }
            
            std::vector<Chromosome> offspring = GenerateOffspring(population);
            population = SelectNextGeneration(population, offspring);
        }
        
        return ExtractParetoFront(population);
    }
    
private:
    void InitializeDefaultObjectives() {
        config_.objectives = {
            {OptimizationObjective::SAFETY, 0.4, true},
            {OptimizationObjective::ECONOMY, 0.3, true},
            {OptimizationObjective::TIME, 0.2, true},
            {OptimizationObjective::COMFORT, 0.1, true}
        };
    }
    
    std::vector<Chromosome> InitializePopulation(
        const Coordinate& start, const Coordinate& end) {
        
        std::vector<Chromosome> population;
        population.reserve(config_.population_size);
        
        for (int i = 0; i < config_.population_size; ++i) {
            Chromosome individual;
            individual.genes = GenerateRandomPath(start, end);
            individual.is_valid = true;
            population.push_back(individual);
        }
        
        return population;
    }
    
    std::vector<Coordinate> GenerateRandomPath(
        const Coordinate& start, const Coordinate& end) {
        
        std::vector<Coordinate> path;
        path.push_back(start);
        
        int num_waypoints = RandomInt(3, 10);
        
        for (int i = 0; i < num_waypoints; ++i) {
            double t = static_cast<double>(i + 1) / (num_waypoints + 1);
            Coordinate base_point(
                start.longitude + t * (end.longitude - start.longitude),
                start.latitude + t * (end.latitude - start.latitude)
            );
            
            double offset_lon = RandomDouble(-0.05, 0.05);
            double offset_lat = RandomDouble(-0.05, 0.05);
            
            path.emplace_back(base_point.longitude + offset_lon,
                            base_point.latitude + offset_lat);
        }
        
        path.push_back(end);
        return path;
    }
    
    void EvaluateFitness(std::vector<Chromosome>& population,
                        const PlanningConstraints& constraints,
                        const PlanningFactors& factors) {
        for (auto& individual : population) {
            individual.fitness_values.clear();
            
            for (const auto& obj : config_.objectives) {
                if (!obj.enabled) continue;
                
                double fitness = CalculateObjective(
                    individual, obj.objective, constraints, factors);
                individual.fitness_values.push_back(fitness);
            }
        }
    }
    
    double CalculateObjective(const Chromosome& individual,
                             OptimizationObjective objective,
                             const PlanningConstraints& constraints,
                             const PlanningFactors& factors) {
        switch (objective) {
            case OptimizationObjective::SAFETY:
                return CalculateSafetyFitness(individual, constraints);
            case OptimizationObjective::ECONOMY:
                return CalculateEconomyFitness(individual);
            case OptimizationObjective::TIME:
                return CalculateTimeFitness(individual, factors);
            case OptimizationObjective::COMFORT:
                return CalculateComfortFitness(individual);
            case OptimizationObjective::ENVIRONMENT:
                return CalculateEnvironmentFitness(individual, factors);
            default:
                return 0.0;
        }
    }
    
    double CalculateSafetyFitness(const Chromosome& individual,
                                  const PlanningConstraints& constraints) {
        double min_safety_margin = std::numeric_limits<double>::max();
        
        for (const auto& point : individual.genes) {
            if (chart_db_) {
                ChartData data = chart_db_->Query(point);
                double margin = data.depth - constraints.min_depth;
                min_safety_margin = std::min(min_safety_margin, margin);
            }
        }
        
        return min_safety_margin;
    }
    
    double CalculateEconomyFitness(const Chromosome& individual) {
        double total_distance = 0;
        
        for (size_t i = 1; i < individual.genes.size(); ++i) {
            total_distance += Distance(
                individual.genes[i-1], individual.genes[i]);
        }
        
        return total_distance;
    }
    
    double CalculateTimeFitness(const Chromosome& individual,
                               const PlanningFactors& factors) {
        double distance = CalculateEconomyFitness(individual);
        double avg_speed = factors.average_speed;
        return distance / avg_speed;
    }
    
    double CalculateComfortFitness(const Chromosome& individual) {
        if (individual.genes.size() < 3) return 0;
        
        int turn_count = 0;
        for (size_t i = 1; i < individual.genes.size() - 1; ++i) {
            double angle = CalculateTurnAngle(
                individual.genes[i-1], individual.genes[i], 
                individual.genes[i+1]);
            if (angle > 15.0) {
                turn_count++;
            }
        }
        
        return turn_count;
    }
    
    double CalculateEnvironmentFitness(const Chromosome& individual,
                                       const PlanningFactors& factors) {
        double distance = CalculateEconomyFitness(individual);
        return distance * factors.emission_factor;
    }
    
    std::vector<Chromosome> NonDominatedSort(
        std::vector<Chromosome>& population) {
        
        for (auto& individual : population) {
            individual.dominated_count = 0;
            individual.dominated_set.clear();
        }
        
        for (size_t i = 0; i < population.size(); ++i) {
            for (size_t j = i + 1; j < population.size(); ++j) {
                if (Dominates(population[i], population[j])) {
                    population[i].dominated_set.push_back(&population[j]);
                    population[j].dominated_count++;
                } else if (Dominates(population[j], population[i])) {
                    population[j].dominated_set.push_back(&population[i]);
                    population[i].dominated_count++;
                }
            }
        }
        
        std::vector<Chromosome> fronts;
        int current_rank = 0;
        
        for (auto& individual : population) {
            if (individual.dominated_count == 0) {
                individual.rank = current_rank;
                fronts.push_back(individual);
            }
        }
        
        return fronts;
    }
    
    bool Dominates(const Chromosome& a, const Chromosome& b) {
        bool dominated = false;
        
        for (size_t i = 0; i < a.fitness_values.size(); ++i) {
            if (a.fitness_values[i] > b.fitness_values[i]) {
                return false;
            }
            if (a.fitness_values[i] < b.fitness_values[i]) {
                dominated = true;
            }
        }
        
        return dominated;
    }
    
    void CalculateCrowdingDistance(std::vector<Chromosome>& front) {
        if (front.empty()) return;
        
        for (auto& individual : front) {
            individual.crowding_distance = 0;
        }
        
        for (size_t obj = 0; obj < front[0].fitness_values.size(); ++obj) {
            std::sort(front.begin(), front.end(),
                [obj](const Chromosome& a, const Chromosome& b) {
                    return a.fitness_values[obj] < b.fitness_values[obj];
                });
            
            front.front().crowding_distance = 
                std::numeric_limits<double>::max();
            front.back().crowding_distance = 
                std::numeric_limits<double>::max();
            
            double obj_range = front.back().fitness_values[obj] - 
                              front.front().fitness_values[obj];
            
            if (obj_range > 0) {
                for (size_t i = 1; i < front.size() - 1; ++i) {
                    front[i].crowding_distance += 
                        (front[i+1].fitness_values[obj] - 
                         front[i-1].fitness_values[obj]) / obj_range;
                }
            }
        }
    }
    
    std::vector<Chromosome> GenerateOffspring(
        const std::vector<Chromosome>& population) {
        
        std::vector<Chromosome> offspring;
        offspring.reserve(config_.population_size);
        
        while (offspring.size() < static_cast<size_t>(config_.population_size)) {
            Chromosome parent1 = TournamentSelection(population);
            Chromosome parent2 = TournamentSelection(population);
            
            if (RandomDouble(0, 1) < config_.crossover_rate) {
                auto children = Crossover(parent1, parent2);
                offspring.push_back(children.first);
                offspring.push_back(children.second);
            } else {
                offspring.push_back(parent1);
                offspring.push_back(parent2);
            }
        }
        
        for (auto& individual : offspring) {
            if (RandomDouble(0, 1) < config_.mutation_rate) {
                Mutate(individual);
            }
        }
        
        return offspring;
    }
    
    Chromosome TournamentSelection(const std::vector<Chromosome>& population) {
        int tournament_size = 5;
        Chromosome best;
        double best_fitness = std::numeric_limits<double>::max();
        
        for (int i = 0; i < tournament_size; ++i) {
            int idx = RandomInt(0, population.size() - 1);
            double fitness = population[idx].fitness_values[0];
            
            if (fitness < best_fitness) {
                best_fitness = fitness;
                best = population[idx];
            }
        }
        
        return best;
    }
    
    std::pair<Chromosome, Chromosome> Crossover(
        const Chromosome& parent1, const Chromosome& parent2) {
        
        Chromosome child1, child2;
        
        size_t min_size = std::min(parent1.genes.size(), parent2.genes.size());
        size_t crossover_point = RandomInt(1, min_size - 2);
        
        child1.genes.insert(child1.genes.end(),
            parent1.genes.begin(), parent1.genes.begin() + crossover_point);
        child1.genes.insert(child1.genes.end(),
            parent2.genes.begin() + crossover_point, parent2.genes.end());
        
        child2.genes.insert(child2.genes.end(),
            parent2.genes.begin(), parent2.genes.begin() + crossover_point);
        child2.genes.insert(child2.genes.end(),
            parent1.genes.begin() + crossover_point, parent1.genes.end());
        
        return {child1, child2};
    }
    
    void Mutate(Chromosome& individual) {
        int mutation_type = RandomInt(0, 2);
        
        switch (mutation_type) {
            case 0:
                MutatePoint(individual);
                break;
            case 1:
                MutateAddPoint(individual);
                break;
            case 2:
                MutateRemovePoint(individual);
                break;
        }
    }
    
    void MutatePoint(Chromosome& individual) {
        if (individual.genes.size() <= 2) return;
        
        int idx = RandomInt(1, individual.genes.size() - 2);
        individual.genes[idx].longitude += RandomDouble(-0.01, 0.01);
        individual.genes[idx].latitude += RandomDouble(-0.01, 0.01);
    }
    
    void MutateAddPoint(Chromosome& individual) {
        if (individual.genes.size() >= 20) return;
        
        int idx = RandomInt(1, individual.genes.size() - 1);
        Coordinate new_point(
            (individual.genes[idx-1].longitude + individual.genes[idx].longitude) / 2,
            (individual.genes[idx-1].latitude + individual.genes[idx].latitude) / 2
        );
        new_point.longitude += RandomDouble(-0.01, 0.01);
        new_point.latitude += RandomDouble(-0.01, 0.01);
        
        individual.genes.insert(individual.genes.begin() + idx, new_point);
    }
    
    void MutateRemovePoint(Chromosome& individual) {
        if (individual.genes.size() <= 3) return;
        
        int idx = RandomInt(1, individual.genes.size() - 2);
        individual.genes.erase(individual.genes.begin() + idx);
    }
    
    std::vector<Route> ExtractParetoFront(
        const std::vector<Chromosome>& population) {
        
        std::vector<Route> pareto_routes;
        
        for (const auto& individual : population) {
            if (individual.rank == 0) {
                Route route;
                for (const auto& coord : individual.genes) {
                    Waypoint wp;
                    wp.longitude = coord.longitude;
                    wp.latitude = coord.latitude;
                    route.AddWaypoint(wp);
                }
                pareto_routes.push_back(route);
            }
        }
        
        return pareto_routes;
    }
    
    bool CheckConvergence(const std::vector<Chromosome>& fronts) {
        if (fronts.empty()) return false;
        
        double avg_fitness = 0;
        for (const auto& individual : fronts) {
            avg_fitness += individual.fitness_values[0];
        }
        avg_fitness /= fronts.size();
        
        return avg_fitness < config_.convergence_threshold;
    }
    
    double Distance(const Coordinate& a, const Coordinate& b) {
        return CoordinateConverter::Instance().CalculateGreatCircleDistance(
            a.latitude, a.longitude, b.latitude, b.longitude);
    }
    
    double CalculateTurnAngle(const Coordinate& prev, 
                             const Coordinate& current,
                             const Coordinate& next) {
        double bearing1 = CoordinateConverter::Instance().CalculateBearing(
            prev.latitude, prev.longitude,
            current.latitude, current.longitude);
        double bearing2 = CoordinateConverter::Instance().CalculateBearing(
            current.latitude, current.longitude,
            next.latitude, next.longitude);
        
        double angle = std::abs(bearing2 - bearing1);
        if (angle > 180) angle = 360 - angle;
        return angle;
    }
    
    int RandomInt(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(rng_);
    }
    
    double RandomDouble(double min, double max) {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(rng_);
    }
    
    IChartDatabase* chart_db_ = nullptr;
    MultiObjectiveConfig config_;
    std::mt19937 rng_{std::random_device{}()};
};
```

### 11.6 算法选择策略

根据不同场景自动选择最优算法：

```cpp
enum class PlanningScenario {
    OPEN_SEA,           // 开阔海域
    RESTRICTED_WATER,   // 受限水域
    COMPLEX_OBSTACLES,  // 复杂障碍环境
    MULTI_OBJECTIVE,    // 多目标优化
    DYNAMIC_ENVIRONMENT // 动态环境
};

class AlgorithmSelector {
public:
    IPlanner* SelectAlgorithm(const PlanningScenario& scenario,
                              const PlanningConstraints& constraints) {
        switch (scenario) {
            case PlanningScenario::OPEN_SEA:
                return &a_star_planner_;
                
            case PlanningScenario::RESTRICTED_WATER:
                return &memory_optimized_planner_;
                
            case PlanningScenario::COMPLEX_OBSTACLES:
                return &rrt_star_planner_;
                
            case PlanningScenario::MULTI_OBJECTIVE:
                return &genetic_planner_;
                
            case PlanningScenario::DYNAMIC_ENVIRONMENT:
                return &rrt_star_planner_;
                
            default:
                return &a_star_planner_;
        }
    }
    
    PlanningScenario DetectScenario(const Coordinate& start,
                                   const Coordinate& end,
                                   const PlanningFactors& factors) {
        if (factors.obstacle_density > 0.3) {
            return PlanningScenario::COMPLEX_OBSTACLES;
        }
        
        if (factors.has_dynamic_obstacles) {
            return PlanningScenario::DYNAMIC_ENVIRONMENT;
        }
        
        if (factors.optimization_objectives.size() > 1) {
            return PlanningScenario::MULTI_OBJECTIVE;
        }
        
        if (factors.channel_width < 200.0) {
            return PlanningScenario::RESTRICTED_WATER;
        }
        
        return PlanningScenario::OPEN_SEA;
    }
    
private:
    AStarPlanner a_star_planner_;
    MemoryOptimizedAStarPlanner memory_optimized_planner_;
    RrtStarPlanner rrt_star_planner_;
    GeneticAlgorithmPlanner genetic_planner_;
};
```

---

## 十二、数据库优化设计

### 12.1 空间索引支持

```sql
CREATE VIRTUAL TABLE track_points_rtree USING rtree(
    id,
    min_longitude, max_longitude,
    min_latitude, max_latitude
);

CREATE INDEX idx_track_points_spatial ON track_points_rtree(
    min_longitude, max_longitude, min_latitude, max_latitude
);

CREATE INDEX idx_track_points_composite ON track_points(
    track_id, timestamp, longitude, latitude
);

CREATE INDEX idx_routes_composite ON routes(
    created_time, modified_time, departure_port, arrival_port
);
```

### 12.2 数据库版本管理

```cpp
struct DatabaseVersion {
    int major;
    int minor;
    int patch;
    std::string description;
};

class DatabaseMigrationManager {
public:
    static DatabaseMigrationManager& Instance();
    
    bool Initialize(const std::string& db_path);
    DatabaseVersion GetCurrentVersion();
    bool MigrateToVersion(const DatabaseVersion& target);
    bool IsMigrationNeeded();
    
    void RegisterMigration(const DatabaseVersion& from, 
                          const DatabaseVersion& to,
                          MigrationFunction migration_func);
    
private:
    bool ExecuteMigration(const DatabaseVersion& from, 
                         const DatabaseVersion& to);
    bool BackupBeforeMigration();
    bool ValidateMigration(const DatabaseVersion& version);
    
    std::map<std::pair<DatabaseVersion, DatabaseVersion>, 
             MigrationFunction> migrations_;
    DatabaseVersion current_version_;
    std::unique_ptr<Database> database_;
};

using MigrationFunction = std::function<bool(Database&)>;

class DatabaseMigrations {
public:
    static void RegisterAllMigrations() {
        auto& manager = DatabaseMigrationManager::Instance();
        
        manager.RegisterMigration(
            {1, 0, 0, "Initial version"},
            {1, 1, 0, "Add spatial index"},
            [](Database& db) {
                db.Execute(
                    "CREATE VIRTUAL TABLE IF NOT EXISTS track_points_rtree "
                    "USING rtree(id, min_longitude, max_longitude, "
                    "min_latitude, max_latitude)");
                return true;
            });
        
        manager.RegisterMigration(
            {1, 1, 0, "Add spatial index"},
            {1, 2, 0, "Add audit log table"},
            [](Database& db) {
                db.Execute(
                    "CREATE TABLE IF NOT EXISTS audit_log ("
                    "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "  timestamp REAL NOT NULL,"
                    "  user_id TEXT,"
                    "  operation TEXT NOT NULL,"
                    "  table_name TEXT NOT NULL,"
                    "  record_id TEXT,"
                    "  old_value BLOB,"
                    "  new_value BLOB,"
                    "  ip_address TEXT"
                    ")");
                return true;
            });
    }
};
```

### 12.3 数据备份恢复

```cpp
class DatabaseBackupManager {
public:
    struct BackupConfig {
        std::string backup_dir;
        int max_backups = 10;
        bool compress = true;
        bool include_wal = true;
    };
    
    bool CreateBackup(const std::string& backup_name = "");
    bool RestoreBackup(const std::string& backup_file);
    std::vector<BackupInfo> ListBackups();
    bool DeleteBackup(const std::string& backup_file);
    bool ValidateBackup(const std::string& backup_file);
    
    void SetAutoBackup(bool enabled, int interval_hours = 24);
    
private:
    bool BackupDatabase(const std::string& backup_path);
    bool BackupWalFile(const std::string& backup_path);
    bool CompressBackup(const std::string& backup_path);
    bool VerifyBackupIntegrity(const std::string& backup_path);
    
    BackupConfig config_;
    std::unique_ptr<Database> database_;
};

struct BackupInfo {
    std::string filename;
    std::string path;
    double created_time;
    size_t size_bytes;
    bool is_valid;
    std::string checksum;
};
```

---

## 十三、安全增强设计

### 13.1 数据隐私保护

```cpp
class PrivacyProtectionManager {
public:
    enum class PrivacyLevel {
        None,
        Low,
        Medium,
        High
    };
    
    struct PrivacyConfig {
        PrivacyLevel position_privacy = PrivacyLevel::Medium;
        PrivacyLevel track_privacy = PrivacyLevel::Medium;
        bool anonymize_export = true;
        double position_precision = 0.001;
    };
    
    static PrivacyProtectionManager& Instance();
    
    void SetPrivacyConfig(const PrivacyConfig& config);
    PrivacyConfig GetPrivacyConfig() const;
    
    Coordinate AnonymizePosition(const Coordinate& coord);
    std::string AnonymizeShipName(const std::string& name);
    int AnonymizeMmsi(int mmsi);
    
    Track AnonymizeTrack(const Track& track);
    Route AnonymizeRoute(const Route& route);
    
    bool ExportWithPrivacy(void* data, const std::string& format);
    
private:
    Coordinate ApplyPrecision(const Coordinate& coord, double precision);
    std::string ApplyHashing(const std::string& value, const std::string& salt);
    int ApplyTokenization(int value, int token_range);
    
    PrivacyConfig config_;
    std::string salt_;
};

class PositionPrivacyFilter {
public:
    PositionData Filter(const PositionData& data, PrivacyLevel level) {
        PositionData filtered = data;
        
        switch (level) {
            case PrivacyLevel::High:
                filtered.longitude = RoundToPrecision(data.longitude, 0.01);
                filtered.latitude = RoundToPrecision(data.latitude, 0.01);
                filtered.speed = RoundToPrecision(data.speed, 1.0);
                break;
                
            case PrivacyLevel::Medium:
                filtered.longitude = RoundToPrecision(data.longitude, 0.001);
                filtered.latitude = RoundToPrecision(data.latitude, 0.001);
                break;
                
            case PrivacyLevel::Low:
                filtered.longitude = RoundToPrecision(data.longitude, 0.0001);
                filtered.latitude = RoundToPrecision(data.latitude, 0.0001);
                break;
                
            case PrivacyLevel::None:
                break;
        }
        
        return filtered;
    }
    
private:
    double RoundToPrecision(double value, double precision) {
        return std::round(value / precision) * precision;
    }
};
```

### 13.2 安全审计日志

```cpp
enum class AuditOperation {
    CREATE,
    READ,
    UPDATE,
    DELETE,
    EXPORT,
    IMPORT,
    LOGIN,
    LOGOUT,
    CONFIG_CHANGE,
    NAVIGATION_START,
    NAVIGATION_STOP,
    ROUTE_MODIFY
};

struct AuditLogEntry {
    int64_t id;
    double timestamp;
    std::string user_id;
    std::string session_id;
    AuditOperation operation;
    std::string table_name;
    std::string record_id;
    std::string old_value;
    std::string new_value;
    std::string ip_address;
    std::string user_agent;
    bool success;
    std::string error_message;
};

class AuditLogger {
public:
    static AuditLogger& Instance();
    
    bool Initialize(const std::string& db_path);
    void Shutdown();
    
    void LogOperation(AuditOperation operation,
                     const std::string& table_name,
                     const std::string& record_id,
                     const std::string& old_value = "",
                     const std::string& new_value = "");
    
    void LogNavigationEvent(AuditOperation operation,
                           const std::string& route_id,
                           const std::string& details = "");
    
    void LogSecurityEvent(AuditOperation operation,
                         bool success,
                         const std::string& error_message = "");
    
    std::vector<AuditLogEntry> QueryLogs(
        double start_time, double end_time,
        const std::string& user_id = "",
        AuditOperation operation = AuditOperation::READ);
    
    std::vector<AuditLogEntry> QueryByRecord(
        const std::string& table_name,
        const std::string& record_id);
    
    void ExportLogs(const std::string& filepath,
                   double start_time, double end_time);
    
    void CleanOldLogs(int days_to_keep = 90);
    
private:
    void WriteLog(const AuditLogEntry& entry);
    std::string SerializeValue(const std::string& value);
    
    std::unique_ptr<Database> database_;
    std::mutex log_mutex_;
    bool enabled_ = true;
};

class AuditedRouteManager : public RouteManager {
public:
    std::string CreateRoute(const Route& route) override {
        std::string route_id = RouteManager::CreateRoute(route);
        
        AuditLogger::Instance().LogOperation(
            AuditOperation::CREATE,
            "routes",
            route_id,
            "",
            route.ToJson());
        
        return route_id;
    }
    
    bool UpdateRoute(const std::string& route_id, const Route& route) override {
        Route old_route = GetRoute(route_id);
        bool success = RouteManager::UpdateRoute(route_id, route);
        
        if (success) {
            AuditLogger::Instance().LogOperation(
                AuditOperation::UPDATE,
                "routes",
                route_id,
                old_route.ToJson(),
                route.ToJson());
        }
        
        return success;
    }
    
    bool DeleteRoute(const std::string& route_id) override {
        Route old_route = GetRoute(route_id);
        bool success = RouteManager::DeleteRoute(route_id);
        
        if (success) {
            AuditLogger::Instance().LogOperation(
                AuditOperation::DELETE,
                "routes",
                route_id,
                old_route.ToJson(),
                "");
        }
        
        return success;
    }
};
```

### 13.3 AIS数据验证

```cpp
class AisDataValidator {
public:
    struct ValidationResult {
        bool is_valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        double confidence_score;
    };
    
    ValidationResult Validate(const AisMessage& message);
    ValidationResult ValidatePositionReport(const AisPositionReport& report);
    ValidationResult ValidateStaticData(const AisStaticData& data);
    
    void SetValidationRules(const ValidationRules& rules);
    
private:
    bool ValidateMmsi(int mmsi);
    bool ValidateCoordinate(double longitude, double latitude);
    bool ValidateSpeed(double speed);
    bool ValidateCourse(double course);
    bool CheckPositionJump(const AisPositionReport& report);
    bool CheckSpeedAnomaly(double speed, double previous_speed, double time_diff);
    bool VerifyChecksum(const std::string& message);
    
    std::map<int, AisPositionReport> last_positions_;
    ValidationRules rules_;
};

struct ValidationRules {
    double max_speed = 30.0;
    double max_acceleration = 5.0;
    double max_position_jump = 10.0;
    int min_mmsi = 100000000;
    int max_mmsi = 999999999;
    bool require_checksum = true;
    bool check_temporal_consistency = true;
};

class AisDataValidator {
public:
    ValidationResult ValidatePositionReport(const AisPositionReport& report) {
        ValidationResult result;
        result.is_valid = true;
        result.confidence_score = 1.0;
        
        if (!ValidateMmsi(report.mmsi)) {
            result.errors.push_back("Invalid MMSI: " + std::to_string(report.mmsi));
            result.is_valid = false;
            result.confidence_score *= 0.5;
        }
        
        if (!ValidateCoordinate(report.longitude, report.latitude)) {
            result.errors.push_back("Invalid coordinates");
            result.is_valid = false;
            result.confidence_score *= 0.3;
        }
        
        if (!ValidateSpeed(report.speed)) {
            result.warnings.push_back("Abnormal speed: " + 
                std::to_string(report.speed) + " knots");
            result.confidence_score *= 0.8;
        }
        
        if (rules_.check_temporal_consistency) {
            auto it = last_positions_.find(report.mmsi);
            if (it != last_positions_.end()) {
                double time_diff = report.timestamp - it->second.timestamp;
                if (time_diff > 0 && time_diff < 3600) {
                    if (CheckPositionJump(report)) {
                        result.warnings.push_back("Position jump detected");
                        result.confidence_score *= 0.7;
                    }
                    
                    if (CheckSpeedAnomaly(report.speed, it->second.speed, time_diff)) {
                        result.warnings.push_back("Speed anomaly detected");
                        result.confidence_score *= 0.8;
                    }
                }
            }
        }
        
        last_positions_[report.mmsi] = report;
        
        return result;
    }
    
private:
    bool CheckPositionJump(const AisPositionReport& report) {
        auto it = last_positions_.find(report.mmsi);
        if (it == last_positions_.end()) return false;
        
        double dist = CoordinateConverter::Instance().CalculateGreatCircleDistance(
            it->second.latitude, it->second.longitude,
            report.latitude, report.longitude);
        
        double time_diff = report.timestamp - it->second.timestamp;
        double max_dist = rules_.max_position_jump * (time_diff / 3600.0);
        
        return dist > max_dist;
    }
};
```

---

## 十四、扩展功能设计

### 14.1 潮汐预测集成

```cpp
struct TideStation {
    std::string id;
    std::string name;
    Coordinate position;
    std::vector<HarmonicConstant> harmonics;
    double datum_offset;
};

struct HarmonicConstant {
    std::string component_name;
    double amplitude;
    double phase;
    double speed;
};

struct TidePrediction {
    double timestamp;
    double water_level;
    double trend;
    std::string tide_state;
};

class ITidePredictor {
public:
    virtual ~ITidePredictor() = default;
    
    virtual bool Initialize(const std::vector<TideStation>& stations) = 0;
    virtual TidePrediction Predict(const Coordinate& position, 
                                   double timestamp) = 0;
    virtual std::vector<TidePrediction> PredictRange(
        const Coordinate& position,
        double start_time, double end_time,
        double interval = 3600) = 0;
    virtual double GetMaxTide(const Coordinate& position,
                             double start_time, double end_time) = 0;
    virtual double GetMinTide(const Coordinate& position,
                             double start_time, double end_time) = 0;
};

class HarmonicTidePredictor : public ITidePredictor {
public:
    bool Initialize(const std::vector<TideStation>& stations) override {
        for (const auto& station : stations) {
            stations_[station.id] = station;
        }
        return true;
    }
    
    TidePrediction Predict(const Coordinate& position, 
                          double timestamp) override {
        TideStation nearest = FindNearestStation(position);
        
        double water_level = 0.0;
        for (const auto& harmonic : nearest.harmonics) {
            double angle = harmonic.speed * timestamp + harmonic.phase;
            water_level += harmonic.amplitude * std::cos(angle * M_PI / 180.0);
        }
        
        water_level += nearest.datum_offset;
        
        TidePrediction prediction;
        prediction.timestamp = timestamp;
        prediction.water_level = water_level;
        prediction.trend = CalculateTrend(position, timestamp);
        prediction.tide_state = DetermineTideState(water_level, nearest);
        
        return prediction;
    }
    
private:
    TideStation FindNearestStation(const Coordinate& position) {
        double min_dist = std::numeric_limits<double>::max();
        TideStation nearest;
        
        for (const auto& pair : stations_) {
            double dist = CoordinateConverter::Instance()
                .CalculateGreatCircleDistance(
                    position.latitude, position.longitude,
                    pair.second.position.latitude, 
                    pair.second.position.longitude);
            if (dist < min_dist) {
                min_dist = dist;
                nearest = pair.second;
            }
        }
        
        return nearest;
    }
    
    std::map<std::string, TideStation> stations_;
};

class UkcCalculatorWithTide : public UkcCalculator {
public:
    void SetTidePredictor(std::shared_ptr<ITidePredictor> predictor) {
        tide_predictor_ = predictor;
    }
    
    UkcComponents CalculateWithTide(const UkcInput& input, double timestamp) {
        UkcComponents ukc = Calculate(input);
        
        if (tide_predictor_) {
            Coordinate position(input.longitude, input.latitude);
            TidePrediction tide = tide_predictor_->Predict(position, timestamp);
            ukc.water_level_correction += tide.water_level;
        }
        
        return ukc;
    }
    
private:
    std::shared_ptr<ITidePredictor> tide_predictor_;
};
```

### 14.2 多源定位融合

```cpp
struct PositionSourceData {
    PositionSource source;
    PositionData data;
    double weight;
    double accuracy;
    double timestamp;
};

class MultiSourcePositionFusion {
public:
    struct FusionConfig {
        bool use_kalman_filter = true;
        double max_source_age = 5.0;
        double min_source_weight = 0.1;
        std::map<PositionSource, double> source_weights = {
            {PositionSource::System, 0.3},
            {PositionSource::Serial, 0.5},
            {PositionSource::Bluetooth, 0.4},
            {PositionSource::Network, 0.2}
        };
    };
    
    PositionData Fuse(const std::vector<PositionSourceData>& sources) {
        if (sources.empty()) {
            return PositionData();
        }
        
        if (config_.use_kalman_filter) {
            return KalmanFusion(sources);
        } else {
            return WeightedAverageFusion(sources);
        }
    }
    
    void SetConfig(const FusionConfig& config) {
        config_ = config;
    }
    
private:
    PositionData WeightedAverageFusion(
        const std::vector<PositionSourceData>& sources) {
        
        double weight_sum = 0;
        double lon_sum = 0;
        double lat_sum = 0;
        double heading_sum = 0;
        double speed_sum = 0;
        
        for (const auto& source : sources) {
            double weight = CalculateWeight(source);
            
            weight_sum += weight;
            lon_sum += source.data.longitude * weight;
            lat_sum += source.data.latitude * weight;
            heading_sum += source.data.heading * weight;
            speed_sum += source.data.speed * weight;
        }
        
        PositionData fused;
        fused.longitude = lon_sum / weight_sum;
        fused.latitude = lat_sum / weight_sum;
        fused.heading = heading_sum / weight_sum;
        fused.speed = speed_sum / weight_sum;
        fused.timestamp = GetCurrentTime();
        fused.quality = DetermineFusedQuality(sources);
        
        return fused;
    }
    
    PositionData KalmanFusion(
        const std::vector<PositionSourceData>& sources) {
        
        if (!kalman_initialized_) {
            InitializeKalmanFilter(sources[0].data);
        }
        
        for (const auto& source : sources) {
            UpdateKalmanFilter(source.data, source.accuracy);
        }
        
        return GetKalmanState();
    }
    
    double CalculateWeight(const PositionSourceData& source) {
        double base_weight = config_.source_weights[source.source];
        double accuracy_factor = 1.0 / (source.accuracy + 1.0);
        double age_factor = 1.0 / (GetCurrentTime() - source.timestamp + 1.0);
        
        return base_weight * accuracy_factor * age_factor;
    }
    
    FusionConfig config_;
    bool kalman_initialized_ = false;
    KalmanFilter kalman_filter_;
};
```

### 14.3 轨迹压缩优化

```cpp
class IncrementalTrackCompressor {
public:
    struct CompressionConfig {
        double tolerance = 10.0;
        int window_size = 100;
        int min_points_to_keep = 3;
        bool preserve_events = true;
    };
    
    void AddPoint(const TrackPoint& point) {
        buffer_.push_back(point);
        
        if (buffer_.size() >= config_.window_size) {
            CompressBuffer();
        }
    }
    
    Track GetCompressedTrack() {
        Track track;
        for (const auto& point : compressed_points_) {
            track.AddPoint(point);
        }
        return track;
    }
    
    void SetConfig(const CompressionConfig& config) {
        config_ = config;
    }
    
private:
    void CompressBuffer() {
        std::vector<TrackPoint> compressed = 
            SlidingWindowDouglasPeucker(buffer_, config_.tolerance);
        
        if (config_.preserve_events) {
            compressed = MergeWithEvents(compressed, buffer_);
        }
        
        compressed_points_.insert(compressed_points_.end(),
                                  compressed.begin(), compressed.end());
        buffer_.clear();
    }
    
    std::vector<TrackPoint> SlidingWindowDouglasPeucker(
        const std::vector<TrackPoint>& points, double tolerance) {
        
        if (points.size() <= 2) return points;
        
        std::vector<TrackPoint> result;
        result.push_back(points.front());
        
        size_t start = 0;
        while (start < points.size() - 1) {
            size_t end = FindNextKeyPoint(points, start, tolerance);
            result.push_back(points[end]);
            start = end;
        }
        
        return result;
    }
    
    size_t FindNextKeyPoint(const std::vector<TrackPoint>& points,
                           size_t start, double tolerance) {
        for (size_t end = start + 2; end < points.size(); ++end) {
            double max_dist = 0;
            
            for (size_t i = start + 1; i < end; ++i) {
                double dist = PerpendicularDistance(
                    points[i], points[start], points[end]);
                max_dist = std::max(max_dist, dist);
            }
            
            if (max_dist > tolerance) {
                return end - 1;
            }
        }
        
        return points.size() - 1;
    }
    
    std::vector<TrackPoint> MergeWithEvents(
        const std::vector<TrackPoint>& compressed,
        const std::vector<TrackPoint>& original) {
        
        std::vector<TrackPoint> merged = compressed;
        
        for (const auto& point : original) {
            if (IsKeyEvent(point)) {
                if (!ContainsPoint(merged, point)) {
                    merged.push_back(point);
                }
            }
        }
        
        std::sort(merged.begin(), merged.end(),
                 [](const TrackPoint& a, const TrackPoint& b) {
                     return a.timestamp < b.timestamp;
                 });
        
        return merged;
    }
    
    CompressionConfig config_;
    std::vector<TrackPoint> buffer_;
    std::vector<TrackPoint> compressed_points_;
};
```

---

## 十五、附录

### 15.1 术语表

| 术语 | 英文 | 说明 |
|-----|------|------|
| UKC | Under Keel Clearance | 富余水深 |
| CPA | Closest Point of Approach | 最近会遇距离 |
| TCPA | Time to CPA | 到达最近会遇点时间 |
| XTE | Cross Track Error | 偏航距离 |
| COG | Course Over Ground | 对地航向 |
| SOG | Speed Over Ground | 对地速度 |
| ETA | Estimated Time of Arrival | 预计到达时间 |
| AIS | Automatic Identification System | 自动识别系统 |
| NMEA | National Marine Electronics Association | 海洋电子协会标准 |

### 15.2 参考文档

- [chart_navi_system.md](../../doc/yangzt/chart_navi_system.md) - 需求说明书
- [IEC 61162-1](https://www.iec.ch/) - NMEA 0183标准
- [IEC 61162-2](https://www.iec.ch/) - NMEA 2000标准
- [IMO SN.1/Circ.289](https://www.imo.org/) - AIS指南

---

**文档版本**: v1.2  
**最后更新**: 2026-04-03
