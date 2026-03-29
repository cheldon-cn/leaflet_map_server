# 潮汐API和气象API的获取对接详述文档

## 一、概述

### 1.1 文档目的

本文档详细说明中国沿海和长江干线的潮汐API和气象API的获取方式、对接流程、数据结构及示例代码，为海图导航系统的数据对接提供技术参考。

### 1.2 适用范围

| 区域 | API类型 | 数据来源 |
|-----|---------|---------|
| 中国沿海 | 潮汐API | 国家海洋信息中心、海洋站 |
| 长江干线 | 水位API | 长江水利委员会、水文站 |
| 全国范围 | 气象API | 中国气象局、气象站 |
| 局部区域 | 实时气象 | 地方气象局 |

## 二、潮汐API对接

### 2.1 中国沿海潮汐API

#### 2.1.1 数据来源

| 来源 | 覆盖范围 | 数据类型 | 获取方式 |
|-----|---------|---------|---------|
| 国家海洋信息中心 | 全国沿海 | 潮汐预报 | API申请 |
| 海洋站实时数据 | 主要港口 | 实时潮位 | 数据共享 |
| 潮汐表 | 主要港口 | 潮汐预报 | 公开数据 |

#### 2.1.2 国家海洋信息中心API

**接口地址：**
```
https://api.nmdis.org.cn/tide/forecast
```

**申请流程：**

| 步骤 | 操作 | 说明 |
|-----|-----|-----|
| 1 | 注册账号 | 访问国家海洋信息中心官网注册 |
| 2 | 提交申请 | 填写数据使用申请表 |
| 3 | 审核通过 | 等待审核，一般7-15个工作日 |
| 4 | 获取密钥 | 获得API访问密钥（AppKey） |

**请求参数：**

| 参数名 | 类型 | 必填 | 说明 |
|-------|-----|-----|-----|
| appKey | string | 是 | API访问密钥 |
| stationCode | string | 是 | 潮汐站代码 |
| startDate | string | 是 | 开始日期（YYYY-MM-DD） |
| endDate | string | 是 | 结束日期（YYYY-MM-DD） |
| dataType | string | 否 | 数据类型（forecast/realtime） |

**请求示例：**
```
GET https://api.nmdis.org.cn/tide/forecast?appKey=YOUR_KEY&stationCode=SHG&startDate=2024-05-20&endDate=2024-05-21
```

**响应数据结构：**
```json
{
  "code": 200,
  "message": "success",
  "data": {
    "stationCode": "SHG",
    "stationName": "上海港",
    "longitude": 121.49,
    "latitude": 31.23,
    "datum": "理论深度基准面",
    "forecasts": [
      {
        "date": "2024-05-20",
        "tides": [
          {
            "time": "00:15",
            "type": "high",
            "height": 3.52
          },
          {
            "time": "06:30",
            "type": "low",
            "height": 0.85
          },
          {
            "time": "12:45",
            "type": "high",
            "height": 3.28
          },
          {
            "time": "18:55",
            "type": "low",
            "height": 1.12
          }
        ]
      }
    ]
  }
}
```

#### 2.1.3 主要港口潮汐站代码

| 港口 | 站代码 | 经度 | 纬度 |
|-----|-------|-----|-----|
| 大连 | DLC | 121.62 | 38.92 |
| 天津 | TJP | 117.78 | 39.00 |
| 青岛 | QDP | 120.30 | 36.08 |
| 上海 | SHG | 121.49 | 31.23 |
| 宁波 | NBP | 121.55 | 29.87 |
| 厦门 | XMP | 118.07 | 24.45 |
| 广州 | GZP | 113.52 | 23.08 |
| 深圳 | SZP | 114.30 | 22.40 |

### 2.2 长江干线水位API

#### 2.2.1 数据来源

| 来源 | 覆盖范围 | 数据类型 | 获取方式 |
|-----|---------|---------|---------|
| 长江水利委员会 | 长江干流 | 实时水位 | 数据共享申请 |
| 全国水雨情信息网 | 全国河流 | 水位、流量 | 公开API |
| 长江航道局 | 长江航道 | 航道水深 | 服务订阅 |

#### 2.2.2 全国水雨情信息网API

