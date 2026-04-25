# echart-ui 仅依赖 echart-facade 可行性检查报告

> **检查目标**: 评估 echart-ui 移除对 echart-data、echart-alarm、echart-ais、echart-route、echart-workspace 的直接依赖，仅通过 echart-facade 调用业务层逻辑的可行性  
> **检查日期**: 2026-04-25  
> **检查范围**: echart-ui 对业务模块的所有直接引用、echart-facade API 覆盖情况、数据类型透传分析

---

## 一、echart-ui 对业务模块的直接引用清单

### 1.1 当前 echart-ui 的业务模块依赖

| 业务模块 | import 引用 | 引用文件 | 引用类型 |
|----------|------------|----------|----------|
| echart-data | `cn.cycle.echart.data.ChartFile` | DataCatalogPanel.java | 数据类型 |
| echart-alarm | 无 | - | - |
| echart-ais | 无 | - | - |
| echart-route | `cn.cycle.echart.route.Route` | RoutePanel.java | 数据类型 |
| echart-route | `cn.cycle.echart.route.Waypoint` | RoutePanel.java | 数据类型 |
| echart-workspace | `cn.cycle.echart.workspace.Workspace` | FxAppContext.java | 数据类型 |

### 1.2 引用分类

| 分类 | 说明 | 涉及类型 |
|------|------|----------|
| **数据类型引用** | UI 层需要使用业务模块的实体类作为泛型参数、显示数据 | `ChartFile`, `Route`, `Waypoint`, `Workspace` |
| **Manager 引用** | UI 层直接调用业务 Manager 的方法 | 已全部迁移到 Facade，**无残留** |
| **工具类引用** | UI 层使用业务模块的计算/校验工具 | **无** |

**关键发现**: echart-ui 对业务模块的引用已全部是**数据类型引用**，无任何 Manager 或业务逻辑的直接调用。

---

## 二、echart-facade API 对业务层逻辑的覆盖情况

### 2.1 AlarmFacade 覆盖情况

| 业务操作 | AlarmManager 方法 | AlarmFacade 方法 | 覆盖状态 |
|----------|-------------------|------------------|----------|
| 触发预警 | triggerAlert() | triggerAlert() | ✅ |
| 触发安全预警 | triggerSafetyAlert() | triggerSafetyAlert() | ✅ |
| 触发导航预警 | triggerNavigationAlert() | triggerNavigationAlert() | ✅ |
| 触发系统预警 | triggerSystemAlert() | triggerSystemAlert() | ✅ |
| 触发天气预警 | triggerWeatherAlert() | triggerWeatherAlert() | ✅ |
| 触发AIS预警 | triggerAisAlert() | triggerAisAlert() | ✅ |
| 确认预警 | acknowledgeAlert() | acknowledgeAlert() | ✅ |
| 清除预警 | clearAlert() | clearAlert() | ✅ |
| 获取预警 | getAlert() | getAlert() | ✅ |
| 获取活跃预警 | getActiveAlerts() | getActiveAlerts() | ✅ |
| 获取未确认预警 | getUnacknowledgedAlerts() | getUnacknowledgedAlerts() | ✅ |
| 按类型查询 | - | getAlertsByType() | ✅ (Facade增强) |
| 按严重度查询 | - | getAlertsBySeverity() | ✅ (Facade增强) |
| 清除所有预警 | clearAllAlerts() | clearAllAlerts() | ✅ |
| 预警计数 | getAlertCount() | getAlertCount() | ✅ |
| 是否有严重预警 | - | hasCriticalAlerts() | ✅ (Facade增强) |
| 添加监听器 | addAlarmListener() | addAlarmListener() | ✅ |
| 移除监听器 | removeAlarmListener() | removeAlarmListener() | ✅ |

**覆盖率: 100%** — AlarmFacade 完全覆盖 AlarmManager 的所有业务操作，并增加了便捷查询方法。

### 2.2 RouteFacade 覆盖情况

