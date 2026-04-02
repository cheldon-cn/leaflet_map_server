# 任务清单持续执行规则

> **完整规则**: @.trae/rules/tace_config.md | @.trae/rules/tace_execution.md | @.trae/rules/tace_exception.md
> **引用规则**: @.trae/rules/tace_references.md

---

## 一、核心约束

| 约束ID | 约束内容 | 违反后果 |
|--------|----------|----------|
| C1 | 持续执行直到所有任务完成或阻塞 | ❌ 执行失败 |
| C2 | 按优先级执行：P0→P1→P2→P3 | ❌ 顺序错误 |
| C3 | 每个任务完成后自动检测继续 | ❌ 停止执行 |
| C4 | 从配置文件读取所有参数 | ❌ 配置违规 |
| C5 | 禁止执行一个任务后就停止 | ❌ 违反持续执行 |
| C6 | 并发执行前检查依赖（@.trae/rules/tace_resource_isolation.md） | ❌ 依赖冲突 |
| C7 | 执行前创建进度文件目录 | ❌ 文件写入失败 |
| C8 | 每个任务完成后执行自检清单 | ❌ 质量失控 |

---

## 二、触发条件

> **匹配模式**: @.trae/rules/tace_trigger_advanced.md

| 触发词 | 说明 |
|--------|------|
| 按任务清单tasks.md执行 | 精确匹配 |
| 按照tasks.md清单实施 | 精确匹配 |

---

## 三、执行自检

> **详细自检**: @.trae/rules/tace_self_check.md

□ 任务状态已更新为 ✅ Done 或 ❌ Failed
□ 已扫描任务清单识别下一个任务
□ 如有待执行任务，已自动继续执行

---

**版本**: v1.5