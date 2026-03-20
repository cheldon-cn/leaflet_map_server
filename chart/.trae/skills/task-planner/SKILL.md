---
name: "task-planner"
description: "Decomposes design documents into independent development tasks with priorities, milestones, and acceptance criteria. Invoke when user needs task planning or project breakdown."
---

# Task Planner Skill

## Purpose

This skill decomposes design documents into independent, actionable development tasks following software engineering best practices, with support for resource allocation, critical path analysis, and iterative planning.

## Quick Start (5 Minutes)

**Basic Usage:**
```
> 基于xxx设计文档，创建任务计划
```

**Specify Team Size:**
```
> 基于database_design.md，为3人团队创建任务计划
```

**That's it!** The system will:
1. Analyze design document structure
2. Decompose into 4-8 hour tasks
3. Calculate critical path
4. Allocate resources
5. Generate TASKS.md with milestones

**Quick Output Example:**
```markdown
# Project Task Plan

## Overview
- Total Tasks: 15
- Total Estimated Hours: 120h (PERT expected)
- Critical Path Duration: 80h
- Team Size: 3 developers

## Milestones

### M1: Foundation (Week 1-2)
| Task | Priority | Effort | Status | Assignee |
|------|----------|--------|--------|----------|
| T1: Project Setup | P0 | 4h | 📋 Todo | Dev A |
| T2: Core Interfaces | P0 | 8h | 📋 Todo | Dev A |
| T3: Database Layer | P1 | 6h | 📋 Todo | Dev B |

### Critical Path
T1 → T2 → T5 → T8 (80h)
⚠️ Any delay on these tasks will delay the project
```

## Performance Expectations

| Project Scale | Tasks Count | Planning Time | Output Size |
|---------------|-------------|---------------|-------------|
| Small (<10 modules) | 10-30 | 1-2 minutes | ~2KB TASKS.md |
| Medium (10-30 modules) | 30-80 | 2-5 minutes | ~5KB TASKS.md |
| Large (30-50 modules) | 80-150 | 5-10 minutes | ~10KB TASKS.md |
| Enterprise (>50 modules) | 150+ | 10-20 minutes | ~20KB+ TASKS.md |

**Performance Factors:**
- Document complexity: More dependencies = longer analysis
- Module count: Linear scaling with module count
- Dependency depth: Deep dependencies require more critical path calculation

**Optimization Tips:**
- Pre-process design documents with design-doc-validator
- Focus on core modules first for large projects
- Use milestone-based planning for iterative refinement

## When to Invoke

- User provides a design document and requests task decomposition
- User asks to create a project plan or roadmap
- User needs to break down complex modules into manageable tasks
- User wants to estimate effort and prioritize development work
- User needs resource allocation and team capacity planning
- User wants to update task status after completing work

## Input Requirements

### Design Document Format

设计文档应包含以下内容：

| 内容项 | 必需 | 说明 |
|--------|------|------|
| **模块列表** | ✅ 必需 | 需要实现的模块/组件名称和描述 |
| **依赖关系** | ✅ 必需 | 模块之间的依赖关系 |
| **技术栈** | ⚠️ 推荐 | 使用的技术和框架 |
| **功能描述** | ✅ 必需 | 每个模块的功能说明 |
| **接口定义** | ⚠️ 推荐 | 模块间的接口契约 |
| **性能要求** | ⚠️ 可选 | 性能指标和约束 |

### Supported Formats

| 格式 | 扩展名 | 说明 |
|------|--------|------|
| Markdown | `.md` | ✅ 推荐，最佳支持 |
| 纯文本 | `.txt` | ✅ 支持，需有清晰结构 |
| JSON | `.json` | ⚠️ 部分支持 |

### Example Input Structure

```markdown
# Module Design Document

## Overview
Brief description of the project...

## Modules
1. **Module A**: Description...
2. **Module B**: Description...

## Dependencies
- Module A → Module B
- Module B → Module C

## Technical Stack
- Language: C++
- Framework: ...
```

## Output File Management

### File Naming Convention

| Scenario | File Name | Location |
|----------|-----------|----------|
| Default | `tasks.md` | `[modulename]/doc/tasks.md` |
| Custom module name | `[modulename]_tasks.md` | `[modulename]/doc/[modulename]_tasks.md` |

### File Backup Strategy

When generating a new task list, follow these steps:

