# JAR包合并方案分析

> **版本**: v1.0  
> **日期**: 2026-04-19  
> **输入文档**: echart_tasks_jar_lists_ui.md v2.1, echart_d_a_jar_tasks_check.md v3.0  
> **分析范围**: 15个JAR包的依赖关系、层级定位、多平台复用、工时分布

---

## 一、合并分析原则

### 1.1 合并可行性评估维度

| 维度 | 权重 | 说明 |
|------|------|------|
| 依赖同构性 | 25% | 依赖集合是否高度重叠 |
| 层级一致性 | 20% | 是否处于同一层级或相邻层级 |
| 平台一致性 | 15% | 多平台复用策略是否一致 |
| 功能内聚性 | 20% | 功能是否属于同一关注点 |
| 消费者一致性 | 10% | 被同一组JAR依赖 |
| 工时规模 | 10% | 合并后JAR工时是否在合理范围(≤200h) |

### 1.2 合并判定标准

| 判定 | 条件 |
|------|------|
| ✅ 适合合并 | 5个以上维度满足，合并后无副作用 |
| ⚠️ 有条件合并 | 3~4个维度满足，需解决部分问题 |
| ❌ 不适合合并 | 2个以下维度满足，或合并后有严重副作用 |

### 1.3 合并禁忌

| 禁忌 | 说明 |
|------|------|
| 跨平台混合 | 不可将平台无关JAR与平台特定JAR合并 |
| 引入传递依赖污染 | 合并后不应使消费者被迫依赖不需要的模块 |
| 破坏层级隔离 | 合并后不应使低层级JAR依赖高层级JAR |
| 工时超过200h | 合并后JAR工时过大不利于维护 |

---

## 二、当前JAR包全景

### 2.1 依赖关系总览

```
Layer 0: echart-core (44h)
Layer 1: echart-event (18h), echart-i18n (24h)
Layer 2: echart-render (112h), echart-data (20h)
Layer 3: echart-alarm (126h), echart-ais (32h), echart-route (34h), echart-workspace (94h)
Layer 4: echart-ui (142h)
Layer 5: echart-ui-render (84h), echart-theme (46h), echart-plugin (24h)
Layer 6: echart-facade (16h)
Layer 7: echart-app (228h)
```

### 2.2 平台复用分类

| 分类 | JAR包 | 数量 |
|------|-------|------|
| 跨平台可复用 | core, event, i18n, render, data, alarm, ais, route, workspace, plugin, facade | 11 |
| JavaFX专用 | ui, ui-render, theme, app | 4 |

---

## 三、候选合并方案分析

### 3.1 方案一：echart-ui + echart-theme 合并

#### 3.1.1 评估矩阵

| 维度 | 评估 | 说明 |
|------|------|------|
| 依赖同构性 | ⚠️ 部分满足 | ui依赖core(+alarm/ais/route/data)，theme依赖core+ui；合并后theme的依赖成为ui的子集 |
| 层级一致性 | ⚠️ 部分满足 | ui在L4，theme在L5；theme依赖ui，合并后消除层级差异 |
| 平台一致性 | ✅ 满足 | 两者都是JavaFX专用，不可跨平台复用 |
| 功能内聚性 | ✅ 满足 | UI控件和主题样式都属于"视觉呈现"关注点 |
| 消费者一致性 | ✅ 满足 | theme仅被app依赖，ui被ui-render/theme/app依赖；合并后仅ui-render和app依赖 |
| 工时规模 | ⚠️ 部分满足 | 合并后142+46=188h，接近200h上限 |

#### 3.1.2 合并影响分析

**合并前依赖关系**:
```
echart-theme ──► echart-ui ──► echart-core
                              ├──► echart-alarm
                              ├──► echart-ais
                              ├──► echart-route
                              └──► echart-data
echart-app ──► echart-theme
            ──► echart-ui
```

