# Dir-Sync-Git Skill 使用说明

**版本**: 1.1  
**最后更新**: 2026-03-19  

---

## 快速开始

### 基本用法

```
> sync directories
> 目录同步
> sync to git
> 比较目录
> sync after tests
```

### 完整示例

```
用户: 测试通过后同步目录

AI: [调用dir-sync-git skill]
    [1/7] 运行前置检查... ✓
    [2/7] 加载配置... ✓
    [3/7] 比较目录... ✓ (5个修改文件, 2个新文件)
    [4/7] 处理冲突... ✓
    [5/7] 处理手动确认项... 
          → config/production.yaml 需要确认 [Y/n]: Y
    [6/7] 同步文件... ✓
    [7/7] Git操作... ✓
    
    同步完成! 分支: brch_sync_20260319
```

---

## 功能概述

Dir-Sync-Git是一个自动化目录同步和Git提交工作流的Skill，支持：

| 功能 | 说明 |
|------|------|
| 目录比较 | 比较源目录和目标目录的文件差异 |
| 文件同步 | 基于修改时间自动同步文件 |
| 冲突处理 | 检测双向修改并提供解决策略 |
| 手动确认 | 对关键文件进行手动确认 |
| Git工作流 | 自动创建分支、提交、推送 |
| 事务性同步 | 支持回滚和恢复 |

---

## 触发条件

### 触发关键词

| 关键词 | 示例 |
|--------|------|
| sync directories | "Sync directories after tests" |
| directory sync | "Run directory sync now" |
| sync to git | "Sync my code to git repo" |
| compare directories | "Compare dev and release directories" |
| sync after tests | "Tests passed, sync after tests" |
| dir-sync-git | "Run dir-sync-git skill" |
| sync dev to target | "Sync dev to target directory" |

### 触发场景

| 场景 | 触发条件 | 自动调用 |
|------|----------|----------|
| 测试后同步 | 用户说"tests passed" + "sync" | 是 |
| 手动同步 | 用户明确请求同步 | 是 |
| 定时同步 | 用户请求定期同步 | 是 |
| 预览模式 | 用户请求预览变更 | 是 |

### 不触发条件

- 用户只提到"git"但没有同步上下文
- 用户询问与同步无关的Git命令
- 用户请求比较文件（不是目录）
- 用户请求备份文件（不是同步）

---

## 配置文件

### 配置文件位置

按优先级查找配置文件：

| 优先级 | 路径 | 说明 |
|--------|------|------|
| 1 | 用户指定路径 | 如果用户提供了配置路径 |
| 2 | `.dir-sync-git.yaml` | 项目根目录下的配置 |
| 3 | `.trae/skills/dir-sync-git/config.yaml` | 默认skill配置 |

### 必需配置项

```yaml
# 必需字段
source:
  path: "e:/dev/project"  # 源目录（开发目录）

target:
  path: "e:/release/project"  # 目标目录（必须是Git仓库）
```

### 常用配置项

```yaml
# 比较设置
comparison:
  method: "builtin"  # builtin 或 beyond_compare
  use_modification_time: true

# 排除模式
exclusions:
  directories:
    - ".git"
    - "node_modules"
    - "build"
  files:
    - "*.log"
    - "*.bak"

# 手动确认项
manual_items:
  enabled: true
  items:
    - path: "config/production.yaml"
      reason: "生产配置 - 需要审核"

# 冲突解决
conflict_resolution:
  default_strategy: "manual"  # source_wins, target_wins, newer_wins, manual, skip

# Git设置
git:
  branch_prefix: "brch_"
  push_to_main: true
```

---

## 工作流程