1. **Check for existing file** in target directory
2. **If file exists**, backup before overwriting:
   - Get file creation/modification time
   - Rename to: `{original_name}_{timestamp}.md`
   - Timestamp format: `YYYYMMDD_HHmmss`
3. **Never delete** existing task files

### Backup Example

```
Original: geom/doc/tasks.md
Backup:   geom/doc/tasks_20260320_143052.md

Original: geom/doc/geom_tasks.md  
Backup:   geom/doc/geom_tasks_20260320_143052.md
```

### Backup Implementation

```bash
# Check if file exists
if [ -f "geom/doc/tasks.md" ]; then
    # Get modification timestamp
    timestamp=$(stat -c %Y "geom/doc/tasks.md" 2>/dev/null || stat -f %m "geom/doc/tasks.md")
    formatted_time=$(date -d @$timestamp +"%Y%m%d_%H%M%S" 2>/dev/null || date -r $timestamp +"%Y%m%d_%H%M%S")
    
    # Create backup
    mv "geom/doc/tasks.md" "geom/doc/tasks_${formatted_time}.md"
fi
```

## Task Planning Process

### Step 1: Document Analysis

1. Read and understand the design document thoroughly
2. Identify all modules, components, and features
3. Map dependencies between components
4. Identify technical risks and challenges
5. Identify required skills and expertise

**Error Handling**:
- If document is empty: Return error "设计文档为空，请提供有效的设计文档"
- If document format is invalid: Return error "设计文档格式不正确，请使用Markdown格式"
- If modules not found: Return warning "未找到模块定义，请确保文档包含模块列表"
- If dependencies unclear: Return warning "依赖关系不明确，请检查设计文档"

### Step 2: Task Decomposition

For each module, create tasks with the following structure:

```markdown
## Task: [Task ID] - [Task Name]

### Description
- Clear, concise description of what needs to be done
- Technical approach and implementation details
- Integration points with other modules

### Reference Documents
- **Design Doc**: [设计文档名称.md](link/to/design/doc.md)
- 实施时以对应设计文档中的描述为准
- 如有疑问请查阅设计文档相关内容

### Priority
- P0: Critical/Blocking (must complete first)
- P1: High (core functionality)
- P2: Medium (important features)
- P3: Low (nice-to-have)

### Dependencies
- List of task IDs that must complete first
- External dependencies (libraries, APIs, etc.)

### Acceptance Criteria
- [ ] **Functional**: [Specific, measurable requirement]
- [ ] **Performance**: [Quantifiable metric, e.g., <100ms response]
- [ ] **Quality**: [Specific quality gate, e.g., SonarQube passed]
- [ ] **Coverage**: Line coverage ≥80%, branch coverage ≥70%
- [ ] **Documentation**: API docs generated, README updated
- [ ] **Integration**: All integration tests pass in CI
- [ ] **Review**: Code approved by at least 1 reviewer

### Estimated Effort (PERT Method)
- Optimistic (O): [X]h - Best case scenario
- Most Likely (M): [Y]h - Normal case
- Pessimistic (P): [Z]h - Worst case
- Expected: [E]h = (O + 4M + P) / 6
- Confidence: High (>80%) / Medium (50-80%) / Low (<50%)
- Complexity: Low/Medium/High
- Story Points: 1/2/3/5/8/13/21 (Fibonacci scale)

### Resource Requirements
- Required Skills: [List of skills]
- Suggested Assignee: [Developer name or role]
- Parallel Candidates: [Tasks that can run in parallel]

### Milestone
- M1: Foundation (core framework)
- M2: Core Features
- M3: Advanced Features
- M4: Polish & Optimization

### Status
- 📋 Todo / 🔄 In Progress / 🔍 In Review / ✅ Done / 🚫 Blocked
```

### Step 3: Task Granularity Guidelines

#### Size Guidelines
- **Ideal**: 4-8 hours (one developer, one day or less)
- **Maximum**: 16 hours (must be split if larger)
- **Minimum**: 2 hours (merge if smaller, unless critical)

#### When to Split
- Task involves multiple technologies/domains
- Task has multiple acceptance criteria
- Task spans multiple components
- Task estimate > 16 hours
- Task has unclear dependencies

#### How to Split
- By component: Frontend/Backend/Database
- By layer: API/Service/Repository
- By feature: Core/Edge cases/Error handling
- By test: Unit/Integration/E2E

