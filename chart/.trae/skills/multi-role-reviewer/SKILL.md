---
name: "multi-role-reviewer"
description: "Multi-role cross-review skill with voting. Invoke for collaborative multi-perspective skill/document review until score > 95."
---

# Multi-Role Cross-Review System

A collaborative review system where multiple expert roles participate in cross-review, critique each other's findings, and vote on correctness. The process continues iteratively until the document score exceeds 95.

## Quick Start (5 Minutes)

**Basic Usage:**
```
> 请对xxx.md进行多角色交叉评审，直到评分>95
```

**Specify Roles:**
```
> 请以数据库架构师、性能工程师、安全专家三个角色交叉评审xxx.md
```

**That's it!** The system will:
1. Auto-select expert roles (or use your specified ones)
2. Each role reviews independently
3. Roles vote on each other's findings
4. Document improves iteratively
5. Stops when score ≥ 95

**Business Scenario Example:**
```
场景：评审一个电商系统的订单服务设计文档

用户输入：
> 请对order_service_design.md进行多角色交叉评审，直到评分>95

系统行为：
1. 自动选择角色：微服务架构师、性能工程师、安全专家
2. 微服务架构师发现：服务拆分粒度过细，建议合并订单和库存服务
3. 性能工程师发现：缺少缓存策略，建议添加Redis缓存层
4. 安全专家发现：支付接口缺少签名验证，建议添加HMAC签名
5. 交叉投票：三个角色都同意安全问题(权重1.0)，部分同意服务合并(权重0.67)
6. 按优先级修复问题
7. 迭代直到评分>95
```

**Voting Process Example:**
```
投票过程详解：

发现："支付接口缺少签名验证"
提出者：安全专家

投票矩阵：
┌─────────────────┬────────────┬────────────────────────────────┐
│ 角色            │ 投票       │ 理由                           │
├─────────────────┼────────────┼────────────────────────────────┤
│ 安全专家        │ (提出者)   │ 支付安全必须                   │
│ 微服务架构师    │ ✅ 同意    │ 安全问题是高优先级             │
│ 性能工程师      │ ✅ 同意    │ 签名验证开销可接受             │
└─────────────────┴────────────┴────────────────────────────────┘

权重计算：(1.0 + 1.0) / 2 = 1.0 → 高优先级，必须修复
```

## Key Differences from technical-reviewer

| Aspect | technical-reviewer | multi-role-reviewer |
|---------|-------------------|---------------------|
| **Review Mode** | Single expert role | Multiple expert roles |
| **Iteration** | Sequential (fix → re-review) | Parallel + Competitive |
| **Decision Making** | Expert decides | Roles vote on correctness |
| **Progression** | Wait until score ≥ 95 | Cross-review anytime |
| **Best For** | Domain-specific deep review | Comprehensive multi-perspective review |

## When to Invoke

Invoke this skill when:
- User wants multiple expert perspectives on a document
- User mentions "交叉评审" (cross-review), "多角色评审" (multi-role review)
- User wants competitive review where correct opinions get more weight
- User wants collaborative improvement with role voting
- Complex documents requiring multiple domain expertise

### Trigger Examples

**Definitely Invoke multi-role-reviewer:**
```
> 请对xxx进行交叉评审
> 请多角色评审xxx文档
> 请以多个专家的视角评审xxx
> 请让不同角色投票评审xxx
> 请对xxx进行多角度评审
> 请交叉审核xxx直到评分>95
```

**Do NOT Invoke (use technical-reviewer instead):**
```
> 请评审xxx文档 (单一角色)
> 请以数据库专家身份评审xxx (指定单一角色)
> 请审计xxx设计 (无交叉评审关键词)
> 作为安全专家评审xxx (单一角色)
```

**Ambiguous - Ask User:**
```
> 请评审xxx
  → Ask: "需要单角色评审还是多角色交叉评审？"
```

## Skill Selection Priority

When user request matches multiple skills, use this priority order:

### Priority Rules

| Priority | Skill | Trigger Keywords | When to Use |
|----------|-------|------------------|-------------|
| 1 | **User-specified** | User explicitly names a skill | Always honor user's explicit choice |
| 2 | **multi-role-reviewer** | "交叉评审", "多角色", "投票", "cross-review", "multi-role" | When multiple perspectives needed |
| 3 | **technical-reviewer** | "评审", "审计", "review", "audit" (without multi-role keywords) | When single expert perspective sufficient |

### Decision Flow