```
┌─────────────────────────────────────────────────────────────┐
│                    Dir-Sync-Git 工作流程                     │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Step 1: 前置检查                                           │
│  ├── 检查Git是否安装                                        │
│  ├── 检查目标目录是否为Git仓库                               │
│  ├── 检查配置文件是否存在                                    │
│  ├── 检查源目录和目标目录是否存在                            │
│  └── 检查目标目录工作树是否干净                              │
│                                                             │
│  Step 2: 目录比较                                           │
│  ├── 递归扫描源目录                                          │
│  ├── 对每个文件（排除模式外）:                               │
│  │   ├── 检查目标是否存在                                    │
│  │   ├── 比较修改时间                                        │
│  │   └── 检测冲突                                            │
│  └── 生成比较报告                                            │
│                                                             │
│  Step 3: 处理手动确认项                                      │
│  ├── 检查变更文件是否匹配manual_items                        │
│  ├── 对每个手动项:                                          │
│  │   ├── 显示文件和原因                                      │
│  │   └── 请求用户确认                                        │
│  └── 继续处理已批准的文件                                    │
│                                                             │
│  Step 4: 文件同步                                           │
│  ├── 如果启用事务模式: 开始事务                              │
│  ├── 创建目标文件备份                                        │
│  ├── 复制/覆盖文件                                          │
│  ├── 记录操作日志                                            │
│  └── 如果启用事务模式: 提交事务                              │
│                                                             │
│  Step 5: Git操作                                            │
│  ├── 创建新分支: brch_{自动生成名称}                         │
│  ├── 暂存变更: git add .                                    │
│  ├── 提交变更                                                │
│  ├── 推送到远程                                              │
│  └── 如果push_to_main为true: 合并到main并推送                │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 冲突处理

### 冲突类型

| 冲突类型 | 检测方式 | 严重程度 |
|----------|----------|----------|
| 双向修改 | 源和目标都在上次同步后修改过 | 高 |
| 目标更新 | 目标文件比源文件新 | 中 |
| 删除冲突 | 一方删除，另一方修改 | 高 |
| 类型冲突 | 文件类型改变（文件↔目录） | 关键 |

### 解决策略

| 策略 | 配置值 | 行为 |
|------|--------|------|
| 源优先 | `source_wins` | 始终使用源文件 |
| 目标优先 | `target_wins` | 保持目标文件不变 |
| 较新优先 | `newer_wins` | 使用修改时间较新的文件 |
| 手动 | `manual` | 停止并询问用户 |
| 跳过 | `skip` | 跳过冲突文件，记录警告 |

### 冲突配置示例

```yaml
conflict_resolution:
  default_strategy: "manual"
  
  patterns:
    - pattern: "*.py"
      strategy: "source_wins"
    - pattern: "config/*"
      strategy: "manual"
    - pattern: "*.md"
      strategy: "newer_wins"
```

---

## 事务性同步

### 启用事务模式

```yaml
advanced:
  transactional_mode: true
  transaction_log: ".sync-transaction.log"
```

### 事务流程

```
BEGIN TRANSACTION
    ├── 创建事务ID
    └── 记录同步开始时间

BACKUP PHASE
    ├── 对每个将被覆盖的文件:
    │   ├── 复制到备份位置
    │   └── 记录: "BACKUP source → backup"
    └── 验证所有备份已创建

SYNC PHASE
    ├── 对每个文件:
    │   ├── 复制源到目标
    │   └── 记录: "SYNC source → target"
    └── 验证所有同步已完成

COMMIT TRANSACTION
    └── 标记事务已提交

ON FAILURE → ROLLBACK
    ├── 读取事务日志
    ├── 从备份恢复文件
    └── 记录: "ROLLBACK transaction_id"
```

### 恢复命令

| 命令 | 说明 |
|------|------|
| `sync-recovery status` | 显示最后事务状态 |
| `sync-recovery rollback <id>` | 回滚指定事务 |
| `sync-recovery list` | 列出所有事务 |
| `sync-recovery cleanup` | 清理旧事务日志 |

---

## AI执行约束

### 安全边界

```
SAFE (自动执行，无需确认):
├── 读取配置文件
├── 比较目录
├── 生成比较报告
├── 创建Git分支
├── 暂存变更
└── 创建提交

REQUIRES CONFIRMATION (需要确认):
├── 覆盖目标文件
├── 推送到远程仓库
├── 合并到main分支
├── 删除分支
└── 处理manual_items

NEVER AUTO-EXECUTE (绝不自动执行):
├── 强制推送 (--force)
├── 删除远程分支
├── 修改.git目录
└── 执行配置中的任意命令
```

---

## 使用示例

### 示例1: 测试后自动同步

```
用户: 测试通过，触发同步工作流