**接口地址：**
```
http://xxfb.hydroinfo.gov.cn/api/v1/water
```

**请求参数：**

| 参数名 | 类型 | 必填 | 说明 |
|-------|-----|-----|-----|
| token | string | 是 | 访问令牌 |
| stcd | string | 是 | 测站代码 |
| type | string | 是 | 数据类型（Z/Q） |
| start | string | 是 | 开始时间 |
| end | string | 是 | 结束时间 |

**响应数据结构：**
```json
{
  "status": "success",
  "data": {
    "station": {
      "stcd": "60105000",
      "name": "汉口站",
      "river": "长江",
      "longitude": 114.30,
      "latitude": 30.58
    },
    "records": [
      {
        "time": "2024-05-20 08:00",
        "waterLevel": 18.52,
        "flow": 28500,
        "trend": "rising"
      },
      {
        "time": "2024-05-20 09:00",
        "waterLevel": 18.58,
        "flow": 28800,
        "trend": "rising"
      }
    ]
  }
}
```

#### 2.2.3 长江主要水文站代码

| 站名 | 站代码 | 位置 | 基准面 |
|-----|-------|-----|-------|
| 宜昌站 | 60103000 | 湖北宜昌 | 黄海基准 |
| 沙市站 | 60104000 | 湖北荆州 | 黄海基准 |
| 汉口站 | 60105000 | 湖北武汉 | 黄海基准 |
| 九江站 | 60106000 | 江西九江 | 黄海基准 |
| 大通站 | 60107000 | 安徽安庆 | 黄海基准 |
| 南京站 | 60108000 | 江苏南京 | 黄海基准 |

#### 2.2.4 长江航道局数据服务

**服务地址：**
```
https://www.cjhdj.com.cn/api/channel
```

**申请流程：**

| 步骤 | 操作 | 说明 |
|-----|-----|-----|
| 1 | 企业注册 | 长江e+平台企业认证 |
| 2 | 签署协议 | 数据使用协议签署 |
| 3 | 缴纳费用 | 按服务类型缴费 |
| 4 | 开通服务 | 获取API访问权限 |

**数据类型：**

| 数据类型 | 说明 | 更新频率 |
|---------|-----|---------|
| 航道水深 | 各河段维护水深 | 每周 |
| 航道宽度 | 航道宽度信息 | 每周 |
| 航标信息 | 航标位置和状态 | 实时 |
| 通告信息 | 航道通告 | 按需 |

### 2.3 潮汐计算算法

#### 2.3.1 调和常数法

潮汐预报采用调和分析法，公式如下：

```
h(t) = H₀ + Σ(fᵢHᵢcos[σᵢt + (V₀+u)ᵢ - gᵢ])
```

其中：
- h(t)：t时刻的潮高
- H₀：平均海面
- Hᵢ：第i个分潮的振幅
- σᵢ：第i个分潮的角速度
- gᵢ：第i个分潮的迟角
- fᵢ：节点因子
- uᵢ：天文相角修正

#### 2.3.2 主要分潮

| 分潮符号 | 名称 | 周期(小时) | 相对重要性 |
|---------|-----|-----------|-----------|
| M2 | 主太阴半日分潮 | 12.42 | 最重要 |
| S2 | 主太阳半日分潮 | 12.00 | 重要 |
| K1 | 太阴太阳全日分潮 | 23.93 | 重要 |
| O1 | 主太阴全日分潮 | 25.82 | 重要 |
| N2 | 太阴椭圆半日分潮 | 12.66 | 较重要 |
| P1 | 主太阳全日分潮 | 24.07 | 较重要 |

## 三、气象API对接

### 3.1 中国气象局API

#### 3.1.1 数据来源

| 来源 | 数据类型 | 覆盖范围 | 获取方式 |
|-----|---------|---------|---------|
| 中国气象局 | 天气预报 | 全国 | API申请 |
| 气象数据中心 | 历史数据 | 全国 | 数据订购 |
| 区域气象中心 | 区域数据 | 各区域 | 数据共享 |

#### 3.1.2 气象局API接口

**接口地址：**
```
https://api.cma.gov.cn/weather
```

**申请流程：**

