---
name: "task-planner"
description: "Decomposes design documents into independent development tasks with priorities, milestones, and acceptance criteria. Invoke when user needs task planning or project breakdown."
---

# Task Planner Skill

## Purpose

This skill decomposes design documents into independent, actionable development tasks following software engineering best practices, with support for resource allocation, critical path analysis, and iterative planning.

## When to Invoke

- User provides a design document and requests task decomposition
- User asks to create a project plan or roadmap
- User needs to break down complex modules into manageable tasks
- User wants to estimate effort and prioritize development work
- User needs resource allocation and team capacity planning

## Task Planning Process

### Step 1: Document Analysis

1. Read and understand the design document thoroughly
2. Identify all modules, components, and features
3. Map dependencies between components
4. Identify technical risks and challenges
5. Identify required skills and expertise

### Step 2: Task Decomposition

For each module, create tasks with the following structure:

```markdown
## Task: [Task ID] - [Task Name]

### Description
- Clear, concise description of what needs to be done
- Technical approach and implementation details
- Integration points with other modules

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

Generate a structured TASKS.md file:

```markdown
# Project Task Plan

## Overview
- Total Tasks: X
- Total Estimated Hours: Y (PERT expected)
- Critical Path Duration: Z hours
- Target Completion: W weeks
- Team Size: N developers

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

## Change Log
| Version | Date | Changes | Impact |
|---------|------|---------|--------|
| v1.0 | 2026-03-19 | Initial plan | - |
```

## Best Practices

1. **Single Responsibility**: Each task should have one clear objective
2. **Testable**: Every task must have verifiable acceptance criteria (SMART)
3. **Estimable**: Use PERT method for better accuracy
4. **Independent**: Minimize dependencies between tasks where possible
5. **Valuable**: Each task should deliver tangible value
6. **Small**: Tasks should be 4-8 hours ideally, max 16 hours

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

## Example Usage

```
User: "Based on database_model_design.md, create a task plan for the C++ core library"

AI: [Invokes task-planner skill]
    [Analyzes design document]
    [Identifies modules and dependencies]
    [Calculates critical path]
    [Allocates resources]
    [Generates TASKS.md with milestones, dependencies, and acceptance criteria]
```

## Notes

- Always validate task plan against original requirements
- Update estimates as more information becomes available
- Track actual vs estimated effort for future planning
- Re-prioritize based on changing requirements
- Use historical velocity data for better estimation
- Review and adjust plan at each milestone
- Communicate changes to all stakeholders