**合并后依赖关系**:
```
echart-ui(含theme) ──► echart-core
                     ├──► echart-alarm
                     ├──► echart-ais
                     ├──► echart-route
                     └──► echart-data
echart-app ──► echart-ui(含theme)
```

**变更清单**:

| 变更项 | 合并前 | 合并后 |
|--------|--------|--------|
| JAR数量 | 2个(ui+theme) | 1个(ui) |
| app依赖 | facade, i18n, ui, ui-render, theme | facade, i18n, ui, ui-render |
| theme相关类 | echart-theme.jar | echart-ui.jar的theme子包 |
| 依赖矩阵 | theme行存在 | theme行删除，ui行不变 |

#### 3.1.3 合并方案

**新JAR名称**: echart-ui.jar（保持不变）

**包结构**:
```
echart-ui.jar
├── com.echart.ui/              # 现有UI控件包
│   ├── layout/                 # 主布局框架
│   ├── ribbon/                 # Ribbon菜单栏
│   ├── sidebar/                # 侧边栏
│   ├── panel/                  # 面板管理
│   ├── statusbar/              # 状态栏
│   ├── adapter/                # 平台适配器
│   ├── alarm/                  # 预警面板
│   ├── route/                  # 航线面板
│   └── data/                   # 数据面板
├── com.echart.ui.theme/        # 新增：主题子包（从theme迁移）
│   ├── ThemeManager.java
│   ├── ThemeSwitcher.java
│   ├── HighDPIScaler.java
│   ├── HighDPILayoutAdjuster.java
│   ├── IconResourceAdapter.java
│   ├── HighContrastDetector.java
│   └── DarkThemeAlarmColorAdapter.java
└── resources/
    └── themes/                 # 主题CSS资源
```

**Gradle配置变更**:
```groovy
// echart-ui/build.gradle（合并后）
dependencies {
    compile project(':echart-core')
    compile project(':echart-alarm')
    compile project(':echart-ais')
    compile project(':echart-route')
    compile project(':echart-data')
    
    compileOnly 'org.openjfx:javafx-controls:8'
    compile 'org.controlsfx:controlsfx:8.40.14'
    compile 'com.pixelduke:fxribbon:1.0.0'
}
// 注意：theme原来只依赖core+ui，合并后theme类可访问alarm/ais/route/data
// 需确保theme类不引入对业务层的直接依赖
```

**app/build.gradle变更**:
```groovy
// 合并前
compile project(':echart-theme')
// 合并后：移除此行，theme已包含在ui中
```

**settings.gradle变更**:
```groovy
// 移除
include 'echart-theme'
```

#### 3.1.4 风险与对策

| 风险 | 等级 | 对策 |
|------|------|------|
| theme类可能意外访问业务层API | 🟡 中 | 在theme子包中设置package-info.java限制访问，代码评审时检查 |
| 合并后JAR体积增大 | 🟢 低 | 188h仍在合理范围，且theme仅7个类 |
| 构建时间增加 | 🟢 低 | theme变更频率低，增量编译影响小 |
| 主题功能与UI控件耦合 | 🟡 中 | 通过子包隔离，theme类仅依赖ui包的公共接口 |

#### 3.1.5 结论

| 判定 | 说明 |
|------|------|
| ✅ 适合合并 | 4/6维度满足，功能内聚(视觉呈现)，平台一致(JavaFX专用)，消费者一致(app) |

---

### 3.2 方案二：echart-ui + echart-ui-render 合并

#### 3.2.1 评估矩阵

| 维度 | 评估 | 说明 |
|------|------|------|
| 依赖同构性 | ❌ 不满足 | ui依赖core+业务层，ui-render依赖core+ui+render；ui-render额外依赖render |
| 层级一致性 | ⚠️ 部分满足 | ui在L4，ui-render在L5；相邻层级 |
| 平台一致性 | ✅ 满足 | 两者都是JavaFX专用 |
| 功能内聚性 | ❌ 不满足 | UI控件 vs Canvas渲染，关注点不同 |
| 消费者一致性 | ⚠️ 部分满足 | ui被3个JAR依赖，ui-render仅被app依赖 |
| 工时规模 | ❌ 不满足 | 合并后142+84=226h，超过200h上限 |