AI: [调用dir-sync-git skill]
    
    [1/7] 运行前置检查...
    ✓ Git已安装
    ✓ 目标是Git仓库
    ✓ 配置有效
    ✓ 源目录存在
    ✓ 目标目录存在
    ✓ 目标工作树干净
    
    [2/7] 加载配置... ✓
    [3/7] 比较目录... ✓
           发现5个修改文件, 2个新文件
           检测到1个冲突: src/config.py (双向修改)
    
    [4/7] 处理冲突...
           冲突: src/config.py
           源和目标都在上次同步后修改过
           解决策略: manual
           
           [用户选择1: 使用源文件]
    
    [5/7] 处理手动确认项...
           config/production.yaml 需要确认 [Y/n]: Y
    
    [6/7] 同步文件 (7个文件)...
           ████████████████████ 100%
    
    [7/7] Git操作...
           创建分支: brch_sync_20260319
           提交变更...
           推送到main? [Y/n]: Y
           合并到main...
           推送到远程...
    
    ✓ 同步完成!
    分支: brch_sync_20260319
    提交: abc1234
    文件: 7个已同步
```

### 示例2: 预览模式（Dry Run）

```
用户: 显示将要同步的内容（预览模式）

AI: [调用dir-sync-git skill，dry-run模式]
    
    预览模式 - 不会进行实际更改
    
    比较结果:
    ┌─────────────────────────────────────────────────────┐
    │ 文件                   │ 状态     │ 操作          │
    ├─────────────────────────────────────────────────────┤
    │ src/new_feature.py     │ 新文件   │ 将复制        │
    │ src/main.py            │ 已修改   │ 将更新        │
    │ src/config.py          │ 冲突     │ 需要审核      │
    │ src/utils.py           │ 未变更   │ 跳过          │
    │ .git/                  │ 已排除   │ 跳过          │
    └─────────────────────────────────────────────────────┘
    
    摘要:
    - 将同步: 2个文件
    - 冲突: 1个（需要解决）
    - 已排除: 15个文件
    
    未进行任何更改（预览模式）。
```

---

## 最佳实践

1. **首次使用前运行预览模式** - 预览变更后再执行实际同步
2. **仔细审核手动确认项** - 它们被标记是有原因的
3. **保持排除列表更新** - 将生成的文件添加到排除列表
4. **启用事务模式** - 对于关键同步，启用回滚能力
5. **同步后测试** - 验证目标仍然正常工作
6. **审核提交** - 检查生成的提交消息
7. **使用冲突解决** - 为您的工作流配置适当的策略
8. **监控进度** - 在同步过程中关注冲突和错误

---

## 错误处理

### 常见错误

| 错误 | 原因 | 解决方案 |
|------|------|----------|
| 目标不是Git仓库 | 目标路径无效 | 在目标目录运行`git init` |
| 有未提交的变更 | 目标有待处理的变更 | 先提交或暂存变更 |
| Beyond Compare未找到 | BC路径不正确 | 使用内置方法或修复路径 |
| 权限被拒绝 | 文件被锁定或无访问权限 | 关闭文件，检查权限 |
| 推送被拒绝 | 远程有新提交 | 先拉取，解决冲突 |
| 事务失败 | 同步中断 | 运行`sync-recovery rollback` |
| 检测到冲突 | 双向修改 | 使用冲突解决策略 |

### 回滚操作

如果同步导致问题：

```bash
# 方式1: Git revert（如果已提交）
cd target_directory
git log --oneline -5  # 找到同步提交
git revert HEAD       # 回滚最后一次提交
git push origin main  # 推送回滚

# 方式2: 事务回滚（如果启用了事务模式）
sync-recovery rollback tx_20260319_152030
```

---

## 版本兼容性

| 组件 | 最低版本 | 推荐版本 |
|------|----------|----------|
| Git | 2.0.0 | 2.30.0+ |
| Beyond Compare | 4.0 | 4.4+ |
| Python（示例代码） | 3.6 | 3.10+ |

---

## 版本历史

| 版本 | 日期 | 变更 |
|------|------|------|
| 1.0 | 2026-03-19 | 初始版本 |
| 1.1 | 2026-03-19 | 添加触发条件、前置检查、冲突处理、事务性同步、AI约束、进度反馈 |