#### Splitting Example
```
Original: "Implement user authentication" (40h)

Split into:
- T1: Design auth API contract (4h)
- T2: Implement password hashing (4h)
- T3: Create user database schema (4h)
- T4: Implement login endpoint (8h)
- T5: Implement registration endpoint (8h)
- T6: Add JWT token generation (4h)
- T7: Write unit tests (8h)
```

### Step 4: Milestone Planning

Organize tasks into milestones based on:

1. **M1 - Foundation** (Week 1-2)
   - Project setup and infrastructure
   - Core abstractions and interfaces
   - Build system and CI/CD
   - Logging and error handling

2. **M2 - Core Features** (Week 3-4)
   - Primary functionality implementation
   - Database connectivity
   - Basic geometry operations

3. **M3 - Advanced Features** (Week 5-6)
   - Complex operations
   - Performance optimizations
   - Additional adapters

4. **M4 - Polish & Optimization** (Week 7-8)
   - Documentation
   - Testing improvements
   - Performance tuning
   - Code cleanup

### Step 5: Dependency Graph & Critical Path Analysis

#### Dependency Graph
Create a visual or textual dependency graph:

```
[T1: Project Setup]
    ├── [T2: Core Interfaces]
    │       ├── [T3: Database Layer]
    │       │       ├── [T5: PostGIS Adapter]
    │       │       └── [T6: SpatiaLite Adapter]
    │       └── [T4: Geometry Types]
    │               └── [T7: WKB Converter]
    └── [T8: Connection Pool]
            └── [T9: Health Check]
```

#### Critical Path Calculation

1. **Forward Pass (Early Start/Finish)**
   - ES(T1) = 0 (start task)
   - EF(T1) = ES(T1) + Duration(T1)
   - ES(T2) = max(EF of all predecessors)

2. **Backward Pass (Late Start/Finish)**
   - LF(last_task) = EF(last_task)
   - LS(T) = LF(T) - Duration(T)
   - LF(T) = min(LS of all successors)

3. **Float Calculation**
   - Total Float = LS - ES = LF - EF
   - Free Float = min(ES of successors) - EF
   - Tasks with zero float are critical

4. **Critical Path Visualization**
```
[T1: Setup] ──────► [T2: Core] ──────► [T5: PostGIS]
   4h, ES=0            8h, ES=4           12h, ES=12
   ⚠️ CRITICAL         ⚠️ CRITICAL        ⚠️ CRITICAL

[T3: DB Layer] ──► [T6: SpatiaLite]
   6h, ES=4            10h, ES=10
   Float=2h            Float=2h

Critical Path: T1 → T2 → T5 (Total: 24h)
Project Duration: 24h minimum
```

### Step 6: Resource Allocation

#### Skill Mapping
| Task | Required Skills | Assigned To | Availability |
|------|-----------------|-------------|--------------|
| T1 | C++, CMake | Developer A | 100% |
| T2 | Database Design | Developer B | 50% |

#### Capacity Planning
- Team size: N developers
- Sprint capacity: N × sprint_days × focus_factor
- Focus factor: typically 0.6-0.8 (accounts for meetings, interruptions)

#### Parallel Execution Analysis
- Identify tasks that can run in parallel
- Identify resource conflicts
- Optimize critical path with resource leveling

#### Load Balancing
- Avoid over-allocation (>100% capacity)
- Distribute work evenly across team
- Consider cross-training for bottleneck skills

### Step 7: Risk Assessment

Identify and document risks:

| Risk | Probability | Impact | Mitigation | Owner |
|------|-------------|--------|------------|-------|
| Technical complexity | Medium | High | Prototype early | Tech Lead |
| Dependency delays | Low | Medium | Buffer time | PM |
| Performance issues | Medium | Medium | Benchmark early | Dev Team |
| Resource unavailability | Medium | High | Cross-training | Team Lead |

### Step 8: Task Status Workflow

#### Status Definitions
| Status | Definition | Entry Criteria | Exit Criteria |
|--------|------------|----------------|---------------|
| 📋 Todo | Ready to start | Dependencies met | Developer assigned |
| 🔄 In Progress | Work ongoing | Developer starts | Work complete |
| 🔍 In Review | Code review | PR submitted | Review approved |
| ✅ Done | Completed | All criteria met | N/A |
| 🚫 Blocked | Cannot proceed | Blocker identified | Blocker resolved |