| 步骤 | 操作 | 说明 |
|-----|-----|-----|
| 1 | 注册账号 | 中国气象数据网注册 |
| 2 | 实名认证 | 个人或企业实名认证 |
| 3 | 申请接口 | 选择需要的API接口 |
| 4 | 审核通过 | 获取API Key |

**请求参数：**

| 参数名 | 类型 | 必填 | 说明 |
|-------|-----|-----|-----|
| apiKey | string | 是 | API密钥 |
| location | string | 是 | 位置（经纬度或城市代码） |
| type | string | 是 | 数据类型 |
| lang | string | 否 | 语言（zh_CN/en） |

**请求示例：**
```
GET https://api.cma.gov.cn/weather?apiKey=YOUR_KEY&location=101010100&type=forecast
```

**响应数据结构：**
```json
{
  "code": 200,
  "message": "success",
  "data": {
    "location": {
      "id": "101010100",
      "name": "北京",
      "longitude": 116.41,
      "latitude": 39.90
    },
    "now": {
      "temp": 25,
      "feelsLike": 27,
      "text": "晴",
      "windDir": "东南",
      "windScale": "3",
      "windSpeed": 15,
      "humidity": 45,
      "pressure": 1013,
      "visibility": 10,
      "cloud": 20
    },
    "forecast": [
      {
        "date": "2024-05-20",
        "tempMax": 28,
        "tempMin": 18,
        "textDay": "晴",
        "textNight": "晴",
        "windDirDay": "东南",
        "windScaleDay": "3-4",
        "humidity": 50
      }
    ]
  }
}
```

### 3.2 海洋气象API

#### 3.2.1 海洋气象数据类型

| 数据类型 | 说明 | 更新频率 |
|---------|-----|---------|
| 海面风 | 风向、风速 | 每3小时 |
| 海浪 | 波高、波向、周期 | 每3小时 |
| 海雾 | 能见度、雾区 | 每3小时 |
| 海温 | 表层水温 | 每日 |
| 台风 | 台风路径、强度 | 每3小时 |

#### 3.2.2 海洋气象API接口

**接口地址：**
```
https://api.nmefc.cn/marine
```

**请求参数：**

| 参数名 | 类型 | 必填 | 说明 |
|-------|-----|-----|-----|
| key | string | 是 | API密钥 |
| area | string | 是 | 海区代码 |
| elements | string | 是 | 要素列表 |
| time | string | 是 | 时间 |

**响应数据结构：**
```json
{
  "code": 200,
  "data": {
    "area": "东海",
    "time": "2024-05-20 08:00",
    "elements": {
      "wind": {
        "direction": 135,
        "speed": 8.5,
        "gust": 12.0
      },
      "wave": {
        "height": 1.5,
        "direction": 130,
        "period": 6
      },
      "visibility": 15,
      "seaTemp": 22.5
    }
  }
}
```

#### 3.2.3 海区代码

| 海区 | 代码 | 范围 |
|-----|-----|-----|
| 渤海 | BH | 117.5°E-122.5°E, 37°N-41°N |
| 黄海北部 | HHN | 122°E-126°E, 35°N-39°N |
| 黄海南部 | HHS | 119°E-126°E, 31°N-35°N |
| 东海 | DH | 117°E-131°E, 24°N-31°N |
| 台湾海峡 | TWX | 118°E-122°E, 22°N-25°N |
| 南海北部 | NHN | 110°E-120°E, 18°N-22°N |

### 3.3 气象预警API

#### 3.3.1 预警数据结构

```json
{
  "alertId": "W2024052001",
  "alertType": "大风预警",
  "alertLevel": "橙色",
  "title": "大风橙色预警信号",
  "content": "预计未来6小时内，长江口海域将出现10-11级大风",
  "issueTime": "2024-05-20T08:00:00Z",
  "expireTime": "2024-05-20T14:00:00Z",
  "affectedArea": {
    "type": "Polygon",
    "coordinates": [[[121.0, 31.0], [122.0, 31.0], [122.0, 32.0], [121.0, 32.0], [121.0, 31.0]]]
  },
  "source": "上海市气象局",
  "actions": ["船舶回港避风", "停止水上作业"]
}
```

#### 3.3.2 预警级别

| 级别 | 颜色 | 含义 |
|-----|-----|-----|
| 一级 | 红色 | 特别严重 |
| 二级 | 橙色 | 严重 |
| 三级 | 黄色 | 较重 |
| 四级 | 蓝色 | 一般 |

