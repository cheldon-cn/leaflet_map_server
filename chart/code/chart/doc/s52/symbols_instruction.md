# S-52 符号化指令集 (Symbolization Instruction Set)

> IHO S-52 Presentation Library Edition 4.0 — 符号化指令分类、语法、示例与解析逻辑

---

## 目录

- [概述](#概述)
- [指令体系架构](#指令体系架构)
- [一、面向几何的指令](#一面向几何的指令)
  - [1.1 SY — 点符号绘制](#11-sy--点符号绘制)
  - [1.2 LS — 线型渲染](#12-ls--线型渲染)
  - [1.3 LC — 线符号绘制](#13-lc--线符号绘制)
  - [1.4 AC — 面填充色](#14-ac--面填充色)
  - [1.5 AP — 面填充图案](#15-ap--面填充图案)
  - [1.6 TX — 文本显示](#16-tx--文本显示)
  - [1.7 TE — 文本标签](#17-te--文本标签)
- [二、逻辑控制指令](#二逻辑控制指令)
  - [2.1 CS — 条件符号化调用](#21-cs--条件符号化调用)
- [三、条件符号化程序 (CSP)](#三条件符号化程序-csp)
  - [3.1 CSP 概述](#31-csp-概述)
  - [3.2 核心 CSP 程序清单](#32-核心-csp-程序清单)
  - [3.3 CSP 伪代码实现](#33-csp-伪代码实现)
- [四、指令组合与执行顺序](#四指令组合与执行顺序)
- [五、指令解析引擎参考实现](#五指令解析引擎参考实现)
- [附录](#附录)

---

## 概述

S-52 符号化指令集是连接 **查询表 (Lookup Table)** 与 **基础资源库 (Symbol Library / Color Table)** 的"机器语言"。查询表最终产出由这些指令构成的命令序列，ECDIS 渲染引擎按序执行这些指令完成海图绘制。

```
┌─────────────┐     查询匹配     ┌──────────────────┐     指令执行     ┌──────────────┐
│  S-57 物标   │ ──────────────▶ │  符号化指令序列    │ ──────────────▶ │  屏幕渲染输出  │
│  (Object)    │                 │  (Instruction Seq) │                 │  (Display)    │
└─────────────┘                  └──────────────────┘                  └──────────────┘
                                        │
                                        │ 引用
                                        ▼
                                 ┌──────────────┐
                                 │  基础资源库    │
                                 │  - 符号库      │
                                 │  - 颜色表      │
                                 │  - 线型库      │
                                 └──────────────┘
```

### 指令分类总览

| 类别 | 指令 | 功能 | 适用几何 |
|------|------|------|----------|
| 几何绘制 | SY | 绘制点符号 | Point |
| 几何绘制 | LS | 线型渲染 | Line / Area边界 |
| 几何绘制 | LC | 线符号绘制 | Line |
| 几何绘制 | AC | 面填充色 | Area |
| 几何绘制 | AP | 面填充图案 | Area |
| 几何绘制 | TX | 文本显示 | Point / Line / Area |
| 几何绘制 | TE | 文本标签 | Point / Line / Area |
| 逻辑控制 | CS | 调用条件符号化程序 | All |

---

## 指令体系架构

```
S-52 符号化指令
├── 面向几何的指令 (Geometry-Oriented)
│   ├── SY(symbol_name[, rotation])               — 点符号
│   ├── LS(style, width, color)                   — 线型渲染
│   ├── LC(line_symbol_name)                      — 线符号
│   ├── AC(color_token)                           — 面填充色
│   ├── AP(pattern_name[, minDist])               — 面填充图案
│   ├── TX('format', field, ha, va, sp, vg, ...)  — 文本显示
│   └── TE('format', field, ha, va, sp, vg, ...)  — 文本标签
│
└── 逻辑控制指令 (Logic Control)
    └── CS(procedure_name)                        — 条件符号化程序
        ├── LIGHTS05                              — 灯光符号化
        ├── DEPARE01                              — 水深区域
        ├── RESARE01                              — 限制区域
        ├── RESTRN01                              — 限制
        ├── SEABED01                              — 海底区域
        ├── SLCONS01                              — 海岸构筑物
        ├── OBSTRN01                              — 障碍物
        ├── WRECKS01                              — 沉船
        ├── TOPMAR01                              — 顶标
        ├── BUISGL01                              — 建筑物
        ├── ISODGR01                              — 隔离危险
        ├── QUAPOS01                              — 位置质量
        └── ... (共 50+ CSP)
```

---

## 一、面向几何的指令

### 1.1 SY — 点符号绘制

**语法**: `SY(symbol_name[, rotation])`

**功能**: 在物标坐标位置绘制指定的点符号。符号定义来自符号库 (Symbol Library)。可选旋转参数用于方向性符号（如风向标）。

**参数**:

| 参数 | 类型 | 必选 | 说明 | 示例值 |
|------|------|------|------|--------|
| symbol_name | string | 是 | 符号库中的符号名称 | BOYCAR01, LIGHTS01 |
| rotation | double | 否 | 旋转角度(度)，顺时针 | 45.0, 90.0 |

**完整示例**:

```
SY(BOYCAR01)          — 绘制方位标浮标符号
SY(BOYISD01)          — 绘制孤立危险标浮标符号
SY(LIGHTS01)          — 绘制灯塔符号
SY(OWNSHP01)          — 绘制本船符号
SY(DNGHLK01)          — 绘制危险高亮符号 (Ed4.0新增)
SY(CHRVDEL1)          — 绘制自动更新删除标记 (Ed4.0新增)
SY(FOGSIG01)          — 绘制雾号符号
SY(RTPBCN01)          — 绘制雷达应答器符号
```

**查询表中的使用**:

```xml
<Entry objectClass="BOYCAR" priority="4" radar="O" displayCategory="BASE" viewingGroup="22210">
  <Condition field="CATCAM" value="1">SY(BOYCAR01)</Condition>
  <Condition field="CATCAM" value="2">SY(BOYCAR02)</Condition>
  <Condition field="CATCAM" value="3">SY(BOYCAR03)</Condition>
  <Condition field="CATCAM" value="4">SY(BOYCAR04)</Condition>
  <Default>SY(BOYCAR01)</Default>
</Entry>
```

**解析逻辑 (C++伪代码)**:

```cpp
void executeSY(const std::string& symbolName,
               const Point2D& position,
               double rotation = 0.0,
               double scale = 1.0)
{
    const Symbol* symbol = SymbolLibrary::lookup(symbolName);
    if (!symbol) {
        LOG_WARN("Symbol not found: " + symbolName);
        return;
    }
    for (const auto& vec : symbol->vectors()) {
        drawVector(position, vec, rotation, scale,
                   ColorTable::current());
    }
}
```

---

### 1.2 LS — 线型渲染

**语法**: `LS(style, width, color)`

**功能**: 使用指定的线型样式、宽度和颜色绘制线段。适用于简单线型和面物标边界线。

**参数**:

| 参数 | 类型 | 说明 | 可选值 |
|------|------|------|--------|
| style | enum | 线型样式 | SOLD, DASH, DOT, DASHDOT |
| width | int | 线宽 (像素) | 1-6 |
| color | string | 颜色标记 | CHBLK, CHRED, CHGRN, CHBLU, ... |

**线型样式定义**:

| 样式 | 英文 | 描述 | 图形表示 | dash/gap模式 |
|------|------|------|----------|-------------|
| SOLD | Solid | 实线 | ━━━━━━━━━━ | 连续 |
| DASH | Dashed | 虚线 | ━ ━ ━ ━ ━ | dash=5, gap=3 |
| DOT | Dotted | 点线 | · · · · · · | dash=1, gap=2 |
| DASHDOT | Dash-Dot | 点划线 | ━·━·━·━· | dash=5,gap=2,dot=1,gap=2 |

**完整示例**:

```
LS(SOLD,2,CHBLK)      — 黑色2像素实线 (海岸线)
LS(DASH,2,CHRED)      — 红色2像素虚线 (限制区域边界)
LS(DASH,1,CHGRN)      — 绿色1像素虚线 (锚地边界)
LS(SOLD,1,DEPCNT)     — 等深线颜色1像素实线 (水深区域边界)
LS(SOLD,3,CHBLK)      — 黑色3像素实线 (桥梁)
LS(DASH,2,CHBLU)      — 蓝色2像素虚线 (航道)
LS(DASH,1,CHYEL)      — 黄色1像素虚线 (海关区域)
LS(DASHDOT,1,CHBLK)   — 黑色1像素点划线 (特殊边界)
```

**查询表中的使用**:

```xml
<Entry objectClass="COALNE" priority="3" radar="O" displayCategory="BASE" viewingGroup="24010">
  <Default>LS(SOLD,2,CHBLK)</Default>
  <Description>海岸线</Description>
</Entry>

<Entry objectClass="RESARE" priority="4" radar="O" displayCategory="STANDARD" viewingGroup="24060">
  <Default>LS(DASH,2,CHRED)</Default>
  <Description>限制区域边界</Description>
</Entry>
```

**解析逻辑 (C++伪代码)**:

```cpp
struct LinePattern {
    std::vector<double> dashArray;
    static LinePattern fromStyle(const std::string& style) {
        static const std::unordered_map<std::string, LinePattern> patterns = {
            {"SOLD",    {{}}},
            {"DASH",    {{5.0, 3.0}}},
            {"DOT",     {{1.0, 2.0}}},
            {"DASHDOT", {{5.0, 2.0, 1.0, 2.0}}},
        };
        auto it = patterns.find(style);
        return it != patterns.end() ? it->second : patterns.at("SOLD");
    }
};

void executeLS(const std::string& style, int width,
               const std::string& color, const Geometry& geometry)
{
    RGB colorRgb = ColorTable::current().resolve(color);
    LinePattern pattern = LinePattern::fromStyle(style);
    drawStyledLine(geometry, pattern, width, colorRgb);
}
```

---

### 1.3 LC — 线符号绘制

**语法**: `LC(line_symbol_name)`

**功能**: 使用预定义的复合线符号绘制线段。与 LS 的区别在于 LC 引用的是符号库中完整的线符号定义，可包含重复图案、多色段等复杂线型。

**参数**:

| 参数 | 类型 | 说明 | 示例值 |
|------|------|------|--------|
| line_symbol_name | string | 线符号库中的符号名称 | CBLSUB01, PIPSOL01 |

**LS 与 LC 的区别**:

| 特性 | LS | LC |
|------|-----|-----|
| 定义方式 | 内联参数 | 符号库引用 |
| 复杂度 | 简单线型 | 复合线型(含重复图案) |
| 灵活性 | 有限 | 高 |
| 典型用途 | 边界线、等深线 | 电缆、管道、航道 |
| 渲染方式 | 单色单线 | 多元素沿路径重复 |

**完整示例**:

```
LC(CBLSUB01)          — 海底电缆线符号
LC(CBLOHD01)          — 架空电缆线符号
LC(PIPSOL01)          — 海底管道线符号(石油)
LC(PIPSOL02)          — 海底管道线符号(水)
LC(PIPSOL06)          — 海底管道线符号(其他)
LC(PIPOHD01)          — 架空管道线符号
LC(FAIRWY01)          — 航道线符号
LC(NAVLNE01)          — 航行线符号
LC(CTNARE01)          — 注意区域线符号
```

**查询表中的使用**:

```xml
<Entry objectClass="CBLSUB" priority="4" radar="O" displayCategory="STANDARD" viewingGroup="24040">
  <Default>LC(CBLSUB01)</Default>
  <Description>海底电缆</Description>
</Entry>

<Entry objectClass="PIPSOL" priority="4" radar="O" displayCategory="STANDARD" viewingGroup="24040">
  <Condition field="PRODCT" value="1">LC(PIPSOL01)</Condition>
  <Condition field="PRODCT" value="2">LC(PIPSOL02)</Condition>
  <Condition field="PRODCT" value="6">LC(PIPSOL06)</Condition>
  <Default>LC(PIPSOL01)</Default>
</Entry>
```

**解析逻辑 (C++伪代码)**:

```cpp
void executeLC(const std::string& lineSymbolName,
               const Geometry& geometry)
{
    const LineSymbol* lineSymbol = LineSymbolLibrary::lookup(lineSymbolName);
    if (!lineSymbol) {
        LOG_WARN("Line symbol not found: " + lineSymbolName);
        return;
    }
    for (const auto& segment : geometry.segments()) {
        double offset = 0.0;
        while (offset < segment.length()) {
            for (const auto& element : lineSymbol->elements()) {
                drawElementAtOffset(element, geometry, offset,
                                    ColorTable::current());
                offset += element.repeatingDistance;
            }
        }
    }
}
```

---

### 1.4 AC — 面填充色

**语法**: `AC(color_token)`

**功能**: 使用指定颜色标记填充面物标区域。颜色值来自当前激活的颜色表 (Day Bright / Dusk / Night)。

**参数**:

| 参数 | 类型 | 说明 | 示例值 |
|------|------|------|--------|
| color_token | string | 颜色表中的颜色标记 | LANDF, DEPVS, CHGRD |

**常用颜色标记**:

| 颜色标记 | Day Bright RGB | 用途 |
|----------|---------------|------|
| LANDF | #C6A664 | 陆地填充 |
| DEPVS | #99CCFF | 深水区-非常浅 |
| DEPMS | #6699CC | 深水区-中浅 |
| DEPDW | #336699 | 深水区-深 |
| DEPMD | #003366 | 深水区-中深 |
| DEPDS | #003366 | 深水区-很深 |
| CHGRD | #B0B0B0 | 灰色(建筑物) |
| DNGHLK | #FF00FF | 危险高亮 (Ed4.0) |
| NODTA | #808080 | 无数据区域 |

**完整示例**:

```
AC(LANDF)             — 陆地填充色
AC(DEPVS)             — 浅水区填充色
AC(DEPDW)             — 深水区填充色
AC(CHGRD)             — 灰色填充(建筑物区域)
AC(DNGHLK)            — 危险高亮填充 (Ed4.0新增)
AC(CHGRF)             — 灰色填充(自动更新删除)
AC(ICEARE)            — 冰区填充色
AC(DEPIT)             — 等深线间区域填充色
```

**查询表中的使用**:

```xml
<Entry objectClass="LNDARE" priority="2" radar="O" displayCategory="BASE" viewingGroup="26010">
  <Default>AC(LANDF);LS(SOLD,1,CHBLK)</Default>
  <Description>陆地区域</Description>
</Entry>

<Entry objectClass="DEPARE" priority="2" radar="O" displayCategory="BASE" viewingGroup="26020">
  <Condition field="DRVAL1" value="0">AC(DEPVS);LS(SOLD,1,DEPCNT)</Condition>
  <Condition field="DRVAL1" value="5">AC(DEPDW);LS(SOLD,1,DEPCNT)</Condition>
</Entry>
```

**解析逻辑 (C++伪代码)**:

```cpp
void executeAC(const std::string& colorToken,
               const Polygon& polygon)
{
    std::optional<RGB> colorRgb = ColorTable::current().resolve(colorToken);
    if (!colorRgb) {
        LOG_WARN("Color token not found: " + colorToken);
        return;
    }
    fillPolygon(polygon, *colorRgb);
}
```

---

### 1.5 AP — 面填充图案

**语法**: `AP(pattern_name[, minDist])`

**功能**: 使用预定义的图案填充面物标区域。图案定义来自符号库，通常为重复的矢量图案。可选 minDist 参数控制图案重复的最小间距。

**参数**:

| 参数 | 类型 | 必选 | 说明 | 示例值 |
|------|------|------|------|--------|
| pattern_name | string | 是 | 符号库中的面填充图案名称 | DIAMOND01, ACHARE01 |
| minDist | double | 否 | 图案最小重复间距(mm) | 2.0 |

**常用面填充图案**:

| 图案名称 | 描述 | 典型用途 |
|----------|------|----------|
| DIAMOND01 | 菱形图案 | 危险区域 |
| ACHARE01 | 锚地图案 | 锚地 |
| RESARE01 | 限制区域图案 | 限制区域 |
| DNGHLK01 | 危险高亮图案 | 危险高亮 (Ed4.0) |
| CHRVDEL2 | 自动更新删除图案 | 自动更新 (Ed4.0) |
| CHRVID02 | 自动更新插入图案 | 自动更新 (Ed4.0) |
| MIPARE01 | 军事区域图案 | 军事区域 |
| ICEARE01 | 冰区图案 | 冰区 |

**完整示例**:

```
AP(DIAMOND01)         — 菱形填充图案
AP(ACHARE01)          — 锚地填充图案
AP(RESARE01)          — 限制区域填充图案
AP(DNGHLK01)          — 危险高亮填充图案 (Ed4.0新增)
AP(CHRVDEL2)          — 自动更新删除填充图案 (Ed4.0新增)
AP(MIPARE01)          — 军事区域填充图案
```

**查询表中的使用**:

```xml
<Entry objectClass="DNGHLK" priority="1" radar="N" displayCategory="ALL" viewingGroup="26300">
  <Default>AC(DNGHLK);AP(DNGHLK01)</Default>
  <Description>危险高亮区域</Description>
</Entry>
```

**解析逻辑 (C++伪代码)**:

```cpp
void executeAP(const std::string& patternName,
               const Polygon& polygon,
               double minDist = 0.0)
{
    const AreaPattern* pattern = AreaPatternLibrary::lookup(patternName);
    if (!pattern) {
        LOG_WARN("Area pattern not found: " + patternName);
        return;
    }
    BoundingBox bounds = polygon.boundingBox();
    double stepX = std::max(pattern->repeatingDistanceX(), minDist);
    double stepY = std::max(pattern->repeatingDistanceY(), minDist);
    for (double y = bounds.minY; y < bounds.maxY; y += stepY) {
        for (double x = bounds.minX; x < bounds.maxX; x += stepX) {
            if (polygon.contains(x, y)) {
                drawPatternAt(*pattern, {x, y}, ColorTable::current());
            }
        }
    }
}
```

---

### 1.6 TX — 文本显示

**语法**: `TX('format', field_name, h_align, v_align, spacing, viewing_group, x_offset, y_offset, color, priority)`

**功能**: 在物标位置显示文本内容。TX 不带边框，直接绘制文本。

**参数**:

| 序号 | 参数 | 类型 | 说明 | 示例值 |
|------|------|------|------|--------|
| 1 | format | string | 格式字符串，%s为占位符 | '%s', 'bn%s' |
| 2 | field_name | string | S-57属性字段名 | OBJNAM, DRVAL1 |
| 3 | h_align | int | 水平对齐 (1=左, 2=中, 3=右) | 2 |
| 4 | v_align | int | 垂直对齐 (1=上, 2=中, 3=下) | 1 |
| 5 | spacing | int | 字符间距 | 2 |
| 6 | viewing_group | string | 视图分组 | '15110' |
| 7 | x_offset | int | X偏移(像素, -1=默认) | -1 |
| 8 | y_offset | int | Y偏移(像素, -1=默认) | -1 |
| 9 | color | string | 文本颜色标记 | CHBLK |
| 10 | priority | int | 显示优先级 | 21 |

**格式字符串说明**:

| 格式 | 说明 | 示例输出 |
|------|------|----------|
| `%s` | 直接显示字段值 | "Beacon No.3" |
| `bn%s` | 前缀bn+字段值 | "bnBeacon No.3" |
| `%s m` | 字段值+后缀 | "10 m" |
| `Fl%s` | 灯光节奏前缀 | "Fl(3)" |

**完整示例**:

```
TX('%s','OBJNAM',2,1,2,'15110',-1,-1,CHBLK,21)
TX('%s','DRVAL1',2,1,2,'15110',-1,-1,CHBLK,21)
TX('bn%s','OBJNAM',2,1,2,'15110',-1,-1,CHBLK,21)
```

**解析逻辑 (C++伪代码)**:

```cpp
void executeTX(const std::string& formatStr,
               const std::string& fieldName,
               int hAlign, int vAlign, int spacing,
               const std::string& viewingGroup,
               int xOffset, int yOffset,
               const std::string& color, int priority,
               const Point2D& position,
               bool border = false)
{
    std::optional<std::string> fieldValue = currentObject.getAttribute(fieldName);
    if (!fieldValue) return;

    std::string text = formatString(formatStr, *fieldValue);
    RGB colorRgb = ColorTable::current().resolve(color);
    int dx = (xOffset != -1) ? xOffset : 0;
    int dy = (yOffset != -1) ? yOffset : 0;

    if (border) {
        Rect textBounds = measureText(text, spacing);
        RGB bgRgb = ColorTable::current().resolve("CHWHT").value_or(RGB{255,255,255});
        drawRect(textBounds.expanded(2), bgRgb, 0.7);
    }

    drawText(text, position, hAlign, vAlign, spacing,
             dx, dy, colorRgb, priority, border);
}
```

---

### 1.7 TE — 文本标签

**语法**: `TE('format', field_name, h_align, v_align, spacing, viewing_group, x_offset, y_offset, color, priority)`

**功能**: 与 TX 语法完全相同，但 TE 会在文本周围绘制边框（文本背景框），使文本在复杂背景上更易阅读。

**TX 与 TE 的区别**:

| 特性 | TX | TE |
|------|-----|-----|
| 边框 | 无 | 有 (半透明背景框) |
| 用途 | 一般文本 | 重要标签 |
| 可读性 | 依赖背景 | 始终清晰 |
| 典型场景 | 水深值、地名 | 灯光特征、重要标注 |

**完整示例**:

```
TE('bn%s','OBJNAM',2,1,2,'15110',-1,-1,CHBLK,21)
TE('%s','DRVAL1',2,1,2,'15110',-1,-1,CHBLK,21)
TE('Fl%s','LITCHR',2,1,2,'15110',-1,-1,CHBLK,21)
TE('%s','VALSOU',2,1,2,'22280',-1,-1,CHBLK,21)
```

**解析逻辑 (C++伪代码)**:

```cpp
void executeTE(const std::string& formatStr,
               const std::string& fieldName,
               int hAlign, int vAlign, int spacing,
               const std::string& viewingGroup,
               int xOffset, int yOffset,
               const std::string& color, int priority,
               const Point2D& position)
{
    executeTX(formatStr, fieldName, hAlign, vAlign, spacing,
              viewingGroup, xOffset, yOffset, color, priority,
              position, true);
}
```

---

## 二、逻辑控制指令

### 2.1 CS — 条件符号化调用

**语法**: `CS(procedure_name)`

**功能**: 调用条件符号化程序 (Conditional Symbolization Procedure, CSP)。当查询表无法用简单的属性-值映射描述符号化规则时，使用 CS 将决策权交给更复杂的逻辑程序。

**参数**:

| 参数 | 类型 | 说明 |
|------|------|------|
| procedure_name | string | CSP程序名称，对应S-52 Part I Appendix F中的过程 |

**CS 与普通指令的区别**:

```
普通指令路径:
  物标 → 查询表匹配 → 直接输出指令序列 → 渲染

CS指令路径:
  物标 → 查询表匹配 → CS(过程名) → CSP执行 → 输出动态指令序列 → 渲染
                                    ↑
                              读取航行人员设置
                              读取物标多属性
                              执行复杂逻辑判断
```

**完整示例**:

```
CS(LIGHTS05)          — 调用灯光条件符号化程序
CS(DEPARE01)          — 调用水深区域条件符号化程序
CS(RESARE01)          — 调用限制区域条件符号化程序
CS(OBSTRN01)          — 调用障碍物条件符号化程序
CS(WRECKS01)          — 调用沉船条件符号化程序
CS(SLCONS01)          — 调用海岸构筑物条件符号化程序
CS(SEABED01)          — 调用海底区域条件符号化程序
CS(TOPMAR01)          — 调用顶标条件符号化程序
CS(ISODGR01)          — 调用隔离危险符号化程序
CS(QUAPOS01)          — 调用位置质量符号化程序
```

**查询表中的使用**:

```xml
<Entry objectClass="LIGHTS" priority="3" radar="O" displayCategory="BASE" viewingGroup="22230">
  <Default>CS(LIGHTS05)</Default>
  <Description>灯塔/灯桩 - 条件符号化</Description>
</Entry>

<Entry objectClass="DEPARE" priority="2" radar="O" displayCategory="BASE" viewingGroup="26020">
  <Default>CS(DEPARE01)</Default>
  <Description>水深区域 - 条件符号化</Description>
</Entry>
```

**解析逻辑 (C++伪代码)**:

```cpp
std::vector<std::string> executeCS(const std::string& procedureName,
                                    const Feature& feature,
                                    const MarinerSettings& settings)
{
    const CSP* csp = CSPLibrary::lookup(procedureName);
    if (!csp) {
        LOG_WARN("CSP not found: " + procedureName);
        return {};
    }
    return csp->evaluate(feature, settings, ColorTable::current());
}
```

---

## 三、条件符号化程序 (CSP)

### 3.1 CSP 概述

CSP (Conditional Symbolization Procedure) 是 S-52 实现"动态"和"智能"决策的核心机制。当查询表无法用简单规则描述符号化逻辑时，通过 CSP 程序根据以下因素决定最终输出：

1. **航行人员设置**: 安全等深线、安全水深、双色水深模式等
2. **物标属性**: 多属性组合判断（如灯光的颜色+节奏+光弧）
3. **空间关系**: 物标间的空间关系（如重叠区域的优先级处理）
4. **显示模式**: Base / Standard / Other 显示类别

**CSP 执行流程**:

```
                    ┌──────────────────┐
                    │  CS(过程名)       │
                    └────────┬─────────┘
                             │
                    ┌────────▼─────────┐
                    │  读取物标属性      │
                    │  OBJNAM, DRVAL1   │
                    │  COLOUR, LITCHR   │
                    │  VALSOU, WATLEV   │
                    └────────┬─────────┘
                             │
                    ┌────────▼─────────┐
                    │  读取航行设置      │
                    │  安全等深线        │
                    │  安全水深          │
                    │  显示模式          │
                    └────────┬─────────┘
                             │
                    ┌────────▼─────────┐
                    │  执行条件逻辑      │
                    │  if/else/switch   │
                    └────────┬─────────┘
                             │
                    ┌────────▼─────────┐
                    │  输出指令序列      │
                    │  SY(...);LS(...)  │
                    │  AC(...);TE(...)  │
                    └──────────────────┘
```

### 3.2 核心 CSP 程序清单

| CSP名称 | 适用物标 | 功能描述 | 关键决策因素 |
|---------|---------|----------|-------------|
| LIGHTS05 | LIGHTS | 灯光符号化 | 颜色、节奏、光弧范围 |
| DEPARE01 | DEPARE | 水深区域着色 | 安全都深线、水深值 |
| RESARE01 | RESARE | 限制区域符号化 | 限制类型、航行状态 |
| RESTRN01 | RESTRN | 限制符号化 | 限制性质 |
| SEABED01 | SBDARE | 海底区域符号化 | 底质类型 |
| SLCONS01 | SLCONS | 海岸构筑物 | 构筑物类别 |
| OBSTRN01 | OBSTRN | 障碍物符号化 | 水深值、安全水深 |
| WRECKS01 | WRECKS | 沉船符号化 | 沉船类别、水深 |
| TOPMAR01 | TOPMAR | 顶标符号化 | 顶标形状 |
| BUISGL01 | BUISGL | 建筑物符号化 | 建筑类别 |
| BOYLAT01 | BOYLAT | 侧面标浮标 | 侧面标类别 |
| BOYCAR01 | BOYCAR | 方位标浮标 | 方位标类别 |
| BCNLAT01 | BCNLAT | 侧面标信标 | 侧面标类别 |
| LITFLT01 | LITFLT | 浮动灯标 | 灯光属性 |
| LITVES01 | LITVES | 灯船 | 灯光属性 |
| MIPARE01 | MIPARE | 军事区域 | 限制类型 |
| ACHARE01 | ACHARE | 锚地 | 锚地类型 |
| DRGARE01 | DRGARE | 疏浚区域 | 水深值 |
| ICEARE01 | ICEARE | 冰区 | 冰况类型 |
| ISODGR01 | — | 隔离危险符号 | 安全都深线、孤立危险模式 |
| QUAPOS01 | — | 位置质量标记 | QUAPOS属性值 |
| DNGHLK01 | DNGHLK | 危险高亮 | 危险类型 (Ed4.0) |
| CHRV01 | CHRV* | 自动更新 | 更新类型 (Ed4.0) |

### 3.3 CSP 伪代码实现

#### 3.3.1 LIGHTS05 — 灯光条件符号化

灯光是最复杂的CSP之一，需要根据颜色、节奏、光弧等多属性组合决定符号。

```cpp
std::vector<std::string> CSP_LIGHTS05(const Feature& feature,
                                       const MarinerSettings& settings)
{
    std::vector<std::string> instructions;

    auto colour = feature.getIntAttribute("COLOUR");
    auto litchr = feature.getIntAttribute("LITCHR");
    auto sigper = feature.getDoubleAttribute("SIGPER");
    auto siggrp = feature.getStringAttribute("SIGGRP");
    auto catlit = feature.getIntAttribute("CATLIT");
    auto orient = feature.getDoubleAttribute("ORIENT");
    auto valnmr = feature.getDoubleAttribute("VALNMR");
    auto litvis = feature.getIntAttribute("LITVIS");

    static const std::unordered_map<int, std::string> colourSymbolMap = {
        {1, "LIGHTS01"},    // 白光
        {2, "LIGHTS02"},    // 红光
        {3, "LIGHTS03"},    // 绿光
        {4, "LIGHTS04"},    // 蓝光
        {5, "LIGHTS05"},    // 黄光
        {6, "LIGHTS06"},    // 紫光 (Ed4.0)
    };
    std::string symbol = "LIGHTS01";
    if (colour) {
        auto it = colourSymbolMap.find(*colour);
        if (it != colourSymbolMap.end()) symbol = it->second;
    }
    instructions.push_back("SY(" + symbol + ")");

    if (orient) {
        auto sector = calculateLightSector(*orient, feature);
        instructions.push_back(renderLightArc(sector, colour.value_or(1)));
    }

    std::string lightChar = buildLightCharacterString(
        litchr, sigper, siggrp, colour);
    if (!lightChar.empty()) {
        instructions.push_back(
            "TE('" + lightChar + "','OBJNAM',2,1,2,'15110',-1,-1,CHBLK,21)");
    }

    if (catlit) {
        switch (*catlit) {
            case 7:  instructions.emplace_back("SY(LITDIR01)"); break;
            case 13: instructions.emplace_back("SY(LITTMP01)"); break;
            default: break;
        }
    }

    return instructions;
}

std::string renderLightArc(const LightSector& sector, int colour) {
    static const std::unordered_map<int, std::string> colourMap = {
        {1, "CHWHT"}, {2, "CHRED"}, {3, "CHGRN"},
        {4, "CHBLU"}, {5, "CHYEL"}, {6, "LITPNA"},
    };
    std::string color = "CHYEL";
    auto it = colourMap.find(colour);
    if (it != colourMap.end()) color = it->second;
    return "LS(SOLD,2," + color + ")";
}

std::string buildLightCharacterString(
    std::optional<int> litchr,
    std::optional<double> sigper,
    std::optional<std::string> siggrp,
    std::optional<int> colour)
{
    static const std::unordered_map<int, std::string> rhythmMap = {
        {1, "F"},    {2, "Fl"},   {3, "Oc"},   {4, "Q"},
        {5, "IQ"},   {6, "VQ"},   {7, "IVQ"},  {8, "Iso"},
        {9, "Al"},   {10, "Mo"},
    };
    std::string rhythm;
    if (litchr) {
        auto it = rhythmMap.find(*litchr);
        if (it != rhythmMap.end()) rhythm = it->second;
    }
    std::string group = siggrp ? ("(" + *siggrp + ")") : "";
    std::string period = sigper ? ("(" + std::to_string(static_cast<int>(*sigper)) + "s)") : "";
    return rhythm + group + period;
}
```

#### 3.3.2 DEPARE01 — 水深区域条件符号化

根据安全等深线和水深值动态决定颜色。

```cpp
std::vector<std::string> CSP_DEPARE01(const Feature& feature,
                                       const MarinerSettings& settings)
{
    std::vector<std::string> instructions;

    auto drval1 = feature.getDoubleAttribute("DRVAL1");
    auto drval2 = feature.getDoubleAttribute("DRVAL2");
    double safetyDepth = settings.safetyDepth;
    double safetyContour = settings.safetyContour;
    bool twoShades = settings.twoShadeMode;

    std::string color = "DEPVS";

    if (twoShades) {
        if (drval2 && *drval2 < safetyContour) {
            color = "DEPVS";
        } else {
            color = "DEPDW";
        }
    } else {
        if (drval2) {
            double d = *drval2;
            if (d <= 2)        color = "DEPVS";
            else if (d <= 5)   color = "DEPMS";
            else if (d <= 10)  color = "DEPDW";
            else if (d <= 20)  color = "DEPMD";
            else               color = "DEPDS";
        }
    }

    instructions.push_back("AC(" + color + ")");
    instructions.emplace_back("LS(SOLD,1,DEPCNT)");

    if (drval2 && std::abs(*drval2 - safetyContour) < 0.1) {
        instructions.emplace_back("LS(SOLD,2,DEPSC)");
    }

    if (drval1) {
        instructions.push_back(
            "TE('%s','DRVAL1',2,1,2,'26020',-1,-1,DEPCNT,21)");
    }

    return instructions;
}
```

#### 3.3.3 RESARE01 — 限制区域条件符号化

```cpp
std::vector<std::string> CSP_RESARE01(const Feature& feature,
                                       const MarinerSettings& settings)
{
    std::vector<std::string> instructions;

    auto catrea = feature.getIntAttribute("CATREA");
    auto restrn = feature.getIntAttribute("RESTRN");

    static const std::unordered_map<int, std::pair<std::string, std::string>> restrictionMap = {
        {1,  {"CHRED", "RESARE01"}},    // 限制区
        {2,  {"CHRED", "RESARE01"}},    // 禁航区
        {4,  {"CHORN", "MIPARE01"}},    // 军事区
        {8,  {"CHGRN", "ACHARE01"}},    // 锚地
        {12, {"CHYEL", "CSTARE01"}},    // 海关区
    };
    std::string fillColor = "CHGRD";
    std::string pattern = "RESARE01";
    if (catrea) {
        auto it = restrictionMap.find(*catrea);
        if (it != restrictionMap.end()) {
            fillColor = it->second.first;
            pattern = it->second.second;
        }
    }

    instructions.push_back("AC(" + fillColor + ")");
    instructions.push_back("AP(" + pattern + ")");
    instructions.push_back("LS(DASH,2," + fillColor + ")");

    static const std::unordered_map<int, std::string> restrnTextMap = {
        {1, "Prohibited"},    // 禁止进入
        {2, "Restricted"},    // 限制进入
        {3, "Caution"},       // 注意区域
        {7, "Anchoring"},     // 锚泊限制
        {8, "Fishing"},       // 捕鱼限制
        {14, "Entry"},        // 进入限制
    };
    if (restrn) {
        auto it = restrnTextMap.find(*restrn);
        if (it != restrnTextMap.end()) {
            instructions.push_back(
                "TE('" + it->second + "','OBJNAM',2,1,2,'24060',-1,-1,CHBLK,21)");
        }
    }

    return instructions;
}
```

#### 3.3.4 OBSTRN01 — 障碍物条件符号化

```cpp
std::vector<std::string> CSP_OBSTRN01(const Feature& feature,
                                       const MarinerSettings& settings)
{
    std::vector<std::string> instructions;

    auto catobs = feature.getIntAttribute("CATOBS");
    auto valsou = feature.getDoubleAttribute("VALSOU");
    double safetyDepth = settings.safetyDepth;

    bool isDanger = false;
    if (valsou) {
        if (*valsou < safetyDepth) isDanger = true;
        if (*valsou <= 0.0) isDanger = true;
    } else {
        isDanger = true;
    }

    if (isDanger) {
        instructions.emplace_back("SY(OBSTRN11)");
        instructions.emplace_back("AC(DNGHLK)");
        instructions.emplace_back("LS(SOLD,2,CHRED)");
    } else {
        static const std::unordered_map<int, std::string> obsSymbolMap = {
            {1, "OBSTRN01"},  // 水下障碍物
            {2, "OBSTRN02"},  // 礁石
            {3, "OBSTRN03"},  // 浅滩
            {4, "OBSTRN04"},  // 鱼栅
            {5, "OBSTRN05"},  // 鱼礁
            {6, "OBSTRN06"},  // 冰障碍
        };
        std::string symbol = "OBSTRN01";
        if (catobs) {
            auto it = obsSymbolMap.find(*catobs);
            if (it != obsSymbolMap.end()) symbol = it->second;
        }
        instructions.push_back("SY(" + symbol + ")");
        instructions.emplace_back("LS(DASH,1,CHBLK)");
    }

    if (valsou) {
        instructions.push_back(
            "TE('%s','VALSOU',2,1,2,'22280',-1,-1,CHBLK,21)");
    }

    return instructions;
}
```

#### 3.3.5 WRECKS01 — 沉船条件符号化

```cpp
std::vector<std::string> CSP_WRECKS01(const Feature& feature,
                                       const MarinerSettings& settings)
{
    std::vector<std::string> instructions;

    auto catwrk = feature.getIntAttribute("CATWRK");
    auto valsou = feature.getDoubleAttribute("VALSOU");
    auto watlev = feature.getIntAttribute("WATLEV");
    double safetyDepth = settings.safetyDepth;

    bool isDanger = false;
    if (valsou && *valsou < safetyDepth) isDanger = true;
    if (watlev && (*watlev == 2 || *watlev == 3 || *watlev == 4)) {
        isDanger = true;
    }
    if (!valsou) isDanger = true;

    if (isDanger) {
        instructions.emplace_back("SY(WRECKS11)");
        instructions.emplace_back("AC(DNGHLK)");
    } else {
        static const std::unordered_map<int, std::string> wrkSymbolMap = {
            {1, "WRECKS01"},  // 非危险沉船-已知深度
            {2, "WRECKS02"},  // 沉船残骸-干出
            {3, "WRECKS03"},  // 沉船残骸-淹没
            {4, "WRECKS04"},  // 沉船残骸-潮汐
            {5, "WRECKS05"},  // 沉船残骸-水下
        };
        std::string symbol = "WRECKS01";
        if (catwrk) {
            auto it = wrkSymbolMap.find(*catwrk);
            if (it != wrkSymbolMap.end()) symbol = it->second;
        }
        instructions.push_back("SY(" + symbol + ")");
    }

    if (valsou) {
        instructions.push_back(
            "TE('%s','VALSOU',2,1,2,'22280',-1,-1,CHBLK,21)");
    }

    return instructions;
}
```

#### 3.3.6 ISODGR01 — 隔离危险符号化

当物标满足隔离危险条件（水深小于安全等深线且远离其他安全水域）时，显示隔离危险符号。

```cpp
std::vector<std::string> CSP_ISODGR01(const Feature& feature,
                                       const MarinerSettings& settings)
{
    std::vector<std::string> instructions;

    auto valsou = feature.getDoubleAttribute("VALSOU");
    double safetyContour = settings.safetyContour;
    bool isolatedDangerMode = settings.isolatedDangerMode;

    if (!isolatedDangerMode) return instructions;

    if (valsou && *valsou < safetyContour) {
        instructions.emplace_back("SY(QUAPOS01)");
        instructions.emplace_back("AC(ISODGR)");
        instructions.emplace_back("LS(SOLD,2,CHRED)");
    }

    return instructions;
}
```

#### 3.3.7 QUAPOS01 — 位置质量标记

根据QUAPOS属性值显示位置质量标记符号。

```cpp
std::vector<std::string> CSP_QUAPOS01(const Feature& feature,
                                       const MarinerSettings& settings)
{
    std::vector<std::string> instructions;

    auto quapos = feature.getIntAttribute("QUAPOS");

    if (quapos) {
        switch (*quapos) {
            case 1:  // 测量值
                break;
            case 2:  // 估算值
                instructions.emplace_back("SY(QUAPOS01)");
                break;
            case 3:  // 推算值
                instructions.emplace_back("SY(QUAPOS02)");
                break;
            case 4:  // 不精确
                instructions.emplace_back("SY(QUAPOS03)");
                break;
            default:
                break;
        }
    }

    return instructions;
}
```

#### 3.3.8 SLCONS01 — 海岸构筑物条件符号化

```cpp
std::vector<std::string> CSP_SLCONS01(const Feature& feature,
                                       const MarinerSettings& settings)
{
    std::vector<std::string> instructions;

    auto catslc = feature.getIntAttribute("CATSLC");
    auto prim = feature.getIntAttribute("PRIM");

    if (prim == 1) {
        instructions.emplace_back("SY(SLCONS01)");
    } else if (prim == 2) {
        instructions.emplace_back("LS(SOLD,2,CHBLK)");
    } else if (prim == 3) {
        switch (catslc.value_or(0)) {
            case 1: case 3: case 4: case 5:
                instructions.emplace_back("AC(CHBLK)");
                break;
            default:
                instructions.emplace_back("AC(CHGRD)");
                break;
        }
        instructions.emplace_back("LS(SOLD,1,CHBLK)");
    }

    return instructions;
}
```

#### 3.3.9 TOPMAR01 — 顶标条件符号化

```cpp
std::vector<std::string> CSP_TOPMAR01(const Feature& feature,
                                       const MarinerSettings& settings)
{
    std::vector<std::string> instructions;

    auto topshp = feature.getIntAttribute("TOPSHP");

    static const std::unordered_map<int, std::string> topmarSymbolMap = {
        {1,  "TOPMAR01"},   // 锥形
        {2,  "TOPMAR02"},   // 罐形
        {3,  "TOPMAR03"},   // 球形
        {4,  "TOPMAR04"},   // 双锥顶对顶
        {5,  "TOPMAR05"},   // 双锥底对底
        {6,  "TOPMAR06"},   // 双锥顶对底
        {7,  "TOPMAR07"},   // 菱形
        {8,  "TOPMAR08"},   // 十字形
        {9,  "TOPMAR09"},   // 朝上三角
        {10, "TOPMAR10"},   // 朝下三角
        {11, "TOPMAR11"},   // 方形
        {12, "TOPMAR12"},   // 朝上三角/朝下三角
        {13, "TOPMAR13"},   // 双球
    };

    if (topshp) {
        auto it = topmarSymbolMap.find(*topshp);
        if (it != topmarSymbolMap.end()) {
            instructions.push_back("SY(" + it->second + ")");
        }
    }

    return instructions;
}
```

#### 3.3.10 DNGHLK01 — 危险高亮 (Ed4.0新增)

```cpp
std::vector<std::string> CSP_DNGHLK01(const Feature& feature,
                                       const MarinerSettings& settings)
{
    std::vector<std::string> instructions;

    instructions.emplace_back("AC(DNGHLK)");
    instructions.emplace_back("AP(DNGHLK01)");
    instructions.emplace_back("LS(SOLD,2,DNGHLK)");
    instructions.emplace_back("SY(DNGHLK01)");

    auto objnam = feature.getStringAttribute("OBJNAM");
    if (objnam) {
        instructions.push_back(
            "TE('%s','OBJNAM',2,1,2,'22510',-1,-1,CHBLK,21)");
    }

    return instructions;
}
```

#### 3.3.11 CHRV01 — 自动更新 (Ed4.0新增)

```cpp
std::vector<std::string> CSP_CHRV01(const Feature& feature,
                                     const MarinerSettings& settings)
{
    std::vector<std::string> instructions;

    auto updateType = feature.getStringAttribute("CHRVTYPE");
    auto prim = feature.getIntAttribute("PRIM");

    if (updateType && *updateType == "DELETE") {
        switch (prim.value_or(0)) {
            case 1:
                instructions.emplace_back("SY(CHRVDEL1)");
                break;
            case 2:
                instructions.emplace_back("LC(CHRVDEL2)");
                break;
            case 3:
                instructions.emplace_back("AC(CHGRF)");
                instructions.emplace_back("AP(CHRVDEL2)");
                instructions.emplace_back("LS(DASH,1,CHRED)");
                break;
        }
    } else if (updateType &&
               (*updateType == "INSERT" || *updateType == "MODIFY")) {
        switch (prim.value_or(0)) {
            case 1:
                instructions.emplace_back("SY(CHRVID01)");
                break;
            case 2:
                instructions.emplace_back("LC(CHRVID02)");
                break;
            case 3:
                instructions.emplace_back("AC(CHGRD)");
                instructions.emplace_back("AP(CHRVID02)");
                instructions.emplace_back("LS(SOLD,1,CHGRN)");
                break;
        }
    }

    return instructions;
}
```

---

## 四、指令组合与执行顺序

### 4.1 指令组合规则

查询表中，多个指令以分号 `;` 分隔组合，按从左到右的顺序执行：

```
AC(LANDF);LS(SOLD,1,CHBLK)              — 先填充颜色，再画边界线
AC(DEPVS);LS(SOLD,1,DEPCNT);SY(ACHARE01) — 先填充，再画边界，最后画点符号
AC(DNGHLK);AP(DNGHLK01);LS(SOLD,2,DNGHLK) — 先填色，再填图案，最后画边界
```

### 4.2 绘制层级

同一物标内，指令按以下层级从底到顶绘制：

| 层级 | 指令类型 | 说明 |
|------|---------|------|
| 1 | AC (面填充色) | 最先绘制，作为底层 |
| 2 | AP (面填充图案) | 在填充色之上 |
| 3 | LS (线型) | 边界线 |
| 4 | LC (线符号) | 复合线型 |
| 5 | SY (点符号) | 在最上层 |
| 6 | TX/TE (文本) | 最顶层 |

### 4.3 显示优先级 (Display Priority)

S-52 定义了9个显示优先级 (1-9)，数值越大越后绘制（越在上层）：

| 优先级 | 描述 | 典型物标 |
|--------|------|----------|
| 1 | 最低 | 自动更新符号 |
| 2 | 低 | 陆地、水深区域 |
| 3 | 中低 | 海岸线、危险物标 |
| 4 | 中 | 浮标、信标、建筑物 |
| 5 | 中高 | 航道、限制区域 |
| 6 | 高 | 灯光、重要标注 |
| 7 | 较高 | 航海员标注 |
| 8 | 很高 | 本船符号 |
| 9 | 最高 | 航线、危险高亮 |

### 4.4 显示类别 (Display Category)

| 类别 | 说明 | 控制方式 |
|------|------|----------|
| BASE | 基础显示 | 始终显示，不可关闭 |
| STANDARD | 标准显示 | 默认显示，可关闭 |
| OTHER | 其他显示 | 默认关闭，可手动开启 |
| ALL | 全部显示 | 始终显示 (Part II 航海员对象) |

---

## 五、指令解析引擎参考实现

### 5.1 数据结构定义

```cpp
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <functional>

struct RGB {
    uint8_t r, g, b;
};

struct Point2D {
    double x, y;
};

struct BoundingBox {
    double minX, minY, maxX, maxY;
};

struct Instruction {
    std::string name;
    std::vector<std::string> args;
};

class Feature {
public:
    std::optional<int> getIntAttribute(const std::string& name) const;
    std::optional<double> getDoubleAttribute(const std::string& name) const;
    std::optional<std::string> getStringAttribute(const std::string& name) const;
};

class Geometry {
public:
    Point2D centroid() const;
    const std::vector<Segment>& segments() const;
};

class MarinerSettings {
public:
    double safetyDepth = 10.0;
    double safetyContour = 30.0;
    bool twoShadeMode = false;
    bool isolatedDangerMode = true;
};

class ColorTable {
public:
    static const ColorTable& current();
    std::optional<RGB> resolve(const std::string& token) const;
};

class SymbolLibrary {
public:
    static const Symbol* lookup(const std::string& name);
};

class LineSymbolLibrary {
public:
    static const LineSymbol* lookup(const std::string& name);
};

class AreaPatternLibrary {
public:
    static const AreaPattern* lookup(const std::string& name);
};

class CSPLibrary {
public:
    static const CSP* lookup(const std::string& name);
};
```

### 5.2 指令解析引擎

```cpp
class S52InstructionEngine {
public:
    S52InstructionEngine()
        : marinerSettings_()
    {}

    void execute(const std::string& instructionSequence,
                 const Feature& feature,
                 const Geometry& geometry)
    {
        auto instructions = parseInstructionSequence(instructionSequence);
        for (const auto& instr : instructions) {
            executeSingle(instr, feature, geometry);
        }
    }

private:
    MarinerSettings marinerSettings_;

    std::vector<Instruction> parseInstructionSequence(const std::string& sequence)
    {
        std::vector<Instruction> result;
        if (sequence.empty()) return result;

        size_t start = 0;
        while (start < sequence.size()) {
            size_t semiPos = findSemicolon(sequence, start);
            std::string token = sequence.substr(start, semiPos - start);
            trim(token);
            if (!token.empty()) {
                result.push_back(parseSingle(token));
            }
            if (semiPos == std::string::npos) break;
            start = semiPos + 1;
        }
        return result;
    }

    Instruction parseSingle(const std::string& str)
    {
        Instruction instr;
        size_t parenOpen = str.find('(');
        if (parenOpen == std::string::npos) {
            instr.name = str;
            return instr;
        }
        instr.name = str.substr(0, parenOpen);
        size_t parenClose = str.rfind(')');
        std::string argsStr = str.substr(parenOpen + 1,
                                          parenClose - parenOpen - 1);
        instr.args = parseArgs(argsStr);
        return instr;
    }

    std::vector<std::string> parseArgs(const std::string& argsStr)
    {
        std::vector<std::string> args;
        std::string current;
        bool inQuotes = false;
        int parenDepth = 0;

        for (char ch : argsStr) {
            if (ch == '\'' && !inQuotes) {
                inQuotes = true;
                current += ch;
            } else if (ch == '\'' && inQuotes) {
                inQuotes = false;
                current += ch;
            } else if (ch == '(' && !inQuotes) {
                parenDepth++;
                current += ch;
            } else if (ch == ')' && !inQuotes) {
                parenDepth--;
                current += ch;
            } else if (ch == ',' && !inQuotes && parenDepth == 0) {
                trim(current);
                args.push_back(current);
                current.clear();
            } else {
                current += ch;
            }
        }
        trim(current);
        if (!current.empty()) args.push_back(current);
        return args;
    }

    size_t findSemicolon(const std::string& str, size_t start)
    {
        bool inQuotes = false;
        int parenDepth = 0;
        for (size_t i = start; i < str.size(); ++i) {
            if (str[i] == '\'') inQuotes = !inQuotes;
            else if (str[i] == '(' && !inQuotes) parenDepth++;
            else if (str[i] == ')' && !inQuotes) parenDepth--;
            else if (str[i] == ';' && !inQuotes && parenDepth == 0) return i;
        }
        return std::string::npos;
    }

    void executeSingle(const Instruction& instr,
                       const Feature& feature,
                       const Geometry& geometry)
    {
        static const std::unordered_map<std::string,
            std::function<void(const std::vector<std::string>&,
                               const Feature&, const Geometry&)>> dispatch = {
            {"SY", [this](auto& a, auto& f, auto& g) { execSY(a, f, g); }},
            {"LS", [this](auto& a, auto& f, auto& g) { execLS(a, f, g); }},
            {"LC", [this](auto& a, auto& f, auto& g) { execLC(a, f, g); }},
            {"AC", [this](auto& a, auto& f, auto& g) { execAC(a, f, g); }},
            {"AP", [this](auto& a, auto& f, auto& g) { execAP(a, f, g); }},
            {"TX", [this](auto& a, auto& f, auto& g) { execTX(a, f, g, false); }},
            {"TE", [this](auto& a, auto& f, auto& g) { execTX(a, f, g, true); }},
            {"CS", [this](auto& a, auto& f, auto& g) { execCS(a, f, g); }},
        };

        auto it = dispatch.find(instr.name);
        if (it != dispatch.end()) {
            it->second(instr.args, feature, geometry);
        } else {
            LOG_WARN("Unknown instruction: " + instr.name);
        }
    }

    void execSY(const std::vector<std::string>& args,
                const Feature&, const Geometry& geometry)
    {
        executeSY(args[0], geometry.centroid());
    }

    void execLS(const std::vector<std::string>& args,
                const Feature&, const Geometry& geometry)
    {
        executeLS(args[0], std::stoi(args[1]), args[2], geometry);
    }

    void execLC(const std::vector<std::string>& args,
                const Feature&, const Geometry& geometry)
    {
        executeLC(args[0], geometry);
    }

    void execAC(const std::vector<std::string>& args,
                const Feature&, const Geometry& geometry)
    {
        executeAC(args[0], static_cast<const Polygon&>(geometry));
    }

    void execAP(const std::vector<std::string>& args,
                const Feature&, const Geometry& geometry)
    {
        double minDist = (args.size() > 1) ? std::stod(args[1]) : 0.0;
        executeAP(args[0], static_cast<const Polygon&>(geometry), minDist);
    }

    void execTX(const std::vector<std::string>& args,
                const Feature&, const Geometry& geometry,
                bool border)
    {
        std::string formatStr = stripQuotes(args[0]);
        std::string fieldName = stripQuotes(args[1]);
        int hAlign = std::stoi(args[2]);
        int vAlign = std::stoi(args[3]);
        int spacing = std::stoi(args[4]);
        std::string viewingGroup = stripQuotes(args[5]);
        int xOffset = std::stoi(args[6]);
        int yOffset = std::stoi(args[7]);
        std::string color = args[8];
        int priority = std::stoi(args[9]);
        executeTX(formatStr, fieldName, hAlign, vAlign, spacing,
                  viewingGroup, xOffset, yOffset, color, priority,
                  geometry.centroid(), border);
    }

    void execCS(const std::vector<std::string>& args,
                const Feature& feature, const Geometry& geometry)
    {
        const CSP* csp = CSPLibrary::lookup(args[0]);
        if (!csp) {
            LOG_WARN("CSP not found: " + args[0]);
            return;
        }
        auto subInstructions = csp->evaluate(feature, marinerSettings_,
                                             ColorTable::current());
        for (const auto& subInstr : subInstructions) {
            auto parsed = parseSingle(subInstr);
            executeSingle(parsed, feature, geometry);
        }
    }

    static std::string stripQuotes(const std::string& s) {
        if (s.size() >= 2 && s.front() == '\'' && s.back() == '\'')
            return s.substr(1, s.size() - 2);
        return s;
    }

    static void trim(std::string& s) {
        auto ws = [](char c) { return std::isspace(static_cast<unsigned char>(c)); };
        s.erase(s.begin(), std::find_if_not(s.begin(), s.end(), ws));
        s.erase(std::find_if_not(s.rbegin(), s.rend(), ws).base(), s.end());
    }
};
```

---

## 附录

### A. 指令速查表

| 指令 | 语法 | 几何 | 示例 |
|------|------|------|------|
| SY | `SY(name[,rot])` | Point | `SY(BOYCAR01)` |
| LS | `LS(style,w,color)` | Line | `LS(SOLD,2,CHBLK)` |
| LC | `LC(name)` | Line | `LC(PIPSOL01)` |
| AC | `AC(color)` | Area | `AC(LANDF)` |
| AP | `AP(pattern[,minDist])` | Area | `AP(DIAMOND01)` |
| TX | `TX('fmt',field,ha,va,sp,vg,xo,yo,col,pri)` | All | `TX('%s','OBJNAM',2,1,2,'15110',-1,-1,CHBLK,21)` |
| TE | `TE('fmt',field,ha,va,sp,vg,xo,yo,col,pri)` | All | `TE('bn%s','OBJNAM',2,1,2,'15110',-1,-1,CHBLK,21)` |
| CS | `CS(procedure)` | All | `CS(LIGHTS05)` |

### B. 线型样式速查

| 样式 | 名称 | 图形 | dash/gap |
|------|------|------|----------|
| SOLD | 实线 | ━━━━━━━━━━ | 连续 |
| DASH | 虚线 | ━ ━ ━ ━ ━ | 5,3 |
| DOT | 点线 | · · · · · · | 1,2 |
| DASHDOT | 点划线 | ━·━·━·━· | 5,2,1,2 |

### C. 常用颜色标记速查

| 标记 | 颜色 | Day Bright | 用途 |
|------|------|-----------|------|
| CHBLK | 黑色 | #000000 | 文本/符号边框 |
| CHWHT | 白色 | #FFFFFF | 背景/高亮 |
| CHRED | 红色 | #FF0000 | 危险/警示 |
| CHGRN | 绿色 | #00FF00 | 安全/航道 |
| CHBLU | 蓝色 | #0000FF | 水深/航道 |
| CHYEL | 黄色 | #FFFF00 | 注意/海关 |
| CHORN | 橙色 | #FFA500 | 军事/特殊 |
| CHPNA | 紫色 | #FF00FF | 航线/灯光 |
| CHBRN | 棕色 | #8B4513 | 构筑物 |
| CHGRF | 灰色 | #B0B0B0 | 建筑物 |
| LANDF | 陆地色 | #C6A664 | 陆地填充 |
| DEPVS | 浅水色 | #99CCFF | 非常浅水 |
| DEPDW | 深水色 | #336699 | 深水 |
| DEPCNT | 等深线色 | #19334D | 等深线 |
| DEPSC | 安全等深线色 | #DEPCNT | 安全都深线 |
| DNGHLK | 危险高亮 | #FF00FF | 危险标记 (Ed4.0) |
| ISODGR | 隔离危险 | #FF0000 | 隔离危险区 |
| DMPINC | 数据质量 | #FFA500 | 质量指示 (Ed4.0) |
| LITPNA | 灯光紫 | #FF00FF | 灯光 (Ed4.0) |
| NODTA | 无数据 | #808080 | 无数据区域 |

### D. 版本历史

| 版本 | 日期 | 修订内容 |
|------|------|----------|
| 1.0 | 2026-05-05 | 初始版本，完整指令集与CSP伪代码 |
| 1.1 | 2026-05-05 | 修正逻辑错误，补充遗漏CSP，伪代码替换为C++ |

---

*文档生成日期: 2026-05-05*
*标准版本: IHO S-52 Presentation Library Ed 4.0.(4)*