#### Status Transitions
```
📋 Todo ──► 🔄 In Progress ──► 🔍 In Review ──► ✅ Done
   │              │                  │
   │              ▼                  │
   │         🚫 Blocked ─────────────┘
   │              │
   └──────────────┘
```

#### Blocked Task Handling
- Identify blocker type: Technical/Resource/External
- Escalation path: Tech Lead → PM → Stakeholder
- Resolution SLA: Critical (4h), High (1d), Medium (3d)

### Step 9: Interface Contract Definition

For tasks with integration dependencies:

#### API Contract Template
```markdown
### Interface: [Interface Name]
- **Owner**: Task ID that provides this interface
- **Consumers**: Task IDs that use this interface
- **Contract**:
  - Input: [data structure]
  - Output: [data structure]
  - Error handling: [error types]
  - Performance: [SLA requirements]
- **Version**: v1.0
- **Stability**: Stable/Evolving/Deprecated
```

#### Contract Review Process
- Owner defines initial contract
- Consumers review and approve
- Changes require consumer notification
- Breaking changes require version bump

### Step 10: Iteration & Change Management

#### Plan Review Cadence
- **Weekly**: Update task status and estimates
- **Bi-weekly**: Re-prioritize based on new information
- **Milestone**: Full plan review and adjustment

#### Change Request Process
1. **Impact Analysis**: How does change affect dependencies?
2. **Effort Re-estimation**: Update affected task estimates
3. **Stakeholder Communication**: Notify affected parties
4. **Plan Update**: Update TASKS.md with changes

#### Velocity Tracking
- Track actual vs estimated effort
- Calculate team velocity (story points/sprint)
- Use historical data to improve future estimates

#### Plan Version Control
- Version each plan iteration (v1.0, v1.1, etc.)
- Track changes in CHANGELOG
- Maintain plan history for retrospective

### Step 11: Output Format

Generate a structured task file with **Task Summary First**:

```markdown
# Project Task Plan

## Overview
- Total Tasks: X
- Total Estimated Hours: Y (PERT expected)
- Critical Path Duration: Z hours
- Target Completion: W weeks
- Team Size: N developers

## Reference Documents
- **Design Doc**: [设计文档名称.md](path/to/design_doc.md)
- 实施时以设计文档中的描述为准
- 如有疑问请查阅对应设计文档

## Task Summary

| Task ID | Task Name | Priority | Milestone | Effort | Status | Design Doc | Dependencies |
|---------|-----------|----------|-----------|--------|--------|-------------|--------------|
| T1 | Project Setup | P0 | M1 | 4h | 📋 Todo | - | - |
| T2 | Core Interfaces | P0 | M1 | 8h | 📋 Todo | design_doc_v1.md | T1 |
| T3 | Database Layer | P1 | M2 | 6h | 📋 Todo | design_doc_v1.md | T2 |
| T4 | Geometry Types | P1 | M2 | 8h | 📋 Todo | design_doc_v1.md | T2 |
| T5 | PostGIS Adapter | P1 | M2 | 12h | 📋 Todo | design_doc_v1.md | T3 |
| T6 | SpatiaLite Adapter | P2 | M3 | 10h | 📋 Todo | design_doc_v1.md | T3 |
| T7 | WKB Converter | P1 | M2 | 6h | 📋 Todo | design_doc_v1.md | T4 |
| T8 | Connection Pool | P2 | M3 | 8h | 📋 Todo | - | T1 |
| T9 | Health Check | P3 | M4 | 4h | 📋 Todo | - | T8 |

## Resource Allocation
| Developer | Skills | Allocation | Tasks |
|-----------|--------|------------|-------|
| Dev A | C++, CMake | 100% | T1, T2, T5 |
| Dev B | Database | 50% | T3, T6 |

## Critical Path
T1 → T2 → T5 (24h)
⚠️ Any delay on these tasks will delay the project

## Milestones

### M1: Foundation (Week 1-2)
| Task | Priority | Effort | Status | Assignee | Float |
|------|----------|--------|--------|----------|-------|
| T1 | P0 | 4h | 📋 Todo | Dev A | 0h ⚠️ |
| T2 | P0 | 8h | 📋 Todo | Dev A | 0h ⚠️ |

### M2: Core Features (Week 3-4)
...

## Dependency Graph
[Visual representation]

## Risk Register
[Risk table]

## Detailed Task Descriptions

### T1 - Project Setup

#### Description
- Initialize project structure with CMake
- Configure build system for multiple platforms
- Set up CI/CD pipeline

#### Priority
P0: Critical/Blocking

#### Dependencies
None

#### Acceptance Criteria
- [ ] **Functional**: Project builds on Windows/Linux/macOS
- [ ] **Quality**: No compiler warnings
- [ ] **Coverage**: N/A (setup task)
- [ ] **Documentation**: README with build instructions

#### Estimated Effort
- Optimistic: 2h
- Most Likely: 4h
- Pessimistic: 8h
- Expected: 4.33h

#### Status
📋 Todo

---

### T2 - Core Interfaces
...

## Change Log
| Version | Date | Changes | Impact |
|---------|------|---------|--------|
| v1.0 | 2026-03-19 | Initial plan | - |
```