| 业务操作 | RouteManager 方法 | RouteFacade 方法 | 覆盖状态 |
|----------|-------------------|------------------|----------|
| 创建航线 | createRoute() | createRoute() | ✅ |
| 创建航线(含航路点) | - | createRoute(name, waypoints) | ✅ (Facade增强) |
| 删除航线 | deleteRoute() | deleteRoute() | ✅ |
| 获取航线 | getRoute() | getRoute() | ✅ |
| 获取所有航线 | getAllRoutes() | getAllRoutes() | ✅ |
| 航线计数 | getRouteCount() | getRouteCount() | ✅ |
| 设置活跃航线 | setActiveRoute() | setActiveRoute() | ✅ |
| 获取活跃航线 | getActiveRoute() | getActiveRoute() | ✅ |
| 清除活跃航线 | clearActiveRoute() | clearActiveRoute() | ✅ |
| 添加航路点 | - | addWaypoint() | ✅ (Facade增强) |
| 移除航路点 | - | removeWaypoint() | ✅ (Facade增强) |
| 移动航路点 | - | moveWaypoint() | ✅ (Facade增强) |
| 计算总距离 | - | calculateTotalDistance() | ✅ (Facade增强) |
| 计算航段距离 | - | calculateLegDistance() | ✅ (Facade增强) |
| 计算方位角 | - | calculateBearing() | ✅ (Facade增强) |
| 航线检查 | - | checkRoute() | ✅ (Facade增强) |
| 导出航线 | - | exportRoute() | ✅ (Facade增强) |
| 导入航线 | - | importRoute() | ✅ (Facade增强) |
| 复制航线 | - | copyRoute() | ✅ (Facade增强) |
| 反转航线 | - | reverseRoute() | ✅ (Facade增强) |
| 下一航路点 | - | getNextWaypoint() | ✅ (Facade增强) |
| 上一航路点 | - | getPreviousWaypoint() | ✅ (Facade增强) |
| 最近航路点 | - | findNearestWaypointIndex() | ✅ (Facade增强) |

**覆盖率: 100%** — RouteFacade 完全覆盖 RouteManager 的所有业务操作，并大幅增加了航线计算和操作方法。

### 2.3 WorkspaceFacade 覆盖情况

| 业务操作 | WorkspaceManager 方法 | WorkspaceFacade 方法 | 覆盖状态 |
|----------|----------------------|---------------------|----------|
| 创建工作区 | createWorkspace() | createWorkspace() | ✅ |
| 删除工作区 | removeWorkspace() | deleteWorkspace() | ✅ |
| 获取工作区 | getWorkspace() | getWorkspace() | ✅ |
| 获取所有工作区 | getAllWorkspaces() | getAllWorkspaces() | ✅ |
| 获取当前工作区 | getCurrentWorkspace() | getCurrentWorkspace() | ✅ |
| 设置当前工作区 | setCurrentWorkspace() | setCurrentWorkspace() | ✅ |
| 保存工作区 | - | saveWorkspace() | ✅ (Facade增强) |
| 加载工作区 | - | loadWorkspace() | ✅ (Facade增强) |
| JSON导出 | - | exportToJson() | ✅ (Facade增强) |
| XML导出 | - | exportToXml() | ✅ (Facade增强) |
| JSON导入 | - | importFromJson() | ✅ (Facade增强) |
| XML导入 | - | importFromXml() | ✅ (Facade增强) |
| 恢复点 | - | createRecoveryPoint() | ✅ (Facade增强) |
| 从恢复点恢复 | - | recoverFromLatest() | ✅ (Facade增强) |
| 注册面板 | - | registerPanel() | ✅ (Facade增强) |
| 面板管理 | - | showPanel/hidePanel/togglePanel() | ✅ (Facade增强) |
| 布局管理 | - | getCurrentLayout/applyLayout() | ✅ (Facade增强) |
| 配置更新 | - | updateWorkspaceConfig() | ✅ (Facade增强) |
| 工作区计数 | getWorkspaceCount() | getWorkspaceCount() | ✅ |
| 未保存检查 | - | hasUnsavedChanges() | ✅ (Facade增强) |