### 3.4 台风路径API

#### 3.4.1 台风信息数据结构

```json
{
  "typhoonId": "202405",
  "nameCn": "梅花",
  "nameEn": "Muifa",
  "number": "202405",
  "tracks": [
    {
      "time": "2024-05-20T08:00:00Z",
      "longitude": 125.5,
      "latitude": 28.3,
      "pressure": 980,
      "windSpeed": 35,
      "moveSpeed": 20,
      "moveDirection": "西北",
      "radius7": 250,
      "radius10": 100,
      "radius12": 50
    }
  ],
  "forecast": [
    {
      "time": "2024-05-21T08:00:00Z",
      "longitude": 124.0,
      "latitude": 30.0,
      "pressure": 970,
      "windSpeed": 40,
      "probability": 0.7
    }
  ]
}
```

## 四、对接实现示例

### 4.1 Python示例代码

#### 4.1.1 潮汐数据获取

```python
import requests
import json
from datetime import datetime, timedelta

class TideAPI:
    def __init__(self, api_key):
        self.api_key = api_key
        self.base_url = "https://api.nmdis.org.cn/tide"
    
    def get_forecast(self, station_code, days=3):
        start_date = datetime.now().strftime("%Y-%m-%d")
        end_date = (datetime.now() + timedelta(days=days)).strftime("%Y-%m-%d")
        
        params = {
            "appKey": self.api_key,
            "stationCode": station_code,
            "startDate": start_date,
            "endDate": end_date,
            "dataType": "forecast"
        }
        
        response = requests.get(f"{self.base_url}/forecast", params=params)
        return response.json()
    
    def get_realtime(self, station_code):
        params = {
            "appKey": self.api_key,
            "stationCode": station_code,
            "dataType": "realtime"
        }
        
        response = requests.get(f"{self.base_url}/realtime", params=params)
        return response.json()

    def calculate_current_height(self, forecast_data, current_time):
        tides = forecast_data['data']['forecasts'][0]['tides']
        for i in range(len(tides) - 1):
            t1 = datetime.strptime(f"{forecast_data['data']['forecasts'][0]['date']} {tides[i]['time']}", "%Y-%m-%d %H:%M")
            t2 = datetime.strptime(f"{forecast_data['data']['forecasts'][0]['date']} {tides[i+1]['time']}", "%Y-%m-%d %H:%M")
            if t1 <= current_time <= t2:
                h1 = tides[i]['height']
                h2 = tides[i+1]['height']
                ratio = (current_time - t1).total_seconds() / (t2 - t1).total_seconds()
                return h1 + (h2 - h1) * ratio
        return None
```

#### 4.1.2 气象数据获取

```python
class WeatherAPI:
    def __init__(self, api_key):
        self.api_key = api_key
        self.base_url = "https://api.cma.gov.cn"
    
    def get_weather(self, location):
        params = {
            "apiKey": self.api_key,
            "location": location,
            "type": "now"
        }
        
        response = requests.get(f"{self.base_url}/weather", params=params)
        return response.json()
    
    def get_marine_weather(self, area_code):
        params = {
            "key": self.api_key,
            "area": area_code,
            "elements": "wind,wave,visibility,seaTemp"
        }
        
        response = requests.get(f"{self.base_url}/marine", params=params)
        return response.json()
    
    def get_alerts(self, area_code):
        params = {
            "apiKey": self.api_key,
            "location": area_code,
            "type": "alert"
        }
        
        response = requests.get(f"{self.base_url}/weather", params=params)
        return response.json()
```

### 4.2 Java示例代码

#### 4.2.1 潮汐数据获取