```
User Request: "评审xxx文档"
       │
       ▼
┌─────────────────────────────────────┐
│ Contains "交叉评审" or "多角色"?     │
├─────────────────────────────────────┤
│         │                           │
│    YES  │   NO                      │
│         │                           │
│    ▼    │    ▼                      │
│ multi-  │  Contains specific        │
│ role-   │  expert role?             │
│ reviewer│    │                      │
│         │ YES│   NO                 │
│         │    ▼    ▼                 │
│         │ tech- default to          │
│         │ nical- multi-role         │
│         │ reviewer                  │
└─────────────────────────────────────┘
```

### Skill Combination Scenarios

| Scenario | Recommended Approach | Reason |
|----------|---------------------|--------|
| Quick single-domain review | technical-reviewer only | Faster, focused expertise |
| Comprehensive multi-domain review | multi-role-reviewer only | Multiple perspectives in one pass |
| Deep then broad review | technical-reviewer → multi-role-reviewer | Single expert deep dive, then cross-validation |
| Initial broad then focused | multi-role-reviewer → technical-reviewer | Identify issues broadly, then deep dive on specific areas |

**Combination Workflow:**

```
Scenario: Deep then Broad Review

Step 1: technical-reviewer (single expert)
        └── Deep analysis from one perspective
        └── Identify domain-specific issues
        └── Score: 85

Step 2: multi-role-reviewer (multiple experts)
        └── Cross-validate findings from Step 1
        └── Add perspectives from other domains
        └── Vote on all findings
        └── Score: 92

Step 3: Iterate until score ≥ 95
```

**When to Combine:**
- Critical documents requiring both depth and breadth
- Documents with both domain-specific and cross-cutting concerns
- When initial single-role review score is below threshold

## Review Process

### Visual Workflow

```
┌─────────────────────────────────────────────────────────────────────────┐
│                    Multi-Role Cross-Review Workflow                      │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  Phase 1: Role Selection & Initial Review                               │
│  ├── Select 2-5 expert roles based on document domain                   │
│  ├── Each role independently reviews the document                       │
│  └── Each role outputs: issues, suggestions, dimension scores           │
│                                                                         │
│  Phase 2: Cross-Review & Voting                                         │
│  ├── Roles review each other's findings                                 │
│  ├── Each role votes on other roles' issues/suggestions:                │
│  │   ├── ✅ Agree (issue is valid, suggestion is correct)               │
│  │   ├── ⚠️ Partially Agree (issue exists but suggestion needs adjust)  │
│  │   └── ❌ Disagree (issue is invalid or suggestion is wrong)          │
│  └── Calculate weight for each finding: Weight = Agree Votes / Total    │
│                                                                         │
│  Phase 3: Consolidation & Prioritization                                │
│  ├── Consolidate all findings with weights                              │
│  ├── High-weight findings (>0.7): Must address                          │
│  ├── Medium-weight findings (0.4-0.7): Should address                   │
│  └── Low-weight findings (<0.4): Optional                               │
│                                                                         │
│  Phase 4: Document Improvement                                          │
│  ├── Address findings in priority order                                 │
│  ├── Update document version                                            │
│  └── Document changes made                                              │
│                                                                         │
│  Phase 5: Re-Evaluation                                                 │
│  ├── Calculate new overall score                                        │
│  ├── If score ≥ 95: Review complete                                     │
│  └── If score < 95: Return to Phase 1 with updated document             │
│                                                                         │
│  Phase 6: Final Report                                                  │
│  ├── Generate consolidated audit report                                 │
│  ├── Include all role perspectives and voting results                   │
│  └── Track iteration history                                            │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### Text Description (Alternative)

**Phase 1 - Role Selection & Initial Review:**
1. Select 2-5 expert roles based on document domain
2. Each role independently reviews the document
3. Each role outputs: issues, suggestions, dimension scores

**Phase 2 - Cross-Review & Voting:**
1. Roles review each other's findings
2. Each role votes: Agree (✅), Partially Agree (⚠️), or Disagree (❌)
3. Calculate weight for each finding: Weight = Agree Votes / Total Votes

**Phase 3 - Consolidation & Prioritization:**
1. Consolidate all findings with calculated weights
2. High-weight findings (>0.7): Must address
3. Medium-weight findings (0.4-0.7): Should address
4. Low-weight findings (<0.4): Optional

**Phase 4 - Document Improvement:**
1. Address findings in priority order (high → medium → low)
2. Update document version
3. Document changes made

**Phase 5 - Re-Evaluation:**
1. Calculate new overall score
2. If score ≥ 95: Review complete, proceed to Phase 6
3. If score < 95: Return to Phase 1 with updated document

**Phase 6 - Final Report:**
1. Generate consolidated audit report
2. Include all role perspectives and voting results
3. Track iteration history

## Role Selection

### Automatic Role Detection

Based on document content and filename, select 2-5 relevant roles:

| Document Domain | Recommended Roles |
|-----------------|-------------------|
| Database Design | Database Architect, Performance Engineer, Security Expert |
| GIS/Spatial | GIS Core Developer, Database Architect, API Architect |
| API Design | API Architect, Security Expert, Frontend Architect |
| System Architecture | System Architect, Performance Engineer, Security Expert |
| Security | Security Expert, System Architect, Compliance Expert |
| **Skill/AI System** | **AI/LLM Expert, Prompt Engineer, System Integration Expert** |
| **Prompt Design** | **Prompt Engineer, AI/LLM Expert, UX Expert** |

### Specialized Review Roles

For skill documents and AI-related content, use these specialized roles:

| Role | Focus Areas | When to Use |
|------|-------------|-------------|
| **AI/LLM Expert** | AI trigger accuracy, description clarity, edge case handling, model understanding | Skill documents, prompt templates, AI system design |
| **Prompt Engineer** | Prompt effectiveness, instruction clarity, output format constraints, hallucination prevention | Skill documents, prompt design, AI behavior control |
| **System Integration Expert** | Skill collaboration, conflict resolution, resource management, error propagation | Multi-skill systems, integration scenarios |

### AI/LLM Expert Review Checklist

```
AI/LLM Expert evaluates:

1. Trigger Accuracy
   ├── Does description contain sufficient trigger keywords?
   ├── Can AI correctly distinguish this skill from similar skills?
   └── Are edge cases covered in trigger conditions?

2. Description Clarity
   ├── Is the description within 200 characters?
   ├── Does it clearly state what the skill does?
   └── Does it clearly state when to invoke?

3. Model Understanding
   ├── Are instructions unambiguous for AI?
   ├── Are there conflicting instructions?
   └── Is the expected output format clear?

4. Edge Case Handling
   ├── What happens if user input is ambiguous?
   ├── What happens if skill fails mid-execution?
   └── Are there fallback behaviors defined?
```

### Prompt Engineer Review Checklist

```
Prompt Engineer evaluates:

1. Prompt Structure
   ├── Is there a clear role definition?
   ├── Is there a clear task description?
   ├── Are there few-shot examples?
   └── Is there output format specification?

2. Instruction Effectiveness
   ├── Are instructions specific enough?
   ├── Are there priority/conflict resolution rules?
   ├── Are constraints clearly stated?
   └── Is the scope well-defined?

3. Output Control
   ├── Can AI reliably produce expected output format?
   ├── Are there format validation mechanisms?
   ├── Is there handling for unexpected outputs?
   └── Are there examples of correct output?

4. Hallucination Prevention
   ├── Are there explicit constraints on what NOT to do?
   ├── Are facts/claims verifiable?
   └── Is there guidance for unknown scenarios?
```

### System Integration Expert Review Checklist

```
System Integration Expert evaluates:

1. Skill Collaboration
   ├── Can this skill work with other skills?
   ├── Are there skill combination scenarios?
   └── Is there skill selection logic?

2. Conflict Resolution
   ├── What if multiple skills match user request?
   ├── Are there priority rules between skills?
   └── Is there a fallback skill hierarchy?

3. Resource Management
   ├── What is estimated execution time?
   ├── What is token/memory consumption?
   └── Are there timeout handling mechanisms?

4. Error Propagation
   ├── How does skill failure affect other skills?
   ├── Is there isolation between skills?
   └── Are there recovery mechanisms?
```

### User-Specified Roles

User can explicitly specify roles:

```
请以数据库架构师、性能工程师、安全专家三个角色交叉评审xxx文档
```

### Role Weight Adjustment

Roles that consistently provide correct findings get higher weight:

```
Role Weight = (Correct Findings) / (Total Findings Proposed)

Example:
- Database Architect: 8/10 findings accepted → Weight: 0.8
- Performance Engineer: 6/10 findings accepted → Weight: 0.6
- Security Expert: 9/10 findings accepted → Weight: 0.9
```

### Terminology Clarification

| Term | Definition | Usage |
|------|------------|-------|
| **Role Weight** | Weight assigned to each expert role based on their finding accuracy | Used in final score calculation |
| **Finding Weight** | Weight assigned to each finding based on voting results | Used in prioritization |

**Key Difference:**
- Role Weight reflects **expert credibility** (how often this expert is correct)
- Finding Weight reflects **finding consensus** (how many experts agree on this finding)

## Cross-Review Mechanism

### Voting Rules

Each role votes on other roles' findings:

| Vote | Meaning | Effect on Finding Weight |
|------|---------|--------------------------|
| ✅ Agree | Issue is valid, suggestion is correct | +1 to agree count |
| ⚠️ Partially | Issue valid but suggestion needs adjustment | +0.5 to agree count |
| ❌ Disagree | Issue is invalid or suggestion is wrong | +0 to agree count |

### Weight Calculation

```
Finding Weight = Σ(Votes) / (Number of Voting Roles)