#### 3.2.2 核心问题

**传递依赖污染**:
```
合并前:
  echart-app ──► echart-ui ──► echart-core, alarm, ais, route, data
  echart-app ──► echart-ui-render ──► echart-render ──► echart-core

合并后:
  echart-app ──► echart-ui(含ui-render) ──► echart-core, alarm, ais, route, data, render
  问题：仅需要UI控件的消费者被迫依赖render模块
```

**关注点分离**:
- echart-ui: UI控件和布局（按钮、菜单、面板、对话框）
- echart-ui-render: Canvas渲染（海图绘制、瓦片缓存、测量工具）
- 两者技术栈差异大：UI控件使用JavaFX Controls，渲染使用Canvas API

#### 3.2.3 结论

| 判定 | 说明 |
|------|------|
| ❌ 不适合合并 | 依赖不同构(render依赖污染)，功能不内聚(控件vs渲染)，工时超标(226h) |

---

### 3.3 方案三：echart-ui + echart-ui-render + echart-theme 三合一

#### 3.3.1 评估矩阵

| 维度 | 评估 | 说明 |
|------|------|------|
| 依赖同构性 | ❌ 不满足 | 三者依赖集合差异大 |
| 层级一致性 | ❌ 不满足 | 跨L4和L5两个层级 |
| 平台一致性 | ✅ 满足 | 三者都是JavaFX专用 |
| 功能内聚性 | ❌ 不满足 | UI控件+Canvas渲染+主题样式，三个不同关注点 |
| 消费者一致性 | ⚠️ 部分满足 | 仅app同时依赖三者 |
| 工时规模 | ❌ 不满足 | 合并后142+84+46=272h，远超200h上限 |

#### 3.3.2 核心问题

1. **render依赖污染**: ui-render依赖render(跨平台渲染引擎)，合并后theme和ui控件被迫传递依赖render
2. **关注点混杂**: 一个JAR包含UI控件、Canvas渲染、主题管理三个不同技术领域
3. **构建耦合**: theme的CSS变更会触发整个JAR重新编译，包括无关的渲染代码
4. **维护困难**: 272h工时的JAR难以分配给单个开发者维护

#### 3.3.3 结论

| 判定 | 说明 |
|------|------|
| ❌ 不适合合并 | 6个维度仅1个满足，工时严重超标(272h)，render依赖污染，关注点混杂 |

---

### 3.4 方案四：echart-event + echart-i18n 合并

#### 3.4.1 评估矩阵

| 维度 | 评估 | 说明 |
|------|------|------|
| 依赖同构性 | ✅ 满足 | 两者都仅依赖core |
| 层级一致性 | ✅ 满足 | 两者都在L1(基础服务层) |
| 平台一致性 | ✅ 满足 | 两者都可跨平台复用 |
| 功能内聚性 | ❌ 不满足 | 事件通信 vs 国际化，完全不同的关注点 |
| 消费者一致性 | ⚠️ 部分满足 | event被5个JAR依赖，i18n被3个JAR依赖，有重叠但不完全一致 |
| 工时规模 | ✅ 满足 | 合并后18+24=42h |

#### 3.4.2 核心问题

**消费者不一致**:
```
依赖event但不依赖i18n: echart-ais, echart-workspace, echart-facade
依赖i18n但不依赖event: echart-app
同时依赖两者: echart-alarm, echart-route

合并后：所有event消费者被迫传递依赖i18n
```

**关注点分离**: 事件通信和国际化是完全不同的基础设施服务，合并违反单一职责原则

#### 3.4.3 结论

| 判定 | 说明 |
|------|------|
| ❌ 不适合合并 | 功能不内聚(事件vs国际化)，消费者不一致导致依赖污染 |