**覆盖率: 100%** — WorkspaceFacade 完全覆盖 WorkspaceManager 的所有业务操作，并增加了持久化、恢复、面板管理等功能。

### 2.4 AISFacade 覆盖情况

| 业务操作 | AISTargetManager 方法 | AISFacade 方法 | 覆盖状态 |
|----------|----------------------|----------------|----------|
| 更新目标 | - | updateTarget() | ✅ (Facade增强) |
| 获取目标 | getTarget() | getTarget() | ✅ |
| 获取所有目标 | getAllTargets() | getAllTargets() | ✅ |
| 获取活跃目标 | - | getActiveTargets() | ✅ (Facade增强) |
| 获取危险目标 | - | getDangerousTargets() | ✅ (Facade增强) |
| 移除目标 | removeTarget() | removeTarget() | ✅ |
| 清除所有目标 | clear() | clearAllTargets() | ✅ |
| 目标计数 | getTargetCount() | getTargetCount() | ✅ |
| CPA计算 | - | calculateCPA() | ✅ (Facade增强) |
| 碰撞风险检查 | - | checkCollisionRisk() | ✅ (Facade增强) |
| CPA阈值设置 | - | setCPAThreshold/getCPAThreshold() | ✅ (Facade增强) |
| TCPA阈值设置 | - | setTCPAThreshold/getTCPAThreshold() | ✅ (Facade增强) |
| 目标预警 | - | getTargetAlerts/clearTargetAlerts() | ✅ (Facade增强) |
| 目标摘要 | - | getTargetSummary/getAllTargetSummaries() | ✅ (Facade增强) |

**覆盖率: 100%** — AISFacade 完全覆盖 AISTargetManager 的所有业务操作，并增加了 CPA/TCPA 计算和碰撞风险检查。

### 2.5 ApplicationFacade 覆盖情况

| 业务操作 | ApplicationFacade 方法 | 覆盖状态 |
|----------|----------------------|----------|
| 获取已加载海图 | getLoadedCharts() | ✅ |
| 加载海图 | loadChart() | ✅ |
| 卸载海图 | unloadChart() | ✅ |
| 加载工作区 | loadWorkspace() | ✅ |
| 保存工作区 | saveWorkspace() | ✅ |
| 获取子Facade | getAlarmFacade/getRouteFacade/getWorkspaceFacade/getAisFacade() | ✅ |

**覆盖率: 100%** — ApplicationFacade 覆盖了 echart-ui 所需的所有海图和工作区操作。

---

## 三、数据类型透传问题分析

### 3.1 核心问题

echart-facade 的 API 签名中**直接返回了业务模块的数据类型**，这意味着 echart-ui 即使只依赖 echart-facade，也需要能访问到这些数据类型的类定义。

| Facade 方法 | 返回的业务数据类型 | 所属模块 |
|-------------|-------------------|----------|
| ApplicationFacade.getLoadedCharts() | `List<ChartFile>` | echart-data |
| RouteFacade.getAllRoutes() | `List<Route>` | echart-route |
| RouteFacade.createRoute() | `Route` | echart-route |
| RouteFacade.getRoute() | `Route` | echart-route |
| RouteFacade.getNextWaypoint() | `Waypoint` | echart-route |
| WorkspaceFacade.getCurrentWorkspace() | `Workspace` | echart-workspace |
| WorkspaceFacade.createWorkspace() | `Workspace` | echart-workspace |
| AISFacade.getTarget() | `AISTarget` | echart-ais |
| AISFacade.calculateCPA() | `CpaResult` | echart-alarm |
| AlarmFacade.getActiveAlerts() | `List<Alert>` | cn.cycle.chart.api.alert |
| WorkspaceFacade.registerPanel() | `Panel` | echart-workspace |
| WorkspaceFacade.getCurrentLayout() | `PanelLayout` | echart-workspace |
| RouteFacade.checkRoute() | `List<CheckResult>` | echart-route |

### 3.2 数据类型分类