## Task Status Update

After completing a task, update the task file:

### Update Process

1. **Locate the task file** (tasks.md or [modulename]_tasks.md)
2. **Update Task Summary table** - change status column
3. **Update Detailed Task Description** - change status section
4. **Update Milestone tables** - reflect new status
5. **Update Change Log** - record the change

### Status Update Example

**Before:**
```markdown
| T1 | Project Setup | P0 | M1 | 4h | 📋 Todo | - |
```

**After:**
```markdown
| T1 | Project Setup | P0 | M1 | 4h | ✅ Done | - |
```

### Update Command

When user says:
- "Mark T1 as done"
- "Update T2 status to in progress"
- "T3 is complete"

The skill should:
1. Read the task file
2. Update all occurrences of the task status
3. Update the change log
4. Save the updated file

## Best Practices

1. **Single Responsibility**: Each task should have one clear objective
2. **Testable**: Every task must have verifiable acceptance criteria (SMART)
3. **Estimable**: Use PERT method for better accuracy
4. **Independent**: Minimize dependencies between tasks where possible
5. **Valuable**: Each task should deliver tangible value
6. **Small**: Tasks should be 4-8 hours ideally, max 16 hours

## Constraints

**❌ 不要做的事**:
1. 不要创建超过16小时的任务（必须拆分）
2. 不要忽略任务依赖关系
3. 不要跳过关键路径分析
4. 不要在没有备份的情况下覆盖现有任务文件
5. 不要创建没有验收标准的任务
6. 不要忽略资源分配冲突
7. 不要遗漏风险登记项

**✅ 要做的事**:
1. 使用PERT方法进行估算
2. 为每个任务定义明确的验收标准
3. 识别并标记关键路径
4. 定期更新任务状态
5. 包含文档和测试任务
6. 平衡资源分配
7. 记录变更历史

## Quality Checklist

Before finalizing the task plan:

- [ ] All design requirements covered
- [ ] No orphan tasks (all dependencies resolvable)
- [ ] Effort estimates use PERT method
- [ ] Critical path identified and marked
- [ ] Resource allocation balanced (no over-allocation)
- [ ] Buffer time included for unknowns
- [ ] Test coverage requirements specified
- [ ] Documentation tasks included
- [ ] Interface contracts defined for integration points
- [ ] Risk register populated
- [ ] Change management process defined
- [ ] Task summary table present at top
- [ ] Existing task file backed up (if any)

## Example Usage

### Creating New Task Plan

```
User: "Based on database_model_design.md, create a task plan for the geom module"

AI: [Invokes task-planner skill]
    [Checks for existing geom/doc/tasks.md]
    [Backs up existing file if found]
    [Analyzes design document]
    [Identifies modules and dependencies]
    [Calculates critical path]
    [Allocates resources]
    [Generates tasks.md with:
      - Task Summary table first
      - dependencies
      - acceptance criteria
      - Milestones
      - Detailed descriptions
      - Change log]
```

### Updating Task Status

```
User: "Mark T1 as done"

AI: [Invokes task-planner skill]
    [Reads tasks.md]
    [Updates Task Summary table]
    [Updates Detailed Task Description]
    [Updates Milestone table]
    [Adds entry to Change Log]
    [Saves updated tasks.md]
```

### Complete Project Example

**Input: User Authentication Module Design**