---

### 3.5 方案五：echart-alarm + echart-ais 合并

#### 3.5.1 评估矩阵

| 维度 | 评估 | 说明 |
|------|------|------|
| 依赖同构性 | ⚠️ 部分满足 | alarm依赖core+event+i18n，ais依赖core+event+alarm；ais多依赖alarm |
| 层级一致性 | ✅ 满足 | 两者都在L3(业务功能层) |
| 平台一致性 | ✅ 满足 | 两者都可跨平台复用 |
| 功能内聚性 | ⚠️ 部分满足 | 预警和AIS有关联(AIS数据触发预警)，但属于不同业务域 |
| 消费者一致性 | ❌ 不满足 | alarm被ais/plugin/facade依赖，ais仅被facade依赖 |
| 工时规模 | ❌ 不满足 | 合并后126+32=158h，接近上限 |

#### 3.5.2 核心问题

**业务域隔离**: 预警(alarm)和AIS是两个独立的业务领域：
- alarm: 预警规则、预警通知、预警声音
- ais: AIS数据接收、AIS目标跟踪、AIS目标显示

合并后预警的变更可能影响AIS的稳定性，反之亦然。

**消费者差异**: plugin依赖alarm但不依赖ais，合并后plugin被迫传递依赖ais

#### 3.5.3 结论

| 判定 | 说明 |
|------|------|
| ❌ 不适合合并 | 消费者不一致(plugin依赖alarm不依赖ais)，业务域不同，工时偏大 |

---

### 3.6 方案六：echart-data + echart-core 合并

#### 3.6.1 评估矩阵

| 维度 | 评估 | 说明 |
|------|------|------|
| 依赖同构性 | ✅ 满足 | data仅依赖core |
| 层级一致性 | ⚠️ 部分满足 | core在L0，data在L2，跨2层 |
| 平台一致性 | ✅ 满足 | 两者都可跨平台复用 |
| 功能内聚性 | ❌ 不满足 | 核心框架 vs 数据管理，不同关注点 |
| 消费者一致性 | ❌ 不满足 | core被所有JAR依赖，data仅被ui依赖 |
| 工时规模 | ✅ 满足 | 合并后44+20=64h |

#### 3.6.2 核心问题

**影响范围过大**: core被所有14个其他JAR依赖，任何data的变更都会触发全项目重新编译

**层级定位不同**: core是最底层框架(接口定义、抽象类)，data是具体数据管理实现

#### 3.6.3 结论

| 判定 | 说明 |
|------|------|
| ❌ 不适合合并 | 层级不一致(L0 vs L2)，影响范围过大(core被所有JAR依赖)，功能不内聚 |

---

### 3.7 方案七：echart-plugin + echart-facade 合并

#### 3.7.1 评估矩阵

| 维度 | 评估 | 说明 |
|------|------|------|
| 依赖同构性 | ❌ 不满足 | plugin依赖core+alarm，facade依赖7个JAR |
| 层级一致性 | ❌ 不满足 | plugin在L5，facade在L6 |
| 平台一致性 | ✅ 满足 | 两者都可跨平台复用 |
| 功能内聚性 | ❌ 不满足 | 插件系统 vs 服务门面，不同关注点 |
| 消费者一致性 | ❌ 不满足 | plugin被facade依赖，facade被app依赖 |
| 工时规模 | ✅ 满足 | 合并后24+16=40h |

#### 3.7.2 核心问题

**依赖爆炸**: facade已经依赖7个JAR，合并plugin后不会减少反而增加复杂度

**层级违反**: facade在L6依赖plugin在L5，合并后无法维持层级结构

#### 3.7.3 结论

| 判定 | 说明 |
|------|------|
| ❌ 不适合合并 | 依赖不同构，层级不一致，功能不内聚 |

---

## 四、合并方案汇总

### 4.1 方案对比

