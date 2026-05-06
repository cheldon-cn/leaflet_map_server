# S-52 符号库

> IHO S-52 Presentation Library Ed 4.0.(4) 符号参考文档
> 
> 最后更新: 2026-05-05 | 标准版本: Ed 4.0.(4) (2025年3月澄清)

## 目录

- [概述](#概述)
- [符号命名规则](#符号命名规则)
- [点符号 (Point Symbols)](#点符号-point-symbols)
  - [浮标类 (Buoys)](#浮标类-buoys)
  - [信标类 (Beacons)](#信标类-beacons)
  - [灯塔类 (Lights)](#灯塔类-lights)
  - [陆标类 (Landmarks)](#陆标类-landmarks)
  - [危险物标类 (Dangers)](#危险物标类-dangers)
  - [无线电/雷达类 (Radio/Radar)](#无线电雷达类-radioradar)
  - [港口设施类 (Ports)](#港口设施类-ports)
  - [锚地类 (Anchorage)](#锚地类-anchorage)
  - [军事/限制区域类 (Military)](#军事限制区域类-military)
  - [服务设施类 (Services)](#服务设施类-services)
  - [自动更新符号 (Automatic Update) - Ed 4.0 新增](#自动更新符号-automatic-update---ed-40-新增)
- [线符号 (Line Symbols)](#线符号-line-symbols)
  - [电缆类 (Cables)](#电缆类-cables)
  - [管道类 (Pipelines)](#管道类-pipelines)
  - [航线类 (Routes)](#航线类-routes)
  - [海岸线类 (Coastlines)](#海岸线类-coastlines)
  - [等深线类 (Depth Contours)](#等深线类-depth-contours)
  - [自动更新线符号 - Ed 4.0 新增](#自动更新线符号---ed-40-新增)
- [面符号 (Area Patterns)](#面符号-area-patterns)
  - [填充图案 (Fill Patterns)](#填充图案-fill-patterns)
  - [区域类型 (Area Types)](#区域类型-area-types)
  - [特殊区域符号](#特殊区域符号)
- [航海员导航对象 (Mariner's Navigational Objects) - Part II](#航海员导航对象-mariners-navigational-objects---part-ii)
  - [危险高亮 (Danger Highlight)](#危险高亮-danger-highlight)
  - [本船符号 (Own Ship Symbol)](#本船符号-own-ship-symbol)
  - [航线与航点 (Routes and Waypoints)](#航线与航点-routes-and-waypoints)
  - [时间标签 (Time Tags)](#时间标签-time-tags)
- [符号渲染指令](#符号渲染指令)
- [线型样式](#线型样式)
- [颜色标记](#颜色标记)
- [S-52 文档结构](#s-52-文档结构)

---

## 概述

S-52 符号库是 IHO S-52 电子海图显示与信息系统海图内容与显示规范的核心组成部分，定义了电子海图中所有物标的符号化表示方法。

| 属性 | 值 |
|------|-----|
| 标准 | IHO S-52 Presentation Library Ed 4.0.(4) |
| 发布日期 | 2014年10月 (澄清至2025年3月) |
| 点符号数量 | 200+ |
| 线符号数量 | 50+ |
| 面符号数量 | 30+ |

### S-52 文档组成

S-52 Presentation Library 由以下部分组成：

| 部分 | 内容 |
|------|------|
| **Part I** | 图表对象 (Chart Objects) |
| **Part I Appendix A** | 颜色表 (Color Tables) |
| **Part I Appendices B-F** | 符号化查询表 (Symbolization Look-up Tables) |
| **Part I Addendum** | 符号目录纸质描述 (Symbol Catalogue) |
| **Part II** | 航海员导航对象 (Mariner's Navigational Objects) |

---

## 符号命名规则

S-52 符号采用统一的命名规范：`[前缀][类型][序号]`

### 命名示例

| 符号名称 | 前缀 | 类型 | 序号 | 说明 |
|----------|------|------|------|------|
| BOYCAR01 | BOY (浮标) | CAR (方位标) | 01 | 方位标浮标-北 |
| BCNLAT02 | BCN (信标) | LAT (侧面标) | 02 | 侧面标信标-右侧标 |
| LIGHTS05 | LIGHTS (灯塔) | - | 05 | 灯塔-白色 |
| WRECKS01 | WRECKS (沉船) | - | 01 | 沉船-危险 |
| CHRVDEL1 | CHR (变更) | VDEL (删除) | 1 | 自动更新-删除 |

### 常用前缀

| 前缀 | 含义 | 英文 |
|------|------|------|
| BOY | 浮标 | Buoy |
| BCN | 信标 | Beacon |
| LIT | 灯光 | Light |
| LND | 陆标 | Landmark |
| WRE | 沉船 | Wreck |
| OBS | 障碍物 | Obstruction |
| RAD | 雷达 | Radar |
| RDO | 无线电 | Radio |
| PIL | 引航 | Pilot |
| ACH | 锚地 | Anchorage |
| RES | 限制 | Restricted |
| MIP | 军事 | Military |
| CHR | 变更 | Change |
| VDEL | 删除 | Deleted |
| VID | 插入/修改 | Inserted/Modified |

---

## 点符号 (Point Symbols)

### 浮标类 (Buoys)

浮标是海上导航的重要标志，用于标示航道、危险区域等。

#### 方位标浮标 (Cardinal Buoys)

方位标根据罗盘方位指示危险区域的位置。

| 符号名称 | 描述 | 方位 | 用途 |
|----------|------|------|------|
| BOYCAR01 | 方位标浮标-北 | N | 危险区域在浮标南侧 |
| BOYCAR02 | 方位标浮标-东 | E | 危险区域在浮标西侧 |
| BOYCAR03 | 方位标浮标-南 | S | 危险区域在浮标北侧 |
| BOYCAR04 | 方位标浮标-西 | W | 危险区域在浮标东侧 |
| BOYCAR11 | 方位标浮标-北(简化) | N | 简化显示模式 |
| BOYCAR12 | 方位标浮标-东(简化) | E | 简化显示模式 |
| BOYCAR13 | 方位标浮标-南(简化) | S | 简化显示模式 |
| BOYCAR14 | 方位标浮标-西(简化) | W | 简化显示模式 |

#### 侧面标浮标 (Lateral Buoys)

侧面标用于标示航道两侧边界。

| 符号名称 | 描述 | 颜色 | 位置 |
|----------|------|------|------|
| BOYLAT01 | 侧面标浮标-左侧标 | 红色 | 航道左侧(IALA A) |
| BOYLAT02 | 侧面标浮标-右侧标 | 绿色 | 航道右侧(IALA A) |
| BOYLAT03 | 侧面标浮标-推荐航道左侧 | 红/绿 | 推荐航道左侧 |
| BOYLAT04 | 侧面标浮标-推荐航道右侧 | 绿/红 | 推荐航道右侧 |
| BOYLAT11 | 侧面标浮标-左侧标(简化) | 红色 | 简化显示模式 |
| BOYLAT12 | 侧面标浮标-右侧标(简化) | 绿色 | 简化显示模式 |

#### 其他浮标类型

| 符号名称 | 描述 | 用途 |
|----------|------|------|
| BOYINB01 | 安装浮标 | 标示安装工程区域 |
| BOYINB11 | 安装浮标(简化) | 简化显示模式 |
| BOYISD01 | 孤立危险标浮标 | 标示孤立危险物 |
| BOYISD11 | 孤立危险标浮标(简化) | 简化显示模式 |
| BOYSAW01 | 安全水域标浮标 | 标示安全水域 |
| BOYSAW11 | 安全水域标浮标(简化) | 简化显示模式 |
| BOYSPP01 | 特殊用途浮标 | 特殊用途标识 |
| BOYSPP11 | 特殊用途浮标(简化) | 简化显示模式 |

---

### 信标类 (Beacons)

信标是固定在海底或岸上的导航标志。

#### 方位标信标 (Cardinal Beacons)

| 符号名称 | 描述 | 方位 |
|----------|------|------|
| BCNCAR01 | 方位标信标-北 | N |
| BCNCAR02 | 方位标信标-东 | E |
| BCNCAR03 | 方位标信标-南 | S |
| BCNCAR04 | 方位标信标-西 | W |
| BCNCAR11 | 方位标信标-北(简化) | N |
| BCNCAR12 | 方位标信标-东(简化) | E |
| BCNCAR13 | 方位标信标-南(简化) | S |
| BCNCAR14 | 方位标信标-西(简化) | W |

#### 侧面标信标 (Lateral Beacons)

| 符号名称 | 描述 | 颜色 |
|----------|------|------|
| BCNLAT01 | 侧面标信标-左侧标 | 红色 |
| BCNLAT02 | 侧面标信标-右侧标 | 绿色 |
| BCNLAT03 | 侧面标信标-推荐航道左侧 | 红/绿 |
| BCNLAT04 | 侧面标信标-推荐航道右侧 | 绿/红 |
| BCNLAT11 | 侧面标信标-左侧标(简化) | 红色 |
| BCNLAT12 | 侧面标信标-右侧标(简化) | 绿色 |

#### 其他信标类型

| 符号名称 | 描述 |
|----------|------|
| BCNISD01 | 孤立危险标信标 |
| BCNISD11 | 孤立危险标信标(简化) |
| BCNSAW01 | 安全水域标信标 |
| BCNSAW11 | 安全水域标信标(简化) |
| BCNSPP01 | 特殊用途信标 |
| BCNSPP11 | 特殊用途信标(简化) |

---

### 灯塔类 (Lights)

灯塔是重要的导航设施，用于标示航道入口、危险区域等。

#### 灯塔符号

| 符号名称 | 描述 | 光色 | 备注 |
|----------|------|------|------|
| LIGHTS01 | 灯塔-一般 | 默认 | 默认灯光符号 |
| LIGHTS02 | 灯塔-红色 | 红 | 红色灯光 |
| LIGHTS03 | 灯塔-绿色 | 绿 | 绿色灯光 |
| LIGHTS04 | 灯塔-黄色 | 黄 | 黄色灯光 |
| LIGHTS05 | 灯塔-白色 | 白 | 白色灯光 |
| LIGHTS06 | 灯塔-蓝色 | 蓝 | 蓝色灯光 (Ed 4.0 新增) |
| LIGHTS07 | 灯塔-橙色 | 橙 | 橙色灯光 |
| LIGHTS08 | 灯塔-紫色 | 紫 | 紫色灯光 |
| LIGHTS09 | 灯塔-扇形 | 扇形光弧 | 扇形灯光 |
| LIGHTS11 | 灯塔(简化) | 默认 | 简化显示模式 |

> **注**: LIGHTS06 蓝色灯光符号为 S-52 Ed 4.0 新增，需要颜色标记 LITBL 支持。

#### 浮动灯标

| 符号名称 | 描述 |
|----------|------|
| LITFLT01 | 浮动灯标 |
| LITFLT11 | 浮动灯标(简化) |

#### 灯船

| 符号名称 | 描述 |
|----------|------|
| LITVES01 | 灯船 |
| LITVES11 | 灯船(简化) |

---

### 陆标类 (Landmarks)

陆标是岸上显著的固定物体，用于导航定位。

| 符号名称 | 描述 | 特征 |
|----------|------|------|
| LNDMRK01 | 陆标-教堂 | 教堂尖塔 |
| LNDMRK02 | 陆标-塔 | 塔状建筑 |
| LNDMRK03 | 陆标-烟囱 | 烟囱 |
| LNDMRK04 | 陆标-纪念碑 | 纪念碑 |
| LNDMRK05 | 陆标-旗杆 | 旗杆 |
| LNDMRK06 | 陆标-雕像 | 雕像 |
| LNDMRK07 | 陆标-穹顶 | 穹顶建筑 |
| LNDMRK08 | 陆标-天线 | 天线/信号塔 |
| LNDMRK09 | 陆标-风车 | 风车 |
| LNDMRK10 | 陆标-灯塔 | 灯塔建筑 |
| LNDMRK11 | 陆标-城堡 | 城堡 |
| LNDMRK12 | 陆标-桥梁 | 桥梁 |
| LNDMRK13 | 陆标-其他 | 其他类型 |

#### 其他建筑物

| 符号名称 | 描述 |
|----------|------|
| BUISGL01 | 大型建筑物 |
| TOWERS01 | 塔 |
| MONUMT01 | 纪念碑 |
| WIMCON01 | 风力发电设施 |

---

### 危险物标类 (Dangers)

危险物标用于标示对航行安全构成威胁的物体。

#### 沉船 (Wrecks)

| 符号名称 | 描述 | 状态 |
|----------|------|------|
| WRECKS01 | 沉船-危险 | 危险沉船 |
| WRECKS02 | 沉船-非危险 | 非危险沉船 |
| WRECKS03 | 沉船-残骸 | 残骸 |
| WRECKS04 | 沉船-露出水面 | 露出水面 |
| WRECKS05 | 沉船-淹没 | 淹没 |
| WRECKS11 | 沉船(简化) | 简化显示 |

#### 障碍物 (Obstructions)

| 符号名称 | 描述 | 类型 |
|----------|------|------|
| OBSTRN01 | 障碍物-一般 | 一般障碍 |
| OBSTRN02 | 障碍物-礁石 | 礁石 |
| OBSTRN03 | 障碍物-浅滩 | 浅滩 |
| OBSTRN04 | 障碍物-暗礁 | 暗礁 |
| OBSTRN05 | 障碍物-沉船 | 沉船障碍 |
| OBSTRN06 | 障碍物-其他 | 其他障碍 |
| OBSTRN11 | 障碍物(简化) | 简化显示 |

#### 水下岩石

| 符号名称 | 描述 |
|----------|------|
| UWTROC01 | 水下岩石 |
| UWTROC11 | 水下岩石(简化) |

#### 隔离危险区 (Isolated Danger)

| 符号名称 | 描述 | 符号特征 |
|----------|------|----------|
| ISOHAZ01 | 隔离危险区 | 洋红色八边形符号 |

> **注**: 隔离危险符号用于标示障碍物、岩石或沉船，其深度值小于航海员定义的安全等深线，且位于安全等深线定义的安全水域内。该符号可用于点、线、面特征。

---

### 无线电/雷达类 (Radio/Radar)

| 符号名称 | 描述 | 用途 |
|----------|------|------|
| RADSTA01 | 雷达站 | 雷达信号发射站 |
| RDOSTA01 | 无线电电台 | 无线电通信站 |
| RTPBCN01 | 雷达应答器 | 雷达信号应答 |
| FOGSIG01 | 雾号 | 雾天信号 |
| RSCSTA01 | 搜救站 | 搜救协调站 |

---

### 港口设施类 (Ports)

| 符号名称 | 描述 | 用途 |
|----------|------|------|
| PILBOP01 | 引航站 | 引航员登船点 |
| PONTON01 | 浮码头 | 浮动码头 |
| HULKES01 | 废船/趸船 | 废弃船只/趸船 |
| CRANES01 | 起重机 | 港口起重机 |
| BERTHS01 | 泊位 | 船舶停泊位 |

---

### 锚地类 (Anchorage)

| 符号名称 | 描述 | 用途 |
|----------|------|------|
| ACHARE01 | 锚地 | 一般锚地 |
| ACHBRT01 | 深水锚地 | 深水锚泊区 |

---

### 军事/限制区域类 (Military)

| 符号名称 | 描述 | 类型 |
|----------|------|------|
| MIPARE01 | 军事区域 | 军事禁区 |
| RESARE01 | 限制区域 | 限制进入区 |
| CSTARE01 | 海关区域 | 海关监管区 |

---

### 服务设施类 (Services)

#### 海底区域

| 符号名称 | 描述 | 底质 |
|----------|------|------|
| SBDARE01 | 海底区域-沙 | 沙底 |
| SBDARE02 | 海底区域-泥 | 泥底 |
| SBDARE03 | 海底区域-石 | 石底 |
| SBDARE04 | 海底区域-珊瑚 | 珊瑚底 |
| SBDARE05 | 海底区域-岩石 | 岩石底 |

#### 其他服务设施

| 符号名称 | 描述 |
|----------|------|
| ICEARE01 | 冰区 |
| LOGPON01 | 木材转运点 |

---

### 自动更新符号 (Automatic Update) - Ed 4.0 新增

S-52 Presentation Library Ed 4.0 新增了四种符号，用于标示自动更新过程中插入、修改或删除的对象。

| 符号名称 | 几何类型 | 描述 | 参考编号 |
|----------|----------|------|----------|
| CHRVDEL1 | 点 | 对象已被删除 | 592 |
| CHRVID01 | 点 | 对象已被插入或修改 | 593 |
| CHRVDEL2 | 线/面 | 对象已被删除 | 595 |
| CHRVID02 | 线/面 | 对象已被插入或修改 | 596 |

> **注**: 这些符号用于 ENC 自动更新后，航海员可以选择查看更新的位置。正常显示时一般将自动更新显示关闭。

---

## 线符号 (Line Symbols)

### 电缆类 (Cables)

| 符号名称 | 描述 | 位置 |
|----------|------|------|
| CBLSUB01 | 海底电缆 | 海底 |
| CBLOHD01 | 架空电缆 | 架空 |

---

### 管道类 (Pipelines)

| 符号名称 | 描述 | 输送物 |
|----------|------|--------|
| PIPSOL01 | 海底管道-油 | 石油 |
| PIPSOL02 | 海底管道-气 | 天然气 |
| PIPSOL03 | 海底管道-水 | 水 |
| PIPSOL04 | 海底管道-化学品 | 化学品 |
| PIPSOL05 | 海底管道-液化气 | 液化气 |
| PIPSOL06 | 海底管道-其他 | 其他物质 |
| PIPSOL07 | 海底管道-未知 | 未知物质 |
| PIPOHD01 | 架空管道 | 架空 |

---

### 航线类 (Routes)

| 符号名称 | 描述 | 用途 |
|----------|------|------|
| FERYRT01 | 轮渡航线 | 轮渡航行路线 |
| RCRTCL01 | 推荐航线 | 推荐航行路线 |
| TSELNE01 | 交通分隔线 | 船舶交通分隔 |

---

### 海岸线类 (Coastlines)

| 符号名称 | 描述 | 类型 |
|----------|------|------|
| COALNE01 | 海岸线 | 一般海岸线 |
| SLCONS01 | 海岸线构筑物 | 人工海岸 |
| SHORLN01 | 岸线 | 一般岸线 |

---

### 等深线类 (Depth Contours)

| 符号名称 | 描述 | 深度范围 |
|----------|------|----------|
| DEPCNT01 | 等深线 | 水深等值线 |
| DEPCNT02 | 安全等深线 | 安全水深线 |

---

### 自动更新线符号 - Ed 4.0 新增

| 符号名称 | 几何类型 | 描述 |
|----------|----------|------|
| CHRVDEL2 | 线/面 | 线或面对象已被删除 |
| CHRVID02 | 线/面 | 线或面对象已被插入或修改 |

---

## 面符号 (Area Patterns)

### 填充图案 (Fill Patterns)

| 符号名称 | 描述 | 用途 |
|----------|------|------|
| DIAMOND01 | 菱形填充 | 限制区域填充 |
| HORIZONTAL01 | 水平线填充 | 特殊区域填充 |
| VERTICAL01 | 垂直线填充 | 特殊区域填充 |
| DIAGONAL01 | 对角线填充 | 危险区域填充 |
| CROSS01 | 交叉线填充 | 禁止区域填充 |
| DOTS01 | 点填充 | 锚地填充 |

---

### 区域类型 (Area Types)

| 符号名称 | 描述 | 颜色标记 |
|----------|------|----------|
| LNDARE01 | 陆地区域 | LANDF |
| DEPARE01 | 水深区域 | DEPVS/DEPMS/DEPDW |
| RESARE01 | 限制区域 | RESARE |
| ACHARE01 | 锚地区域 | ACHARE |
| FAIRWY01 | 航道区域 | FAIRWY |

---

### 特殊区域符号

#### 未测量区域 (Unsurveyed Area)

| 符号名称 | 描述 | 显示特征 |
|----------|------|----------|
| UNSARE01 | 未测量区域 | 灰色填充 + 虚线图案 |

> **注**: ENC 生产者有时需要在海图中包含没有调查数据的区域，这些区域只有基础测量数据，不足以支持安全航行。ECDIS 使用灰色填充和虚线图案显示这些未测量区域，提醒航海员进入这些水域不安全。

#### 疏浚区域 (Dredged Area)

| 符号名称 | 描述 | 显示特征 |
|----------|------|----------|
| DRGARE01 | 疏浚区域 | 灰色点填充模式 |

> **注**: 疏浚区域以灰色点填充模式显示。疏浚/维护水深值可能不总是直接显示，可使用 ECDIS 的 pick report 功能查看 DRVAL1 值。

---

## 航海员导航对象 (Mariner's Navigational Objects) - Part II

S-52 Part II 定义了航海员导航对象，这些是 ECDIS 系统特有的功能符号，不属于 ENC 数据。

### 危险高亮 (Danger Highlight)

航海员可以手动标记危险区域，用于标示对船舶构成危险的区域。

| 符号名称 | 描述 | 显示特征 |
|----------|------|----------|
| DNGHLK01 | 危险高亮-点 | 洋红色标记 |
| DNGHLK02 | 危险高亮-线 | 红色粗实线边界 |
| DNGHLK03 | 危险高亮-面 | 红色透明填充 + 红色边界 |

> **注**: 航海员标记的危险区域应使用红色粗实线边界，面区域使用透明红色填充。

---

### 本船符号 (Own Ship Symbol)

| 符号名称 | 描述 | 用途 |
|----------|------|------|
| OWNSHP01 | 本船符号-一般 | 标示本船位置和航向 |
| OWNSHP02 | 本船符号-简化 | 简化显示模式 |

#### 本船符号参数

| 参数 | 描述 |
|------|------|
| 船首向 | 指向船舶航行方向 |
| 对地航向 | 显示对地运动方向 |
| 对水航向 | 显示对水运动方向 |
| 安全轮廓 | 本船安全轮廓范围 |

---

### 航线与航点 (Routes and Waypoints)

| 符号名称 | 描述 | 用途 |
|----------|------|------|
| ROUTE01 | 计划航线 | 显示计划航线 |
| WPT001 | 航点 | 标示转向点 |
| XTELN01 | 偏航限制线 | 显示允许偏航范围 |

---

### 时间标签 (Time Tags)

| 符号名称 | 描述 | 用途 |
|----------|------|------|
| TIMTAG01 | 时间标签 | 显示预计到达时间 |

---

## 符号渲染指令

S-52 定义了一套完整的符号渲染指令，用于控制物标的显示方式。

### 点符号指令

| 指令 | 语法 | 说明 | 示例 |
|------|------|------|------|
| SY | `SY(symbol_name)` | 点符号 | `SY(BOYCAR01)` |

### 线符号指令

| 指令 | 语法 | 说明 | 示例 |
|------|------|------|------|
| LS | `LS(style, width, color)` | 线型渲染 | `LS(SOLD,2,CHBLK)` |
| LC | `LC(line_symbol)` | 线符号 | `LC(PIPSOL01)` |

### 面符号指令

| 指令 | 语法 | 说明 | 示例 |
|------|------|------|------|
| AC | `AC(color_token)` | 面填充色 | `AC(LANDF)` |
| AP | `AP(pattern_name)` | 面填充图案 | `AP(DIAMOND01)` |

### 文本指令

| 指令 | 语法 | 说明 | 示例 |
|------|------|------|------|
| TE | `TE('format', field, ...)` | 文本标签 | `TE('bn%s','OBJNAM',2,1,2,'15110',-1,-1,CHBLK,21)` |
| TX | `TX('format', field, ...)` | 文本显示 | `TX('%s','OBJNAM',2,1,2,'15110',-1,-1,CHBLK,21)` |

### 条件符号化

| 指令 | 语法 | 说明 |
|------|------|------|
| CS | `CS(procedure)` | 条件符号化过程（内部命令） |

---

## 线型样式

| 样式名称 | 英文 | 描述 |
|----------|------|------|
| SOLD | Solid | 实线 |
| DASH | Dashed | 虚线 |
| DOT | Dotted | 点线 |
| DASHDOT | Dash-Dot | 点划线 |

---

## 颜色标记

### 导航颜色

| 标记 | 颜色 | 用途 |
|------|------|------|
| CHBLK | 黑色 | 文本和符号边框 |
| CHWHT | 白色 | 背景和高亮 |
| CHRED | 红色 | 危险/警示 |
| CHGRN | 绿色 | 安全/航道 |
| CHYEL | 黄色 | 注意 |
| CHBLU | 蓝色 | 水域 |
| CHORN | 橙色 | 警告 |
| CHPNA | 紫色 | 电缆/管道 |
| CHBRN | 棕色 | 地形 |
| LITBL | 蓝色 | 蓝色灯光 (Ed 4.0 新增) |

### 水深颜色

| 标记 | 描述 | 深度范围 |
|------|------|----------|
| DEPVS | 非常浅 | 0-2m |
| DEPMS | 中浅 | 2-5m |
| DEPDW | 深 | 5-10m |
| DEPMD | 中深 | 10-20m |
| DEPDS | 很深 | >20m |

### 区域颜色

| 标记 | 描述 |
|------|------|
| LANDF | 陆地填充 |
| RESARE | 限制区域 |
| ACHARE | 锚地 |
| FAIRWY | 航道 |
| SBDARE | 海底区域 |
| DNGHLK | 危险高亮 |

### 特殊标记

| 标记 | 描述 | 用途 |
|------|------|------|
| DMPINC | 数据质量指示 | 标示数据质量不确定 |
| QUAPOS | 质量位置 | 位置质量标记 |

---

## S-52 文档结构

```
S-52 Presentation Library
├── Part I: Chart Objects (图表对象)
│   ├── Appendix A: Color Tables (颜色表)
│   │   ├── Day Bright
│   │   ├── Dusk
│   │   └── Night
│   ├── Appendix B: Point Lookup Table (点查询表)
│   ├── Appendix C: Line Lookup Table (线查询表)
│   ├── Appendix D: Area Lookup Table (面查询表)
│   ├── Appendix E: Isolated Danger Symbol (隔离危险符号)
│   ├── Appendix F: Conditional Symbology (条件符号化)
│   └── Addendum: Symbol Catalogue (符号目录)
└── Part II: Mariner's Navigational Objects (航海员导航对象)
    ├── Own Ship Symbol (本船符号)
    ├── Routes and Waypoints (航线与航点)
    ├── Danger Highlight (危险高亮)
    └── Time Tags (时间标签)
```

---

## 参考资料

- [IHO S-52 Presentation Library](https://iho.int/en/s-57-enc-portrayal)
- [IHO S-57 Object Catalogue](https://iho.int/en/s-57-object-catalogue)
- [S-52 Annex A: ECDIS Presentation Library Ed 4.0.(4)](https://iho.int/uploads/user/pubs/standards/s-52/)
- [IHO Standards in Force](https://iho.int/standards-in-force)

---

*文档生成日期: 2026-05-05*
*标准版本: IHO S-52 Presentation Library Ed 4.0.(4) (澄清至2025年3月)*