```java
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.net.URI;
import java.time.LocalDate;
import java.time.format.DateTimeFormatter;

public class TideAPIClient {
    private final String apiKey;
    private final String baseUrl = "https://api.nmdis.org.cn/tide";
    private final HttpClient httpClient;
    
    public TideAPIClient(String apiKey) {
        this.apiKey = apiKey;
        this.httpClient = HttpClient.newHttpClient();
    }
    
    public String getForecast(String stationCode, int days) throws Exception {
        LocalDate today = LocalDate.now();
        LocalDate endDate = today.plusDays(days);
        
        String url = String.format("%s/forecast?appKey=%s&stationCode=%s&startDate=%s&endDate=%s",
            baseUrl, apiKey, stationCode,
            today.format(DateTimeFormatter.ISO_LOCAL_DATE),
            endDate.format(DateTimeFormatter.ISO_LOCAL_DATE));
        
        HttpRequest request = HttpRequest.newBuilder()
            .uri(URI.create(url))
            .GET()
            .build();
        
        HttpResponse<String> response = httpClient.send(request, 
            HttpResponse.BodyHandlers.ofString());
        
        return response.body();
    }
}
```

### 4.3 数据缓存策略

#### 4.3.1 缓存配置

| 数据类型 | 缓存时间 | 更新策略 |
|---------|---------|---------|
| 潮汐预报 | 24小时 | 定时更新 |
| 实时潮位 | 10分钟 | 定时更新 |
| 天气预报 | 3小时 | 定时更新 |
| 气象预警 | 5分钟 | 实时推送 |
| 台风路径 | 1小时 | 定时更新 |

#### 4.3.2 缓存实现

```python
import time
from functools import wraps

def cache_result(expire_seconds=3600):
    cache = {}
    
    def decorator(func):
        @wraps(func)
        def wrapper(*args, **kwargs):
            key = str(args) + str(kwargs)
            if key in cache:
                result, timestamp = cache[key]
                if time.time() - timestamp < expire_seconds:
                    return result
            result = func(*args, **kwargs)
            cache[key] = (result, time.time())
            return result
        return wrapper
    return decorator

class CachedTideAPI(TideAPI):
    @cache_result(expire_seconds=86400)
    def get_forecast(self, station_code, days=3):
        return super().get_forecast(station_code, days)
    
    @cache_result(expire_seconds=600)
    def get_realtime(self, station_code):
        return super().get_realtime(station_code)
```

## 五、数据质量控制

### 5.1 数据校验

| 校验项 | 校验规则 | 异常处理 |
|-------|---------|---------|
| 数据完整性 | 检查必填字段 | 返回错误信息 |
| 数值范围 | 检查数值合理性 | 标记异常数据 |
| 时间连续性 | 检查时间序列 | 插值或报警 |
| 空间一致性 | 检查位置合理性 | 标记异常位置 |

### 5.2 异常数据处理

| 异常类型 | 处理方式 |
|---------|---------|
| 数据缺失 | 使用历史数据插值 |
| 数值异常 | 使用相邻数据平滑 |
| 时间异常 | 调整时间戳 |
| 位置异常 | 使用最近有效位置 |

## 六、对接注意事项

### 6.1 申请流程注意事项

| 注意项 | 说明 |
|-------|-----|
| 资质要求 | 部分API需要企业资质 |
| 使用范围 | 明确数据使用范围限制 |
| 费用说明 | 了解收费标准 |
| 数据保密 | 遵守数据保密协议 |

### 6.2 技术对接注意事项

| 注意项 | 说明 |
|-------|-----|
| 调用频率 | 遵守API调用频率限制 |
| 错误处理 | 完善的错误处理机制 |
| 超时设置 | 合理设置请求超时 |
| 重试机制 | 实现请求重试机制 |

### 6.3 数据使用注意事项

| 注意项 | 说明 |
|-------|-----|
| 版权声明 | 数据来源版权声明 |
| 使用限制 | 不得转售或二次分发 |
| 安全存储 | 敏感数据加密存储 |
| 日志记录 | 记录数据使用日志 |

## 七、附录

### 7.1 常见错误码

| 错误码 | 说明 | 处理建议 |
|-------|-----|---------|
| 401 | 认证失败 | 检查API Key |
| 403 | 权限不足 | 检查接口权限 |
| 404 | 数据不存在 | 检查参数 |
| 429 | 请求过于频繁 | 降低请求频率 |
| 500 | 服务器错误 | 稍后重试 |

### 7.2 联系方式

| 机构 | 联系方式 |
|-----|---------|
| 国家海洋信息中心 | service@nmdis.org.cn |
| 中国气象局 | data@cma.gov.cn |
| 长江水利委员会 | service@cjw.gov.cn |