| 方案 | 合并对象 | 判定 | 核心原因 | 合并后工时 |
|------|---------|------|---------|-----------|
| 方案一 | ui + theme | ✅ 适合 | 功能内聚、平台一致、消费者一致 | 188h |
| 方案二 | ui + ui-render | ❌ 不适合 | render依赖污染、功能不内聚、工时超标 | 226h |
| 方案三 | ui + ui-render + theme | ❌ 不适合 | 工时严重超标、依赖污染、关注点混杂 | 272h |
| 方案四 | event + i18n | ❌ 不适合 | 功能不内聚、消费者不一致 | 42h |
| 方案五 | alarm + ais | ❌ 不适合 | 消费者不一致、业务域不同 | 158h |
| 方案六 | data + core | ❌ 不适合 | 层级不一致、影响范围过大 | 64h |
| 方案七 | plugin + facade | ❌ 不适合 | 依赖不同构、层级不一致 | 40h |

### 4.2 推荐方案

**唯一推荐: 方案一 — echart-ui + echart-theme 合并**

合并后JAR包结构从15个减少到14个，减少1个JAR包管理开销。

---

## 五、方案一详细实施计划

### 5.1 合并步骤

| 步骤 | 内容 | 工时 |
|------|------|------|
| 1 | 创建echart-ui.jar的theme子包 | 0.5h |
| 2 | 迁移theme的7个Java类到ui.jar的theme子包 | 1h |
| 3 | 迁移theme的CSS资源文件到ui.jar | 0.5h |
| 4 | 更新theme类的包声明和import | 1h |
| 5 | 更新ui.jar的build.gradle（无需变更，theme依赖已是ui的子集） | 0h |
| 6 | 更新app/build.gradle移除theme依赖 | 0.5h |
| 7 | 更新settings.gradle移除echart-theme | 0.5h |
| 8 | 更新依赖详解表和依赖矩阵 | 1h |
| 9 | 编译验证 | 1h |
| **合计** | - | **6h** |

### 5.2 合并后依赖关系变更

**依赖详解表变更**:

| JAR包 | 合并前依赖 | 合并后依赖 |
|-------|-----------|-----------|
| echart-ui | echart-core, echart-alarm, echart-ais, echart-route, echart-data | 不变 |
| echart-theme | echart-core, echart-ui | **删除（已合并到ui）** |
| echart-app | echart-facade, echart-i18n, echart-ui, echart-ui-render, echart-theme | echart-facade, echart-i18n, echart-ui, echart-ui-render |

**依赖矩阵变更**:

| JAR包 | core | ui | ui-render | event | i18n | render | data | alarm | ais | route | workspace | theme | plugin | facade |
|-------|------|-----|-----------|-------|------|--------|------|-------|-----|-------|-----------|-------|--------|--------|
| echart-ui | ✅ | - | | | | | ✅ | ✅ | ✅ | ✅ | | | | |
| ~~echart-theme~~ | ~~✅~~ | ~~✅~~ | | | | | | | | | | ~~-~~ | | |
| echart-app | | ✅ | ✅ | | ✅ | | | | | | | ~~✅~~ | | ✅ |

### 5.3 合并后层级划分

```
Layer 0 - 核心层
  └── echart-core

Layer 1 - 基础服务层
  ├── echart-event
  └── echart-i18n

Layer 2 - 核心功能层
  ├── echart-render
  └── echart-data

Layer 3 - 业务功能层
  ├── echart-alarm
  ├── echart-ais
  ├── echart-route
  └── echart-workspace

Layer 4 - UI层（含主题管理）
  └── echart-ui          ← 合并theme后，UI层同时包含控件和主题

Layer 5 - 扩展功能层
  ├── echart-ui-render
  └── echart-plugin

Layer 6 - 服务门面层
  └── echart-facade

Layer 7 - 应用层
  └── echart-app
```

### 5.4 合并后工时分布