| 类型 | 所属模块 | echart-ui 是否使用 | 透传方式 |
|------|----------|-------------------|----------|
| `ChartFile` | echart-data | ✅ DataCatalogPanel | Facade API 返回值 |
| `Route` | echart-route | ✅ RoutePanel | Facade API 返回值 |
| `Waypoint` | echart-route | ✅ RoutePanel | Facade API 返回值 |
| `Workspace` | echart-workspace | ✅ FxAppContext | Facade API 返回值 |
| `AISTarget` | echart-ais | ❌ 当前未使用 | Facade API 返回值 |
| `Alert` | cn.cycle.chart.api.alert | ✅ AlarmPanel | Facade API 返回值 |
| `CpaResult` | echart-alarm | ❌ 当前未使用 | Facade API 返回值 |
| `Panel` | echart-workspace | ❌ 当前未使用 | Facade API 返回值 |
| `PanelLayout` | echart-workspace | ❌ 当前未使用 | Facade API 返回值 |
| `CheckResult` | echart-route | ❌ 当前未使用 | Facade API 返回值 |

### 3.3 透传问题的影响

由于 echart-facade 的 API 返回了业务模块的数据类型，echart-ui 在编译期需要能解析这些类型。有两种解决方案：

---

## 四、可行性评估

### 4.1 方案一：Facade 引入业务模块依赖（传递依赖） ✅ 可行

**原理**: echart-facade 已依赖 echart-data、echart-alarm、echart-ais、echart-route、echart-workspace，echart-ui 依赖 echart-facade 后，通过 Gradle 的传递依赖机制自动获取这些模块的类定义。

**变更内容**:

| 变更项 | 说明 |
|--------|------|
| echart-ui/build.gradle | 移除 5 个业务模块的直接依赖，保留 echart-facade |
| echart-ui/settings.gradle | 移除 5 个业务模块的 include |
| 代码变更 | **无需变更**，数据类型通过传递依赖可用 |

**优点**:
- 变更量最小，仅修改 build.gradle 和 settings.gradle
- 数据类型通过传递依赖自动可用，无需修改任何 Java 代码
- 编译期类型安全完全保留

**缺点**:
- echart-ui 仍可"看到"业务模块的内部类（如 Manager），存在绕过 Facade 的风险
- 依赖关系不够显式，开发者可能不清楚间接依赖了哪些模块

**可行性**: ✅ **完全可行，推荐方案**

### 4.2 方案二：Facade 封装数据类型为 DTO ✅ 可行但工作量大

**原理**: 在 echart-facade 中定义 DTO（Data Transfer Object）类，封装业务模块的数据类型，Facade API 只返回 DTO，不暴露原始业务类型。

**变更内容**:

| 变更项 | 说明 |
|--------|------|
| echart-facade | 新增 DTO 类：ChartFileDTO、RouteDTO、WaypointDTO、WorkspaceDTO 等 |
| echart-facade | 修改所有 Facade 方法返回 DTO 而非原始类型 |
| echart-ui | 修改所有使用业务类型的地方改用 DTO |
| echart-ui/build.gradle | 移除 5 个业务模块的直接依赖 |
| echart-ui/settings.gradle | 移除 5 个业务模块的 include |

**优点**:
- 彻底解耦，echart-ui 完全不感知业务模块
- Facade 层完全控制数据暴露粒度
- 多端复用时各端只需依赖 facade 即可

**缺点**:
- 工作量大，需创建大量 DTO 类和转换逻辑
- DTO 与原始类型之间的转换有性能开销
- Route、Waypoint 等类型有可变状态（setName、setLatitude 等），DTO 需要同步修改回原始对象
- Alert 类型来自 cn.cycle.chart.api.alert（外部 API 包），封装成本更高

**可行性**: ✅ 技术可行，但工作量大且收益有限

### 4.3 方案三：业务模块拆分 API 包和实现包 ✅ 可行但需重构

**原理**: 将每个业务模块的数据类型（如 ChartFile、Route、Waypoint、Workspace）提取到独立的 API 子模块中，Facade 和 UI 都只依赖 API 包。