Where:
- Agree vote = 1.0
- Partially Agree vote = 0.5
- Disagree vote = 0.0

Example:
Finding: "DbConnection interface is too large"
Votes: DB Architect(✅), Perf Engineer(✅), Security Expert(⚠️)
Weight = (1.0 + 1.0 + 0.5) / 3 = 0.83 → High Priority
```

### Dispute Resolution

When roles disagree significantly:

1. **Identify Dispute**: Finding weight between 0.3-0.7
2. **Request Clarification**: Ask disagreeing role to explain reasoning
3. **Re-Vote**: After clarification, roles can change votes
4. **Final Decision**: If still disputed, use majority vote or escalate to user

### Voting Matrix Visualization Example

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         Voting Matrix Example                                │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Finding: "Missing connection pool exhaustion handling"                     │
│  Proposed by: Database Architect                                            │
│                                                                             │
│  ┌─────────────────┬────────────────┬───────────────────────────────────┐   │
│  │ Role            │ Vote           │ Reasoning                         │   │
│  ├─────────────────┼────────────────┼───────────────────────────────────┤   │
│  │ DB Architect    │ (Proposer)     │ High impact on system stability   │   │
│  │ Perf Engineer   │ ✅ Agree       │ Can cause cascading failures      │   │
│  │ Security Expert │ ⚠️ Partial     │ Valid but priority lower than auth│   │
│  └─────────────────┴────────────────┴───────────────────────────────────┘   │
│                                                                             │
│  Weight: (1.0 + 0.5) / 2 = 0.75 → Medium Priority                           │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────┐
│                         Dispute Case Example                                 │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Finding: "Should use ORM instead of raw SQL"                               │
│  Proposed by: API Architect                                                 │
│                                                                             │
│  Initial Votes:                                                             │
│  ┌─────────────────┬────────────────┬───────────────────────────────────┐   │
│  │ Role            │ Vote           │ Reasoning                         │   │
│  ├─────────────────┼────────────────┼───────────────────────────────────┤   │
│  │ API Architect   │ (Proposer)     │ Better abstraction, easier to test│   │
│  │ DB Architect    │ ❌ Disagree    │ Performance critical, ORM overhead│   │
│  │ Perf Engineer   │ ❌ Disagree    │ Batch operations need raw SQL     │   │
│  └─────────────────┴────────────────┴───────────────────────────────────┘   │
│                                                                             │
│  Initial Weight: 0 / 2 = 0.0 → Disputed (Low Priority)                      │
│                                                                             │
│  Resolution Process:                                                        │
│  1. DB Architect explains: "This is a GIS system with spatial queries,      │
│     ORM doesn't support PostGIS functions efficiently"                      │
│  2. API Architect acknowledges: "Valid point, I withdraw this finding"      │
│  3. Finding marked as: ❌ Rejected (Consensus reached)                      │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Conflict Resolution Case Studies

**Case 1: Technical Disagreement**
```
Scenario: Performance vs. Security trade-off

Finding: "Add rate limiting to all API endpoints"
- Proposed by: Security Expert
- DB Architect: ✅ Agree (prevents DoS)
- Perf Engineer: ⚠️ Partial (agrees on need, disagrees on implementation)

Conflict: Perf Engineer wants token bucket, Security Expert wants fixed window

Resolution:
1. Both explain reasoning
2. Consensus: Use sliding window (middle ground)
3. Weight updated: 0.75 → 0.83 (after agreement)
```

**Case 2: Priority Disagreement**
```
Scenario: Different priority assessments

Finding: "Add comprehensive logging"
- Proposed by: DevOps Engineer
- Security Expert: ✅ Agree (audit trail)
- Perf Engineer: ❌ Disagree (performance impact)

Conflict: Is this finding worth the performance cost?

Resolution:
1. Escalate to user with options:
   A. Accept with performance impact
   B. Accept with sampling (log 10% of requests)
   C. Reject due to performance concerns