| JAR包 | 合并前工时 | 合并后工时 | 变化 |
|-------|-----------|-----------|------|
| echart-ui | 142h | **188h** | +46h(含theme) |
| echart-theme | 46h | **删除** | 合并到ui |
| 其他JAR | 不变 | 不变 | - |
| **总工时** | 1044h | **1044h** | 不变 |

---

## 六、echart-ui / echart-ui-render / echart-theme 三合一专项分析

> 本节针对用户问题：`echart_tasks_jar_lists_ui.md#L1019-1021` 三个JAR是否适合合并为一个JAR

### 6.1 三个JAR概览

| JAR包 | 层级 | 依赖 | 被依赖 | 平台 | 实际工时 | 核心职责 |
|-------|------|------|--------|------|---------|---------|
| echart-ui | L4 | core, alarm, ais, route, data | ui-render, theme, app | JavaFX专用 | 142h | UI控件和布局 |
| echart-ui-render | L5 | core, ui, render | app | JavaFX专用 | 84h | Canvas渲染 |
| echart-theme | L5 | core, ui | app | JavaFX专用 | 46h | 主题管理 |

### 6.2 三合一可行性评估

#### 6.2.1 支持合并的理由

| 理由 | 分析 |
|------|------|
| 平台一致 | ✅ 三者都是JavaFX专用，不可跨平台复用。合并不会破坏多平台策略 |
| 消费者单一 | ✅ 仅echart-app同时依赖三者，无其他JAR依赖ui-render或theme |
| 减少JAR数量 | ✅ 从15个减少到13个，简化依赖管理 |
| 部署简化 | ✅ JavaFX应用只需一个UI JAR而非三个 |

#### 6.2.2 反对合并的理由

| 理由 | 分析 | 严重度 |
|------|------|--------|
| **render依赖污染** | ui-render依赖echart-render(跨平台渲染引擎)。合并后，仅需要UI控件的场景也必须引入render依赖链。theme和ui控件完全不涉及Canvas渲染，却被迫传递依赖render | 🔴 严重 |
| **关注点混杂** | 三个JAR分属不同技术领域：(1)UI控件(JavaFX Controls/Ribbon)，(2)Canvas渲染(GraphicsContext/Canvas API)，(3)主题管理(CSS/样式表)。合并后一个JAR承载三个不同技术栈 | 🔴 严重 |
| **工时超标** | 合并后142+84+46=272h，远超200h合理上限。单个开发者难以独立维护 | 🟡 中等 |
| **构建耦合** | theme的CSS修改会触发包含渲染代码的整个JAR重新编译，反之亦然。增量编译效率降低 | 🟡 中等 |
| **层级破坏** | ui在L4，ui-render和theme在L5。合并后无法清晰定义层级位置 | 🟡 中等 |
| **未来扩展困难** | 如果需要为Android/Web实现UI，需要替换整个合并JAR，而非仅替换ui部分 | 🟡 中等 |

#### 6.2.3 render依赖污染详解

这是三合一方案最核心的问题：

```
当前依赖链（清晰）:
  app ──► ui ──► core, alarm, ais, route, data    (UI控件，无render依赖)
  app ──► ui-render ──► core, ui, render           (Canvas渲染，需要render)
  app ──► theme ──► core, ui                       (主题管理，无render依赖)

三合一后依赖链（污染）:
  app ──► echart-javafx ──► core, alarm, ais, route, data, render
                                    ↑
                          render是仅ui-render需要的依赖
                          但theme和ui控件被迫传递依赖
```

**具体影响**:
- echart-render.jar 依赖JNI Bridge(ogc_chart_jni.dll)
- 合并后，仅使用UI控件(如对话框、菜单)的场景也必须加载native库
- 增加JAR包体积(render相关类约84h工作量)
- 增加启动时间(需要加载render模块的初始化逻辑)

### 6.3 三合一结论

| 判定 | 说明 |
|------|------|
| ❌ 不适合三合一 | render依赖污染是致命问题，关注点混杂导致维护困难，272h工时超标 |