**变更内容**:

| 变更项 | 说明 |
|--------|------|
| 每个业务模块 | 拆分为 echart-xxx-api（数据类型+接口）和 echart-xxx-impl（实现） |
| echart-facade | 依赖各 xxx-api 包 |
| echart-ui | 依赖 echart-facade + 各 xxx-api 包 |

**优点**:
- 依赖关系清晰，UI 只能看到数据类型，看不到 Manager 实现
- 符合 OSGi / 模块化最佳实践
- 为后续 Java 模块化（JPMS）做准备

**缺点**:
- 重构量最大，每个业务模块都要拆分
- 需要修改所有模块的 build.gradle
- 当前项目使用 Gradle 4.5.1，对 Java 模块化支持有限

**可行性**: ✅ 技术可行，但属于架构级重构，短期不推荐

---

## 五、方案对比与推荐

### 5.1 方案对比

| 维度 | 方案一（传递依赖） | 方案二（DTO封装） | 方案三（API包拆分） |
|------|-------------------|-------------------|-------------------|
| 变更量 | 小（2个文件） | 大（10+文件） | 极大（全模块重构） |
| 解耦程度 | 中 | 高 | 高 |
| 编译安全 | ✅ 完全安全 | ✅ 完全安全 | ✅ 完全安全 |
| 运行时性能 | ✅ 无开销 | ⚠️ DTO转换开销 | ✅ 无开销 |
| 多端复用 | ⚠️ 传递依赖可见 | ✅ 完全隔离 | ✅ 完全隔离 |
| 风险 | 低 | 中 | 高 |
| 推荐度 | ⭐⭐⭐ | ⭐⭐ | ⭐ |

### 5.2 推荐方案：方案一（传递依赖）

**理由**:

1. **当前阶段最务实**: echart-ui 对业务模块的引用已全部是数据类型，无 Manager 直接调用，移除直接依赖后通过传递依赖自动满足编译需求
2. **变更量最小**: 仅需修改 build.gradle 和 settings.gradle 两个文件
3. **零代码变更**: 无需修改任何 Java 源码
4. **可渐进演进**: 后续如需进一步解耦，可在方案一基础上升级到方案二或方案三

**具体变更**:

```groovy
// echart-ui/build.gradle - 移除以下5行
// compile project(':echart-data')
// compile project(':echart-alarm')
// compile project(':echart-ais')
// compile project(':echart-route')
// compile project(':echart-workspace')

// 保留
compile project(':echart-facade')  // 通过传递依赖获取上述5个模块
```

```groovy
// echart-ui/settings.gradle - 移除以下5段
// include ':echart-data'
// project(':echart-data').projectDir = ...
// include ':echart-alarm'
// project(':echart-alarm').projectDir = ...
// include ':echart-ais'
// project(':echart-ais').projectDir = ...
// include ':echart-route'
// project(':echart-route').projectDir = ...
// include ':echart-workspace'
// project(':echart-workspace').projectDir = ...
```

### 5.3 防止绕过 Facade 的补充措施

方案一存在"仍可看到 Manager 类"的风险，建议通过以下方式约束：

| 措施 | 说明 |
|------|------|
| 代码审查规则 | 在 pr_coding_standards.md 中增加规则：echart-ui 禁止直接 import 业务 Manager 类 |
| ArchUnit 测试 | 编写架构守卫测试，检测 echart-ui 是否直接引用了 Manager 类 |
| IDE 检查 | 配置 IntelliJ 的依赖分析规则，标记 echart-ui 对 Manager 的直接引用 |

---

## 六、ApplicationFacade 接口清理建议

当前 ApplicationFacade 接口仍暴露了 5 个 Manager getter 方法，这些方法与 Facade 模式的设计意图矛盾——Facade 应封装 Manager 而非暴露 Manager。建议移除：