```markdown
# User Authentication Module Design

## Overview
Implement user authentication with JWT tokens, supporting registration, login, and password reset.

## Modules
1. **Auth Controller**: Handle HTTP requests
2. **Auth Service**: Business logic
3. **User Repository**: Data access
4. **Token Service**: JWT generation and validation

## Dependencies
- Auth Controller → Auth Service
- Auth Service → User Repository
- Auth Service → Token Service

## Technical Stack
- Language: Python 3.11
- Framework: FastAPI
- Database: PostgreSQL
- Cache: Redis
```

**Output: TASKS.md**

```markdown
# User Authentication Module - Task Plan

## Overview
- Total Tasks: 12
- Total Estimated Hours: 56h (PERT expected)
- Critical Path Duration: 32h
- Target Completion: 2 weeks
- Team Size: 2 developers

## Reference Documents
- **Design Doc**: [user_auth_design.md](../doc/user_auth_design.md)
- 实施时以设计文档中的描述为准
- 如有疑问请查阅对应设计文档

## Resource Allocation
| Developer | Skills | Allocation | Tasks |
|-----------|--------|------------|-------|
| Dev A | Python, FastAPI | 100% | T1, T2, T5, T6, T9, T10 |
| Dev B | PostgreSQL, Redis | 100% | T3, T4, T7, T8, T11, T12 |

## Critical Path
T1 → T2 → T5 → T9 → T10 (32h)
⚠️ Any delay on these tasks will delay the project

## Milestones

### M1: Foundation (Week 1)
| Task | Priority | Effort | Status | Assignee | Float |
|------|----------|--------|--------|----------|-------|
| T1: Project Setup | P0 | 2h | 📋 Todo | Dev A | 0h ⚠️ |
| T2: User Model & Schema | P0 | 4h | 📋 Todo | Dev A | 0h ⚠️ |
| T3: Database Migration | P0 | 3h | 📋 Todo | Dev B | 1h |
| T4: Redis Connection | P1 | 2h | 📋 Todo | Dev B | 2h |

### M2: Core Features (Week 1-2)
| Task | Priority | Effort | Status | Assignee | Float |
|------|----------|--------|--------|----------|-------|
| T5: User Repository | P0 | 4h | 📋 Todo | Dev A | 0h ⚠️ |
| T6: Token Service | P0 | 4h | 📋 Todo | Dev A | 2h |
| T7: Password Hashing | P1 | 2h | 📋 Todo | Dev B | 3h |
| T8: Email Service | P2 | 3h | 📋 Todo | Dev B | 4h |

### M3: API Endpoints (Week 2)
| Task | Priority | Effort | Status | Assignee | Float |
|------|----------|--------|--------|----------|-------|
| T9: Auth Service | P0 | 6h | 📋 Todo | Dev A | 0h ⚠️ |
| T10: Auth Controller | P0 | 4h | 📋 Todo | Dev A | 0h ⚠️ |
| T11: Integration Tests | P1 | 4h | 📋 Todo | Dev B | 2h |
| T12: API Documentation | P2 | 2h | 📋 Todo | Dev B | 4h |

## Dependency Graph
```
[T1: Setup] ──► [T2: Model] ──► [T5: Repository] ──► [T9: Service] ──► [T10: Controller]
                      │                                        │
                      ▼                                        ▼
               [T3: Migration]                          [T11: Tests]
                      │                                        │
                      ▼                                        ▼
               [T4: Redis] ──► [T6: Token] ──► [T7: Hash] ──► [T12: Docs]
                                        │
                                        ▼
                                  [T8: Email]
```

## Risk Register
| Risk | Probability | Impact | Mitigation | Owner |
|------|-------------|--------|------------|-------|
| JWT library incompatibility | Low | High | Use proven library | Dev A |
| Redis connection issues | Medium | Medium | Add retry logic | Dev B |
| Password hashing performance | Low | Medium | Benchmark early | Dev B |

## Change Log
| Version | Date | Changes | Impact |
|---------|------|---------|--------|
| v1.0 | 2026-03-20 | Initial plan | - |
## Notes

- Always validate task plan against original requirements
- Update estimates as more information becomes available
- Track actual vs estimated effort for future planning
- Re-prioritize based on changing requirements
- Always backup existing task files before overwriting
- Task summary table should be at the top for quick reference
- Update status in all relevant sections when a task changes
- Track all changes in the Change Log
- Use historical velocity data for better estimation
- Review and adjust plan at each milestone
- Communicate changes to all stakeholders