### 6.4 替代方案：部分合并（推荐）

采用**方案一(ui+theme)**替代三合一：

| 对比项 | 三合一 | 部分合并(ui+theme) |
|--------|--------|-------------------|
| JAR数量 | 13个 | 14个 |
| render依赖污染 | ❌ 有 | ✅ 无 |
| 关注点分离 | ❌ 混杂 | ✅ 清晰(控件+主题 vs 渲染) |
| 合并后工时 | 272h(超标) | 188h(合理) |
| 层级清晰度 | ❌ 模糊 | ✅ 清晰(ui在L4含theme，ui-render在L5) |
| 构建隔离 | ❌ 全耦合 | ✅ 渲染独立构建 |

**部分合并后的依赖关系**:

```
Layer 4: echart-ui (含theme) ──► core, alarm, ais, route, data
Layer 5: echart-ui-render ──► core, ui(含theme), render
Layer 7: echart-app ──► facade, i18n, ui(含theme), ui-render
```

---

## 七、合并后全景

### 7.1 合并后JAR包列表（14个）

| 序号 | JAR包 | 层级 | 工时 | 平台复用 |
|------|-------|------|------|---------|
| 1 | echart-core | L0 | 44h | ✅ 跨平台 |
| 2 | echart-event | L1 | 18h | ✅ 跨平台 |
| 3 | echart-i18n | L1 | 24h | ✅ 跨平台 |
| 4 | echart-render | L2 | 112h | ✅ 跨平台 |
| 5 | echart-data | L2 | 20h | ✅ 跨平台 |
| 6 | echart-alarm | L3 | 126h | ✅ 跨平台 |
| 7 | echart-ais | L3 | 32h | ✅ 跨平台 |
| 8 | echart-route | L3 | 34h | ✅ 跨平台 |
| 9 | echart-workspace | L3 | 94h | ✅ 跨平台 |
| 10 | **echart-ui(含theme)** | **L4** | **188h** | ❌ JavaFX专用 |
| 11 | echart-ui-render | L5 | 84h | ❌ JavaFX专用 |
| 12 | echart-plugin | L5 | 24h | ✅ 跨平台 |
| 13 | echart-facade | L6 | 16h | ✅ 跨平台 |
| 14 | echart-app | L7 | 228h | ❌ JavaFX专用 |

### 7.2 合并后依赖矩阵

| JAR包 | core | ui | ui-render | event | i18n | render | data | alarm | ais | route | workspace | plugin | facade |
|-------|------|-----|-----------|-------|------|--------|------|-------|-----|-------|-----------|--------|--------|
| echart-core | - | | | | | | | | | | | | |
| echart-ui | ✅ | - | | | | | ✅ | ✅ | ✅ | ✅ | | | |
| echart-ui-render | ✅ | ✅ | - | | | ✅ | | | | | | | |
| echart-event | ✅ | | | - | | | | | | | | | |
| echart-i18n | ✅ | | | | - | | | | | | | | |
| echart-render | ✅ | | | | | - | | | | | | | |
| echart-data | ✅ | | | | | | - | | | | | | |
| echart-alarm | ✅ | | | ✅ | ✅ | | | - | | | | | |
| echart-ais | ✅ | | | ✅ | | | | ✅ | - | | | | |
| echart-route | ✅ | | | ✅ | ✅ | | | | | - | | | |
| echart-workspace | ✅ | | | ✅ | | | | | | | - | | |
| echart-plugin | ✅ | | | | | | | ✅ | | | | - | |
| echart-facade | ✅ | | | ✅ | | | | ✅ | ✅ | ✅ | ✅ | ✅ | - |
| echart-app | | ✅ | ✅ | | ✅ | | | | | | | | ✅ |

---

*分析完成。唯一可行的合并方案为 echart-ui + echart-theme，三合一方案因render依赖污染和工时超标不建议采用。*