| 方法 | 当前状态 | 建议 |
|------|----------|------|
| `getChartFileManager()` | 返回 ChartFileManager | ❌ 移除，已有 getLoadedCharts/loadChart/unloadChart |
| `getAlarmManager()` | 返回 AlarmManager | ❌ 移除，已有 getAlarmFacade() |
| `getAisTargetManager()` | 返回 AISTargetManager | ❌ 移除，已有 getAisFacade() |
| `getRouteManager()` | 返回 RouteManager | ❌ 移除，已有 getRouteFacade() |
| `getWorkspaceManager()` | 返回 WorkspaceManager | ❌ 移除，已有 getWorkspaceFacade() |

移除后，ApplicationFacade 接口将不再 import 任何 Manager 类，仅依赖 Facade 类和数据类型，更加符合门面模式的设计原则。

---

## 七、结论

| 检查项 | 结果 | 说明 |
|--------|------|------|
| 业务逻辑覆盖 | ✅ 100% | 四个子 Facade 完全覆盖所有 Manager 的业务操作 |
| 数据类型依赖 | ⚠️ 透传 | Facade API 返回业务数据类型，需通过传递依赖解决 |
| 可行性 | ✅ 可行 | 推荐方案一（传递依赖），变更量最小 |
| 风险 | 低 | 无代码变更，仅构建配置调整 |
| 推荐度 | ⭐⭐⭐ | 当前阶段最优方案 |

**总结**: echart-ui 仅依赖 echart-facade 在技术上完全可行。四个子 Facade 已 100% 覆盖所有业务操作，echart-ui 对业务模块的引用仅剩数据类型（ChartFile、Route、Waypoint、Workspace），这些通过 Gradle 传递依赖即可满足。建议采用方案一实施，并配合代码审查规则防止绕过 Facade 直接调用 Manager。

---

**版本**: v1.0

---

## 八、实施记录（2026-04-25）

### 8.1 实际变更

| 变更项 | 文件 | 变更内容 | 状态 |
|--------|------|----------|------|
| build.gradle | echart-ui/build.gradle | 移除 echart-data、echart-alarm、echart-ais、echart-route、echart-workspace 的直接依赖 | ✅ 已完成 |
| settings.gradle | echart-ui/settings.gradle | **保留** include（Gradle project 依赖要求） | ⚠️ 调整 |
| ApplicationFacade | ApplicationFacade.java | 移除 5 个 Manager getter 方法 | ✅ 已完成 |
| DefaultApplicationFacade | DefaultApplicationFacade.java | 移除 5 个 Manager getter 实现 | ✅ 已完成 |
| 测试类 | DefaultApplicationFacadeTest.java | 移除 5 个 Manager getter 测试 | ✅ 已完成 |

### 8.2 方案调整说明

原报告建议同时移除 settings.gradle 中的 include，但实施时发现：

**Gradle 的 project 依赖要求被依赖的项目在 settings.gradle 中定义。** 传递依赖机制只对 Maven/Ivy 外部库有效，对 `project(':xxx')` 依赖无效。

因此调整方案为：
- ✅ **移除 build.gradle 中的直接依赖** — 数据类型通过传递依赖获得
- ⚠️ **保留 settings.gradle 中的 include** — Gradle 构建系统要求

### 8.3 编译验证结果

| 模块 | 编译结果 |
|------|----------|
| echart-facade | ✅ BUILD SUCCESSFUL |
| echart-ui | ✅ BUILD SUCCESSFUL |
| echart-app | ✅ BUILD SUCCESSFUL |

### 8.4 最终依赖关系

```
echart-ui
  ├── echart-core (直接依赖)
  ├── echart-i18n (直接依赖)
  ├── echart-facade (直接依赖) ← 业务逻辑统一入口
  ├── echart-theme (直接依赖)
  ├── echart-render (直接依赖)
  └── 传递依赖（通过 echart-facade）
        ├── echart-data (数据类型: ChartFile)
        ├── echart-alarm (数据类型: Alert)
        ├── echart-ais (数据类型: AISTarget)
        ├── echart-route (数据类型: Route, Waypoint)
        └── echart-workspace (数据类型: Workspace)
```

**版本**: v1.1（已实施）