2. User chooses option B
3. Finding modified and accepted
```

## Scoring System

### Dimension Scoring

Each role independently scores 6 dimensions:

| Dimension | Weight | Description |
|-----------|--------|-------------|
| Architecture Rationality | 20% | Design patterns, layering, coupling |
| Extensibility | 15% | Extension points, plugin support |
| Performance Assurance | 20% | Hot paths, optimization, benchmarks |
| Ecosystem Compatibility | 15% | Library compatibility, standards |
| Implementation Feasibility | 15% | Workload, risk, team capability |
| Documentation Completeness | 15% | Examples, tests, API docs |

### Final Score Calculation

```
Final Score = Σ(Role Weight × Role's Dimension Score) / Σ(Role Weights)

Example:
- DB Architect (Weight 0.8): Score 88
- Perf Engineer (Weight 0.6): Score 82
- Security Expert (Weight 0.9): Score 90

Final Score = (0.8×88 + 0.6×82 + 0.9×90) / (0.8 + 0.6 + 0.9)
            = (70.4 + 49.2 + 81) / 2.3
            = 87.2
```

### Score Validation Rules

Before outputting scores, validate for consistency:

| Validation Rule | Check | Action if Failed |
|-----------------|-------|------------------|
| **Range Check** | Each dimension score must be 0-100 | Clamp to valid range |
| **Weight Sum** | Dimension weights must sum to 100% | Normalize weights |
| **Role Weight Validity** | Role weights must be > 0 | Set minimum 0.1 |
| **Score Progression** | Each iteration should improve or explain why not | Document reason if no improvement |
| **Cross-Role Consistency** | Similar findings should have similar scores | Flag for review if variance > 20% |

### Score Consistency Check

```
For each finding, check score consistency across roles:

Finding: "Missing error handling"
- Role A scores: 75 (Architecture), 70 (Implementation)
- Role B scores: 80 (Architecture), 72 (Implementation)
- Variance: |80-75|/75 = 6.7% → Acceptable

If variance > 20%:
- Flag for cross-review discussion
- Roles must explain their scoring rationale
- Reach consensus before final score
```

## Output Structure

### Audit Report Path

```
<document_directory>/<document_name>_multi_audit.md
```

### Report Content

```markdown
# [Document Name] Multi-Role Cross-Review Report

**Document**: [filename]  
**Version**: [version]  
**Date**: [date]  
**Participating Roles**: [list with weights]

---

### 📊 评审结果摘要

| 项目 | 值 |
|------|-----|
| **目标评分** | [target_score]/100 |
| **最终评分** | [final_score]/100 |
| **评审状态** | ✅ 通过 / ⏳ 进行中 / ❌ 未通过 |
| **迭代次数** | [iterations] |

---

## 一、Review Overview

### 1.1 Role Selection
| Role | Initial Weight | Focus Areas |

### 1.2 Review Methodology
[Cross-review process description]

---

## 二、Role-Specific Reviews

### 2.1 [Role 1] Review
**Dimension Scores**: [table]
**Issues Found**: [list]
**Suggestions**: [list]

### 2.2 [Role 2] Review
[Same format]

---

## 三、Cross-Review & Voting Results

### 3.1 Issue Voting Matrix
| Issue | Proposed By | Role 1 | Role 2 | Role 3 | Weight | Priority |

### 3.2 Suggestion Voting Matrix
| Suggestion | Proposed By | Role 1 | Role 2 | Role 3 | Weight | Status |

### 3.3 Disputed Items
| Item | Dispute Summary | Resolution |

---

## 四、Consolidated Findings

### 4.1 High Priority (Weight ≥ 0.7)
| ID | Finding | Proposed By | Weight | Status | Resolution Version |

### 4.2 Medium Priority (0.4 ≤ Weight < 0.7)
| ID | Finding | Proposed By | Weight | Status | Resolution Version |

### 4.3 Low Priority (Weight < 0.4)
| ID | Finding | Proposed By | Weight | Status | Resolution Version |

---

## 五、Finding Details

### 5.1 Finding Status Definitions

| 状态 | 标识 | 说明 |
|------|------|------|
| ✅ 已解决 | RESOLVED | 问题已修复，已验证通过 |
| ⏳ 进行中 | IN_PROGRESS | 问题正在处理中 |
| 📋 待处理 | PENDING | 问题已确认，等待处理 |
| ⏭️ 后续版本 | DEFERRED | 问题已确认，推迟到后续版本处理 |
| ❌ 不采纳 | REJECTED | 问题无效或建议不适用 |

### 5.2 Resolved Findings Detail

For each resolved finding, provide:

```markdown
#### [ID]: [Finding Title]

**状态**: ✅ 已解决  
**解决版本**: [version]  
**提出者**: [role]  
**权重**: [weight]

**问题描述**:
[Detailed description of the issue]

**影响范围**:
- [Impact area 1]
- [Impact area 2]

**修改方法**:
[Detailed description of how the issue was fixed]

**验证方式**:
[How the fix was verified]

**相关代码/章节**:
- [Link to modified section]
```

---

## 六、Document Changes

### 6.1 Version History
| Version | Changes | Addressed Findings |

### 6.2 Finding Resolution Status
| ID | Finding | Status | Resolution Version |

---

## 七、Score Progression

### 7.1 Score by Iteration
| Iteration | Role 1 | Role 2 | Role 3 | Final Score |

### 7.2 Score Visualization
[ASCII art showing score progression]

---

## 八、Role Weight Evolution

| Role | Initial Weight | Final Weight | Correct Findings |

---

## 九、Final Conclusion

**Final Score**: [score]/100
**Target Score**: [target_score]/100
**Status**: ✅ Pass / ❌ Continue Review
**Key Improvements**: [list]
**Remaining Items**: [list or "None"]

---

## 十、Review Signature

**Review Type**: Multi-Role Cross-Review
**Participating Roles**: [list]
**Total Iterations**: [number]
**Final Status**: [status]
```

## Best Practices

### 1. Role Immersion
Each role must fully adopt their expert persona:
- Use domain-specific terminology
- Apply domain-specific evaluation criteria
- Reference domain-specific best practices

### 2. Structured Output
Use consistent formats for:
- Issue descriptions (Problem → Impact → Suggestion)
- Voting (✅/⚠️/❌ with reasoning)
- Score tables

### 3. Version Tracking
Maintain clear version history:
- Document version increments each improvement cycle
- Track which findings were addressed in each version

### 4. Issue Closure
Ensure every finding has:
- Weight calculated from votes
- Priority assigned based on weight
- Resolution status tracked

### 5. Document Sync
Keep all outputs synchronized:
- Design document updated with fixes
- Audit report reflects current state
- Role weights updated based on accuracy

### 6. Competitive Correctness
Roles that provide correct findings get more influence:
- Track finding acceptance rate per role
- Adjust role weights accordingly
- Roles with higher accuracy get more voting power

## Example Usage

### Example 1: Auto-Select Roles

```
> 请对database_model_design.md进行多角色交叉评审，直到评分>95
```

Expected behavior:
1. Auto-select: Database Architect, Performance Engineer, Security Expert
2. Each role reviews independently
3. Cross-review with voting
4. Consolidate findings by weight
5. Improve document
6. Iterate until score ≥ 95

### Example 2: Specify Roles

```
> 请以数据库架构师、性能工程师、安全专家三个角色交叉评审database_model_design.md，
> 谁说的正确谁多评审，直到评分>95
```

Expected behavior:
1. Use specified 3 roles
2. Each role reviews and votes
3. Correct findings get higher weight
4. Roles with more correct findings get more influence
5. Iterate until score ≥ 95

### Example 3: With Role Backgrounds

```
> 请以下列角色交叉评审service_design.md：
> 1. 微服务架构师，拥有10年分布式系统经验
> 2. 性能工程师，专注于高并发系统优化
> 3. 安全专家，专注于API安全和认证
> 交叉评审，直到评分>95
```

### Example 4: Skill Document Review (Using Specialized Roles)

```
> 请对multi-role-reviewer技能文档进行交叉评审

Expected behavior:
1. Auto-detect: Skill/AI System domain
2. Select roles: AI/LLM Expert, Prompt Engineer, System Integration Expert
3. AI/LLM Expert evaluates: trigger accuracy, description clarity
4. Prompt Engineer evaluates: prompt structure, output control
5. System Integration Expert evaluates: skill collaboration, conflict resolution
6. Cross-review voting on findings
7. Iterate until score ≥ 95
```

### Example 5: Handling Disagreement

```
> 请交叉评审api_design.md，重点关注安全性

Scenario during review:
- Security Expert: "All endpoints need rate limiting" (High priority)
- Performance Engineer: "Rate limiting adds latency" (Disagrees)
- API Architect: "Use selective rate limiting" (Partial agreement)

Resolution:
1. Weight calculation: (1.0 + 0 + 0.5) / 2 = 0.75 (Medium priority)
2. Roles discuss: Security Expert accepts selective approach
3. Updated finding: "Critical endpoints need rate limiting"
4. New weight: (1.0 + 1.0 + 1.0) / 2 = 1.0 (High priority)
```

### Example 6: Iteration with Score Progression

```
Initial Review:
- Score: 78/100
- High-weight findings: 3
- Medium-weight findings: 5

Iteration 1:
- Address all high-weight findings
- Score: 85/100
- Remaining: 2 medium-weight findings

Iteration 2:
- Address remaining findings
- Score: 92/100
- New findings from re-review: 1 low-weight

Iteration 3:
- Address low-weight finding
- Score: 96/100
- ✅ Review complete
```

## Iteration Rules

1. **Score Threshold**: Continue until final score ≥ 95
2. **Version Increment**: Each improvement cycle increments version
3. **Weight Update**: Role weights updated after each iteration
4. **Finding Closure**: All high-weight findings must be addressed
5. **Maximum Iterations**: Stop after 5 iterations if score still < 95
6. **Early Termination**: User can terminate early if satisfied

## Negative Constraints (What NOT to Do)

To ensure focused and effective reviews, the skill must NOT:

### ❌ Scope Violations

| Constraint | Reason |
|------------|--------|
| Do NOT review code implementation files | This skill is for design documents, not source code |
| Do NOT review binary or non-text files | Cannot parse non-text content |
| Do NOT review documents without user request | Must be explicitly invoked |
| Do NOT modify documents without user approval | Changes require user confirmation |

### ❌ Behavior Violations

| Constraint | Reason |
|------------|--------|
| Do NOT skip the voting phase | Cross-review voting is core to this skill |
| Do NOT ignore low-weight findings entirely | Must document all findings, even if not addressed |
| Do NOT proceed to next iteration without addressing high-weight findings | High-weight findings must be resolved |
| Do NOT invent roles not in the defined list | Use only defined or user-specified roles |

### ❌ Output Violations

| Constraint | Reason |
|------------|--------|
| Do NOT output scores without dimension breakdown | All scores must show dimension details |
| Do NOT skip the audit report generation | Audit report is mandatory output |
| Do NOT omit version tracking | All iterations must be tracked |
| Do NOT produce unstructured output | Follow the defined report template |

### ❌ Interaction Violations

| Constraint | Reason |
|------------|--------|
| Do NOT ask user for every decision | Use voting mechanism for role disagreements |
| Do NOT terminate without user consent | Must offer options when stuck |
| Do NOT override user-specified roles | User role choices take priority |
| Do NOT exceed maximum iterations silently | Must report when iteration limit reached |

## Error Handling & Fallback

### Skill Invocation Failure

When the skill fails to execute properly:

| Failure Type | Fallback Strategy |
|--------------|-------------------|
| Role selection fails | Use default roles: System Architect, Performance Engineer |
| Voting timeout | Use majority vote without waiting |
| Document read error | Report error to user, suggest manual fix |
| Score calculation error | Use average of available role scores |

### Iteration Failure Handling

When maximum iterations (5) reached without achieving score ≥ 95:

```
┌─────────────────────────────────────────────────────────────┐
│              Iteration Failure Recovery                      │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Step 1: Analyze Blockers                                   │
│  ├── Identify findings that couldn't be resolved            │
│  ├── Categorize: Technical vs. Resource constraints         │
│  └── Check if findings are disputed (weight 0.4-0.6)        │
│                                                             │
│  Step 2: Generate Report                                    │
│  ├── List unresolved findings with weights                  │
│  ├── Explain why they couldn't be addressed                 │
│  └── Provide recommendations for future resolution          │
│                                                             │
│  Step 3: User Decision                                      │
│  ├── Option A: Accept current score (document is "good enough") │
│  ├── Option B: Manual intervention to resolve blockers      │
│  ├── Option C: Reduce scope (focus on high-weight issues only) │
│  └── Option D: Request additional expert roles              │
│                                                             │
│  Step 4: Continue or Exit                                   │
│  ├── If user accepts: Generate final report                 │
│  └── If user intervenes: Resume from Phase 4                │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Failure Report Template

```markdown
## Iteration Limit Reached

**Current Score**: [score]/100
**Iterations Completed**: 5/5
**Target Score**: 95/100

### Unresolved Findings

| ID | Finding | Weight | Blocker Reason |
|----|---------|--------|----------------|

### Recommendations

1. [Specific recommendation]
2. [Specific recommendation]

### User Options

Reply with:
- "接受" to accept current score
- "干预" to manually resolve blockers
- "缩小范围" to focus on high-weight issues only
- "添加角色:[角色名]" to add more expert roles
```

## Progress Feedback

### Progress Indicators

During the review process, provide progress feedback:

```
┌─────────────────────────────────────────────────────────────┐
│                    Review Progress                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Phase 1: Role Selection ████████████████████ 100%          │
│  Selected: DB Architect, Perf Engineer, Security Expert     │
│                                                             │
│  Phase 2: Independent Review ████████████░░░░ 75%           │
│  ✓ DB Architect: 8 issues found                             │
│  ✓ Perf Engineer: 6 issues found                            │
│  ⏳ Security Expert: Reviewing...                            │
│                                                             │
│  Estimated time remaining: ~3 minutes                       │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Time Estimation

| Phase | Estimated Time |
|-------|----------------|
| Phase 1: Role Selection | 10-30 seconds |
| Phase 2: Independent Review | 1-3 minutes per role |
| Phase 3: Cross-Review & Voting | 30-60 seconds |
| Phase 4: Document Improvement | 1-5 minutes |
| Phase 5: Re-Evaluation | 30-60 seconds |
| **Total per iteration** | **3-10 minutes** |

### Token Estimation

| Component | Token Range | Notes |
|-----------|-------------|-------|
| Skill loading | 2,000-3,000 | One-time per invocation |
| Role persona | 500-800 per role | Context for each expert |
| Document reading | 100-500 per page | Depends on document length |
| Independent review | 1,000-2,000 per role | Finding generation |
| Cross-review voting | 500-1,000 per finding | Voting and weight calculation |
| Document improvement | 1,000-3,000 | Depends on changes needed |
| Audit report | 1,500-2,500 | Final report generation |

**Total Estimation Formula:**

```
Total Tokens ≈ 3,000 + (Roles × 1,500) + (Document Pages × 300) + (Findings × 800) + (Iterations × 5,000)

Example (3 roles, 10-page doc, 15 findings, 2 iterations):
= 3,000 + (3 × 1,500) + (10 × 300) + (15 × 800) + (2 × 5,000)
= 3,000 + 4,500 + 3,000 + 12,000 + 10,000
= 32,500 tokens (approximate)
```

**Token Optimization Tips:**
- Use fewer roles for simpler documents
- Limit iterations for large documents
- Focus on high-weight findings to reduce finding count

### Progress Messages

Display these messages during execution:
- "正在选择专家角色..."
- "角色A正在独立评审..."
- "角色B正在评审角色A的发现..."
- "正在计算投票权重..."
- "正在整合发现并排序..."
- "正在改进文档..."
- "正在重新评估评分..."

## Comparison Summary

| Feature | technical-reviewer | multi-role-reviewer |
|---------|-------------------|---------------------|
| Expert Roles | 1 (single) | 2-5 (multiple) |
| Review Style | Sequential | Parallel + Competitive |
| Decision Making | Expert decides | Weighted voting |
| Correctness | Expert judgment | Consensus building |
| Best For | Deep domain review | Comprehensive review |
| Speed | Faster (single pass) | Slower (multiple perspectives) |
| Coverage | Domain-specific | Multi-domain |

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0.0 | 2026-03-18 | Initial release with core cross-review functionality |
| 1.1.0 | 2026-03-18 | Added: Quick Start guide, business scenario examples, error handling, progress feedback, iteration failure recovery |
| 1.2.0 | 2026-03-18 | Added: Voting matrix visualization, conflict resolution case studies, text description alternative |
| 1.3.0 | 2026-03-18 | Added: AI/LLM Expert, Prompt Engineer, System Integration Expert roles with detailed checklists for skill/AI system review |
| 1.4.0 | 2026-03-18 | Added: Negative Constraints section, Skill Selection Priority rules, Terminology Clarification (Role Weight vs Finding Weight), Score Validation Rules, Token Estimation, Trigger Examples, Skill Combination Scenarios, Extended Few-shot examples |
| 1.4.1 | 2026-03-18 | Added: Voting Process Example in Quick Start for better understanding of cross-review mechanism |
| 1.5.0 | 2026-03-18 | Added: Target score and final score in report header, Finding status tracking (RESOLVED/IN_PROGRESS/PENDING/DEFERRED/REJECTED), Resolved findings detail template with problem description, impact scope, fix method, and verification |

### Versioning Policy

This skill follows semantic versioning (MAJOR.MINOR.PATCH):

- **MAJOR**: Breaking changes to workflow or output format
- **MINOR**: New features, enhanced functionality
- **PATCH**: Bug fixes, minor improvements

### Future Roadmap

| Version | Planned Features |
|---------|-----------------|
| 1.6.0 | Support for custom voting rules |
| 1.7.0 | Integration with external review tools |
| 2.0.0 | Machine learning-based role selection |
