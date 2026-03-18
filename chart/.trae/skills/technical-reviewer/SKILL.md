---
name: "technical-reviewer"
description: "Technical design document/skill reviewer with expert role-play. Invoke for single-expert review/audit of design documents until score > 95."
---

# Technical Design Document Reviewer

A comprehensive skill for reviewing technical design documents with expert role-play, structured output, and iterative improvement until quality threshold is met.

## Quick Start (5 Minutes)

**Basic Usage:**
```
> 请评审xxx.md，直到评分>95
```

**Specify Expert Role:**
```
> 请以数据库架构师的身份评审database_design.md
```

**That's it!** The system will:
1. Auto-detect expert role based on document type
2. Review document from expert perspective
3. Create audit report with scores
4. If score < 95, improve document iteratively
5. Stop when score ≥ 95

**Example:**
```
场景：评审一个数据库设计文档

用户输入：
> 请评审database_model_design.md，重点关注架构合理性、扩展性

系统行为：
1. 自动检测：Database Architect
2. 评审6个维度：架构合理性、扩展性、性能保证、生态兼容性、落地可行性、文档完整性
3. 发现问题：连接池缺少耗尽处理、批量插入缺少错误恢复
4. 创建评审报告：database_model_design_audit.md
5. 评分：82分，需要改进
6. 改进文档，重新评审
7. 迭代直到评分>95
```

## When to Invoke

Invoke this skill when:
- User asks to review/audit a design document
- User wants to evaluate architecture rationality, extensibility, performance
- User requests document improvement with scoring
- User mentions "评审" (review), "审计" (audit), "打分" (scoring)
- User wants iterative improvement until quality threshold

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

**When to Combine:**
- Critical documents requiring both depth and breadth
- Documents with both domain-specific and cross-cutting concerns
- When initial single-role review score is below threshold

## Comparison Summary

### technical-reviewer vs multi-role-reviewer

| Aspect | technical-reviewer | multi-role-reviewer |
|--------|-------------------|---------------------|
| **Expert Roles** | Single expert (auto-detect or user-specified) | Multiple experts (2-5 roles) |
| **Review Depth** | Deep dive from one perspective | Broad coverage from multiple perspectives |
| **Issue Validation** | Single expert judgment | Cross-voting with weight calculation |
| **Speed** | Faster (2-5 minutes) | Slower (5-15 minutes) |
| **Token Cost** | Lower (~15,000 tokens) | Higher (~30,000+ tokens) |
| **Best For** | Domain-specific documents, quick reviews | Cross-cutting concerns, critical documents |
| **Conflict Resolution** | Expert decides | Voting mechanism |

### When to Use Which

| Document Type | Recommended Skill | Reason |
|---------------|-------------------|--------|
| Single-domain design (database, API, UI) | technical-reviewer | Deep domain expertise |
| Multi-domain design (system architecture) | multi-role-reviewer | Multiple perspectives needed |
| Security-focused documents | Either (specify Security Expert) | Both can handle with role specification |
| Quick sanity check | technical-reviewer | Faster turnaround |
| Critical production documents | multi-role-reviewer | Thorough validation |
| Skill documents | multi-role-reviewer | Need AI/LLM, Prompt, UX perspectives |

### Decision Matrix

```
                    ┌─────────────────────────────────────┐
                    │        Document Complexity          │
                    │    Low          Medium        High  │
┌───────────────────┼─────────────────────────────────────┤
│ Importance  Low   │  tech         tech          multi   │
│             Medium│  tech         tech          multi   │
│             High  │  tech         multi         multi   │
└───────────────────┴─────────────────────────────────────┘

Legend:
- tech = technical-reviewer
- multi = multi-role-reviewer
```

## Review Process

```
┌─────────────────────────────────────────────────────────────┐
│                 Technical Review Workflow                    │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Step 1: Read Document                                      │
│  ├── Read the target design document                        │
│  └── Understand architecture, interfaces, implementation    │
│                                                             │
│  Step 2: Expert Role-Play Review                            │
│  ├── Define expert role based on document domain            │
│  ├── Evaluate 6 dimensions:                                 │
│  │   ├── Architecture Rationality (20%)                     │
│  │   ├── Extensibility (15%)                                │
│  │   ├── Performance Assurance (20%)                        │
│  │   ├── Ecosystem Compatibility (15%)                      │
│  │   ├── Implementation Feasibility (15%)                   │
│  │   └── Documentation Completeness (15%)                   │
│  └── Identify issues by risk level (High/Medium/Low)        │
│                                                             │
│  Step 3: Create Audit Report                                │
│  ├── Save to <document_name>_audit.md                       │
│  ├── Include: strengths, risks, suggestions                 │
│  └── Provide dimension scores and overall score             │
│                                                             │
│  Step 4: Check Score Threshold                              │
│  ├── If score >= 95: Review complete                        │
│  └── If score < 95: Continue to Step 5                      │
│                                                             │
│  Step 5: Improve Document                                   │
│  ├── Update document version                                │
│  ├── Address all identified issues                          │
│  └── Return to Step 2 for re-review                         │
│                                                             │
│  Step 6: Update Audit Report                                │
│  ├── Add new version review section                         │
│  ├── Update suggestion status                               │
│  └── Compare before/after scores                            │
│                                                             │
│  Step 7: Iterate until score >= 95                          │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

## Expert Role Definition

### Role Selection Modes

The skill supports **three modes** for expert role selection:

| Mode | How It Works | When to Use |
|------|--------------|-------------|
| **Auto-Detect** | Automatically match role based on document content/filename | Standard reviews, quick assessments |
| **User-Specified** | User explicitly defines expert role in request | Domain-specific deep reviews |
| **Hybrid** | Auto-detect with user override capability | Most flexible, recommended default |

### Mode 1: Auto-Detect (Default)

When user does not specify expert role, the skill automatically detects based on:

**Detection Rules (Priority Order):**
1. **Filename keywords** → Match to role
2. **Document content keywords** → Match to role
3. **Default** → General Technical Architect

| Keyword Pattern | Detected Role | Focus Areas |
|-----------------|---------------|-------------|
| `database`, `db_`, `storage`, `sql` | Database Architect | Storage engine, indexing, MVCC, throughput |
| `layer`, `model`, `geometry`, `spatial` | GIS Core Developer | OGC standards, spatial operations, memory |
| `api`, `interface`, `sdk` | API Architect | Interface design, versioning, compatibility |
| `performance`, `benchmark`, `optimize` | Performance Engineer | Hot paths, optimization, benchmarking |
| `security`, `auth`, `encrypt` | Security Expert | Authentication, encryption, vulnerability |
| `network`, `protocol`, `rpc` | Network Architect | Protocol design, latency, reliability |
| `ui`, `ux`, `frontend` | Frontend Architect | Component design, state management, UX |
| `test`, `qa`, `quality` | QA Architect | Test strategy, coverage, automation |
| `skill`, `prompt`, `agent`, `.trae/`, `trae skill` | AI/LLM Skill Reviewer | Skill trigger, prompt quality, AI behavior |

**Skill Document Detection**:
When detecting skill-related documents (`.trae/skills/`, `SKILL.md`, or containing `skill` keywords):
- Use AI/LLM Expert checklist for evaluation
- Focus on: trigger accuracy, description clarity, edge case handling
- Consider delegating to `multi-role-reviewer` for broader perspective

### Mode 2: User-Specified

User can explicitly specify expert role in the request:

**Format 1: Direct Specification**
```
请以[专家角色]的身份评审xxx文档
```

**Format 2: Role + Background**
```
作为[领域]的[专家角色]，拥有[经验描述]，请评审xxx文档
```

**Examples:**
```
请以PostgreSQL内核开发者的身份评审database_design.md

作为微服务架构师，拥有10年分布式系统设计经验，请评审service_design.md

以React核心团队成员的视角评审frontend_architecture.md
```

### Mode 3: Hybrid (Recommended)

Auto-detect with user confirmation at review start:

```
检测到文档类型: Database Design
自动匹配专家角色: Database Architect
如需更换角色，请回复角色名称；否则回复"确认"继续评审
```

### Impact of Role Selection

| Scenario | Wrong Role Impact | Correct Role Benefit |
|----------|-------------------|----------------------|
| Database doc reviewed by Frontend expert | Misses MVCC, indexing issues | Deep storage engine insights |
| Security doc reviewed by General architect | Misses vulnerability patterns | Identifies attack vectors |
| API doc reviewed by Database expert | May over-index on data concerns | Focuses on interface contracts |

**Recommendation:** For critical documents, use **User-Specified mode** with detailed expert background.

## Risk Level Classification

| Level | Symbol | Definition | Action Required |
|-------|--------|------------|-----------------|
| High | 🔴 | Affects stability/data consistency | Must fix in current version |
| Medium | 🟠 | Affects performance/user experience | Should fix in current version |
| Low | 🟡 | Affects development efficiency | Can fix in future versions |

## Scoring Criteria

| Score Range | Grade | Description |
|-------------|-------|-------------|
| 95-100 | A+ (Excellent) | Ready for production |
| 90-94 | A (Excellent) | Minor issues, ready for production |
| 85-89 | B+ (Good) | Reasonable design, needs minor fixes |
| 80-84 | B (Good) | Basic design reasonable, needs fixes |
| 70-79 | C (Pass) | Obvious issues, needs major fixes |
| <70 | D (Fail) | Serious issues, needs redesign |

### Score Validation Rules

Before outputting scores, validate for consistency:

| Validation Rule | Check | Action if Failed |
|-----------------|-------|------------------|
| **Range Check** | Each dimension score must be 0-100 | Clamp to valid range |
| **Weight Sum** | Dimension weights must sum to 100% | Normalize weights |
| **Issue-Score Alignment** | High issues should lower score significantly | Verify score reflects issue severity |
| **Score Progression** | Each iteration should improve or explain why not | Document reason if no improvement |
| **Dimension Balance** | No single dimension should dominate (>40% weight) | Redistribute weights |

### Score Calculation Formula

```
Overall Score = Σ(Dimension Score × Dimension Weight)

Where:
- Architecture Rationality: 20%
- Extensibility: 15%
- Performance Assurance: 20%
- Ecosystem Compatibility: 15%
- Implementation Feasibility: 15%
- Documentation Completeness: 15%

Example:
- Architecture: 85 × 0.20 = 17.0
- Extensibility: 90 × 0.15 = 13.5
- Performance: 80 × 0.20 = 16.0
- Ecosystem: 88 × 0.15 = 13.2
- Implementation: 82 × 0.15 = 12.3
- Documentation: 75 × 0.15 = 11.25
- Total: 83.25 → Round to 83
```

## Audit Report Template

```markdown
# [Document Name] Technical Review Report

**Document**: [filename]
**Version**: [version]
**Date**: [date]
**Reviewer Role**: [expert role]
**Focus Areas**: [list]
**Target Score**: > 95
**Final Score**: ✅/❌ [score]/100 (达标/未达标)

---

## 一、Review Overview
[Brief summary of review approach]

---

## 二、Strengths Analysis

### 2.1 Architecture Strengths
| Strength | Description | Value |

### 2.2 Performance Strengths
| Strength | Description | Value |

### 2.3 Engineering Strengths
| Strength | Description | Value |

---

## 三、Risks and Issues

### 3.1 🔴 High Risk Issues
#### Issue 1: [Title]
**Description**: [details]
**Impact**: [consequences]
**Suggestion**: [solution]
**Status**: 📋 待解决 / 🔄 进行中 / ✅ 已解决(vX.X)

### 3.2 🟠 Medium Risk Issues
[Same format]

### 3.3 🟡 Low Risk Issues
[Same format]

---

## 四、Extensibility Analysis
[Table of supported and pending extension scenarios]

---

## 五、Ecosystem Compatibility
[Compatibility assessment with existing libraries]

---

## 六、Implementation Feasibility
[Technical feasibility, workload estimation, risk assessment]

---

## 七、Improvement Suggestions Summary

### 7.1 High Priority (Must Implement)
| ID | Suggestion | Related Issue | Effort | Status |

### 7.2 Medium Priority (Should Implement)
| ID | Suggestion | Related Issue | Effort | Status |

### 7.3 Low Priority (Optional)
| ID | Suggestion | Related Issue | Effort | Status |

**Status Legend**:
- 📋 待解决: Issue identified, pending resolution
- 🔄 进行中: Currently being addressed
- ✅ 已解决(vX.X): Resolved in version X.X

---

## 八、Scoring

### 8.1 Dimension Scores
| Dimension | Score | Notes |

### 8.2 Overall Score
[ASCII art score visualization]

### 8.3 Review Conclusion
**Result**: ✅/❌ Pass/Fail
**Strengths**: [list]
**Pending Items**: [list]

---

## 九、Review Signature
**Reviewer**: [name/role]
**Date**: [date]
**Status**: [status]
**Next Step**: [action]

---

## 十、Revision Tracking

### 10.1 Revision History
| Version | Date | Changes | Score |

### 10.2 Suggestion Status Tracking
| ID | Suggestion | Priority | Status | Resolved Version |

**Statistics**:
- Total suggestions: N
- Completed: N (N%)
- Pending: N (N%)
```

## Iteration Rules

1. **Score Threshold**: Continue iterating until overall score >= 95
2. **Version Increment**: Each iteration increments minor version (e.g., v1.1 → v1.2)
3. **Issue Closure**: All High/Medium issues must be resolved
4. **Documentation Sync**: Update both design doc and audit report each iteration
5. **Maximum Iterations**: Stop after 5 iterations if score still < 95, report blockers

## Status Tracking Rules

### Issue Status Lifecycle

```
📋 待解决 ──► 🔄 进行中 ──► ✅ 已解决(vX.X)
   │              │              │
   │              │              └── Document updated, issue addressed
   │              │
   │              └── Working on the issue
   │
   └── Issue identified in review
```

### Status Update Triggers

| Trigger | Action | Status Change |
|---------|--------|---------------|
| Issue identified | Add to report with 📋 待解决 | New → Pending |
| Start addressing | Update status to 🔄 进行中 | Pending → In Progress |
| Document updated | Update status to ✅ 已解决(vX.X) | In Progress → Resolved |
| Re-review finds issue | Revert to 📋 待解决 | Resolved → Pending |

### Document Modification Flow

```
┌─────────────────────────────────────────────────────────────────────────┐
│                    Document Modification & Status Update                 │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  Step 1: Identify Issues                                                │
│  ├── Review document and identify all issues                            │
│  ├── Add issues to audit report with status: 📋 待解决                   │
│  └── Provide suggestions for each issue                                 │
│                                                                         │
│  Step 2: Modify Target Document                                         │
│  ├── Update target document version (e.g., v1.5 → v1.6)                 │
│  ├── Address issues based on priority                                   │
│  └── Add change notes in document                                       │
│                                                                         │
│  Step 3: Update Audit Report Status                                     │
│  ├── For each addressed issue:                                          │
│  │   └── Change status: 📋 待解决 → ✅ 已解决(vX.X)                       │
│  ├── Update suggestion status in Section 7                              │
│  ├── Update statistics in Section 10.2                                  │
│  └── Add revision entry in Section 10.1                                 │
│                                                                         │
│  Step 4: Re-Review                                                      │
│  ├── Re-evaluate document with resolved issues                          │
│  ├── Calculate new score                                                │
│  └── Update Final Score in header                                       │
│                                                                         │
│  Step 5: Iterate until score >= 95                                      │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### Status Update Example

**Before Document Modification (v1.5):**
```markdown
| ID | Issue | Severity | Status |
|----|-------|----------|--------|
| D1 | Connection pool lacks leak detection | 🟠 Medium | 📋 待解决 |
| S1 | Missing field-level access control | 🟠 Medium | 📋 待解决 |
| A2 | No API deprecation policy | 🟠 Medium | 📋 待解决 |
```

**After Document Modification (v1.6):**
```markdown
| ID | Issue | Severity | Status |
|----|-------|----------|--------|
| D1 | Connection pool lacks leak detection | 🟠 Medium | ✅ 已解决(v1.6) |
| S1 | Missing field-level access control | 🟠 Medium | ✅ 已解决(v1.6) |
| A2 | No API deprecation policy | 🟠 Medium | ✅ 已解决(v1.6) |
```

### Header Score Update

**Before:**
```markdown
**Target Score**: > 95
**Final Score**: ❌ 92/100 (未达标)
```

**After:**
```markdown
**Target Score**: > 95
**Final Score**: ✅ 96/100 (达标)
```

## Progress Feedback

### Progress Indicators

During the review process, provide progress feedback:

```
┌─────────────────────────────────────────────────────────────┐
│                    Review Progress                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Step 1: Read Document ████████████████████ 100%            │
│  Document: database_model_design.md (15 pages)              │
│                                                             │
│  Step 2: Expert Review ████████████░░░░ 75%                 │
│  ✓ Architecture Rationality: 85                             │
│  ✓ Extensibility: 90                                        │
│  ⏳ Performance Assurance: Reviewing...                      │
│                                                             │
│  Estimated time remaining: ~2 minutes                       │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Time Estimation

| Step | Estimated Time |
|------|----------------|
| Step 1: Read Document | 10-30 seconds |
| Step 2: Expert Review | 1-3 minutes |
| Step 3: Create Audit Report | 30-60 seconds |
| Step 4-6: Iteration | 1-5 minutes per iteration |
| **Total (no iteration)** | **2-5 minutes** |
| **Total (with iterations)** | **5-20 minutes** |

### Token Estimation

| Component | Token Range | Notes |
|-----------|-------------|-------|
| Skill loading | 1,500-2,500 | One-time per invocation |
| Document reading | 100-500 per page | Depends on document length |
| Expert review | 1,500-3,000 | Dimension scoring and issue identification |
| Audit report | 1,000-2,000 | Report generation |
| Document improvement | 1,000-3,000 | Depends on changes needed |

**Total Estimation Formula:**

```
Total Tokens ≈ 2,000 + (Document Pages × 300) + (Iterations × 5,000)

Example (10-page doc, 2 iterations):
= 2,000 + (10 × 300) + (2 × 5,000)
= 2,000 + 3,000 + 10,000
= 15,000 tokens (approximate)
```

**Token Optimization Tips:**
- Focus on High/Medium issues first
- Limit iterations for large documents
- Use user-specified expert role for faster detection

## Error Handling & Fallback

### Skill Invocation Failure

When the skill fails to execute properly:

| Failure Type | Fallback Strategy |
|--------------|-------------------|
| Document read error | Report error to user, suggest manual fix |
| Expert role detection fails | Use default: General Technical Architect |
| Score calculation error | Use average of dimension scores |
| Document write error | Report error, save audit report only |

### Iteration Failure Handling

When maximum iterations (5) reached without achieving score ≥ 95:

```
┌─────────────────────────────────────────────────────────────┐
│              Iteration Failure Recovery                      │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Step 1: Analyze Blockers                                   │
│  ├── Identify issues that couldn't be resolved              │
│  ├── Categorize: Technical vs. Resource constraints         │
│  └── Check if issues require external expertise             │
│                                                             │
│  Step 2: Generate Report                                    │
│  ├── List unresolved issues with severity                   │
│  ├── Explain why they couldn't be addressed                 │
│  └── Provide recommendations for future resolution          │
│                                                             │
│  Step 3: User Decision                                      │
│  ├── Option A: Accept current score (document is "good enough") │
│  ├── Option B: Manual intervention to resolve blockers      │
│  ├── Option C: Reduce scope (focus on High/Medium issues only) │
│  └── Option D: Request multi-role review for broader perspective │
│                                                             │
│  Step 4: Continue or Exit                                   │
│  ├── If user accepts: Generate final report                 │
│  └── If user intervenes: Resume from current state          │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Failure Report Template

```markdown
## Iteration Limit Reached

**Current Score**: [score]/100
**Iterations Completed**: 5/5
**Target Score**: 95/100

### Unresolved Issues

| ID | Issue | Severity | Blocker Reason |
|----|-------|----------|----------------|

### Recommendations

1. [Specific recommendation]
2. [Specific recommendation]

### User Options

Reply with:
- "接受" to accept current score
- "干预" to manually resolve blockers
- "缩小范围" to focus on critical issues only
- "多角色评审" to request multi-role review
```

## Output Requirements

### Audit Report Storage

**Storage Path Rules:**

| Scenario | Path Rule | Example |
|----------|-----------|---------|
| Same directory (default) | `<document_directory>/<document_name>_audit.md` | `doc/database_model_design_audit.md` |
| User-specified path | User-provided path | `reviews/2026/database_audit.md` |
| Centralized audit folder | `.trae/audits/<document_name>_audit.md` | `.trae/audits/database_model_design_audit.md` |

**Naming Convention:**

```
<original_filename>_audit.md

Examples:
- database_model_design.md → database_model_design_audit.md
- layer_model_design.md → layer_model_design_audit.md
- api_spec_v2.md → api_spec_v2_audit.md
```

**Path Selection Priority:**
1. User explicitly specified path → Use specified path
2. Document in `doc/` folder → Create audit in same folder
3. Document in root or other location → Create `.trae/audits/` folder

### Audit Report Content Structure

```markdown
# [Document Name] Technical Review Report

**Document**: [filename]  
**Version**: [version]  
**Date**: [date]  
**Reviewer Role**: [expert role with background]  
**Review Focus**: [list of focus areas]

---

## 一、Review Overview
[Brief summary of review approach and methodology]

---

## 二、Strengths Analysis

### 2.1 Architecture Strengths
| Strength | Description | Practical Value |

### 2.2 Performance Strengths
| Strength | Description | Practical Value |

### 2.3 Engineering Strengths
| Strength | Description | Practical Value |

### 2.4 Code Quality Strengths
| Strength | Description | Practical Value |

---

## 三、Risks and Issues

### 3.1 🔴 High Risk Issues
#### Issue N: [Title]
**Description**: [detailed description]
**Impact**: [practical consequences]
**Suggestion**: [specific solution]

### 3.2 🟠 Medium Risk Issues
[Same format as High Risk]

### 3.3 🟡 Low Risk Issues
[Same format as High Risk]

---

## 四、Extensibility Analysis

### 4.1 Supported Extension Scenarios
| Scenario | Support Method | Rating |

### 4.2 Pending Extension Scenarios
| Scenario | Current Status | Priority |

---

## 五、Ecosystem Compatibility

### 5.1 Compatibility Assessment
| Component | Compatibility | Notes |

### 5.2 Compatibility Risks
| Risk | Severity | Description |

---

## 六、Implementation Feasibility

### 6.1 Technical Feasibility
| Aspect | Assessment | Notes |

### 6.2 Workload Estimation
| Module | Lines of Code | Duration | Complexity | Dependencies |

### 6.3 Risk Assessment
| Risk | Probability | Impact | Mitigation |

---

## 七、Improvement Suggestions Summary

### 7.1 High Priority (Must Implement)
| ID | Suggestion | Related Issue | Effort |

### 7.2 Medium Priority (Should Implement)
| ID | Suggestion | Related Issue | Effort |

### 7.3 Low Priority (Optional)
| ID | Suggestion | Related Issue | Effort |

---

## 八、Scoring

### 8.1 Dimension Scores
| Dimension | Score | Notes |

### 8.2 Overall Score
[ASCII art visualization of score]

### 8.3 Review Conclusion
**Result**: ✅/❌ Pass/Fail
**Core Strengths**: [numbered list]
**Pending Items**: [numbered list]

---

## 九、Review Signature

**Reviewer**: [expert role and background]  
**Review Date**: [date]  
**Review Status**: [status with grade]  
**Next Step**: [action to take]

---

## 十、Revision Tracking

### 10.1 Revision History
| Version | Date | Changes | Status |

### 10.2 Suggestion Status Tracking
| ID | Suggestion | Priority | Status | Version |

**Statistics**:
- Total suggestions: N
- Completed: N (N%)
- Pending: N (N%)

---

## 十一、Version N Review (For Iterations)

### N.1 Revision Verification
[Table of issues and their resolution status]

### N.2 Version N Dimension Scores
| Dimension | Previous Score | Current Score | Improvement |

### N.3 Version N Overall Score
[ASCII art visualization]

### N.4 Version N Review Conclusion
**Result**: ✅/❌ Pass/Fail
**Improvements Made**: [list]
**Remaining Issues**: [list or "None"]
```

### Output File Summary

| File | Path | Purpose |
|------|------|---------|
| Audit Report | `<doc_path>_audit.md` | Review findings, scores, suggestions |
| Updated Document | Original document path | Improved version with fixes |
| Version History | Within audit report | Track all iterations |

### Content Requirements

**Mandatory Sections:**
1. ✅ Review Overview (with expert role defined)
2. ✅ Strengths Analysis (at least 3 categories)
3. ✅ Risks and Issues (categorized by risk level)
4. ✅ Extensibility Analysis (supported and pending scenarios)
5. ✅ Ecosystem Compatibility (with existing libraries/frameworks)
6. ✅ Implementation Feasibility (workload estimation, risk assessment)
7. ✅ Improvement Suggestions (with priority and effort)
8. ✅ Scoring (dimension scores + overall score)
9. ✅ Review Signature (with status)

**Note:** All 9 sections are mandatory for a complete technical review.

## Example Usage

User request:
> 请以技术评审专家的身份，评审database_model_design.md，重点关注架构合理性、扩展性、性能瓶颈，输出评审意见到audit文件，修改文档直到评分>95

Expected behavior:
1. Read database_model_design.md
2. Review as database architect expert
3. Create database_model_design_audit.md with scores
4. If score < 95, update document to new version
5. Re-review and update audit report
6. Iterate until score >= 95

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
| Do NOT skip dimension scoring | All 6 dimensions must be evaluated |
| Do NOT proceed to next iteration without addressing High/Medium issues | Critical issues must be resolved |
| Do NOT invent expert roles not in the defined list | Use only defined or user-specified roles |
| Do NOT exceed maximum iterations silently | Must report when iteration limit reached |

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
| Do NOT ask user for every decision | Use expert judgment for issue prioritization |
| Do NOT terminate without user consent | Must offer options when stuck |
| Do NOT override user-specified expert role | User role choices take priority |
| Do NOT skip the iterative improvement | Must continue until score ≥ 95 or max iterations |

## Best Practices

1. **Role Immersion**: Fully adopt the expert persona for domain-specific insights
2. **Structured Output**: Use tables, code blocks, and ASCII art for readability
3. **Version Tracking**: Maintain clear version history and change log
4. **Issue Closure**: Ensure every issue has a solution and verification status
5. **Document Sync**: Keep audit report and design document synchronized
6. **Quantitative Scoring**: Provide specific scores, not just qualitative assessments

## Few-shot Examples

### Example 1: Auto-Detect Expert Role

```
> 请评审database_model_design.md

Expected behavior:
1. Auto-detect: Database Architect (from filename "database")
2. Review 6 dimensions
3. Identify issues: connection pool, batch insert, spatial index
4. Create audit report with score 82
5. Improve document
6. Re-review until score ≥ 95
```

### Example 2: User-Specified Expert Role

```
> 请以PostgreSQL内核开发者的身份评审storage_engine.md

Expected behavior:
1. Use specified role: PostgreSQL内核开发者
2. Deep focus on: storage engine, MVCC, WAL, indexing
3. Expert-level insights on PostgreSQL internals
4. Create audit report
5. Iterate until score ≥ 95
```

### Example 3: With Focus Areas

```
> 请评审api_design.md，重点关注安全性和性能

Expected behavior:
1. Auto-detect: API Architect
2. Extra focus on: Security, Performance dimensions
3. Weight security and performance issues higher
4. Create audit report
5. Iterate until score ≥ 95
```

### Example 4: Iteration Progression

```
Initial Review:
- Score: 78/100
- High issues: 2
- Medium issues: 3

Iteration 1:
- Address all High issues
- Score: 85/100
- Remaining: 2 Medium issues

Iteration 2:
- Address remaining issues
- Score: 92/100
- New findings: 1 Low issue

Iteration 3:
- Address Low issue
- Score: 96/100
- ✅ Review complete
```

### Example 5: Maximum Iterations Reached

```
After 5 iterations:
- Score: 92/100
- Unresolved: 1 High issue (requires architecture redesign)

Action:
- Generate failure report
- Offer user options:
  A. Accept current score
  B. Manual intervention
  C. Reduce scope
  D. Add more expert roles
```

## Version History

| Version | Date | Changes | Author |
|---------|------|---------|--------|
| 1.0.0 | 2026-03-15 | Initial skill creation | - |
| 1.1.0 | 2026-03-16 | Added expert role selection modes (Auto/User/Hybrid) | - |
| 1.2.0 | 2026-03-17 | Added detailed audit report template | - |
| 1.3.0 | 2026-03-18 | Added Skill Selection Priority, Negative Constraints, Few-shot Examples | Multi-role Review |
| 1.3.1 | 2026-03-18 | Added Score Validation Rules, Progress Feedback, Token Estimation | Multi-role Review |
| 1.3.2 | 2026-03-18 | Added Error Handling, Comparison Summary, Version History | Multi-role Review |
| 1.3.3 | 2026-03-18 | Added skill document detection rules, AI/LLM Skill Reviewer role | Multi-role Review |

### Change Log Details

**v1.3.3 (Current)**
- Added skill document detection rules in Auto-Detect mode
- Added AI/LLM Skill Reviewer role for skill-related documents
- Improved detection for `.trae/skills/` and `SKILL.md` files

**v1.3.2**
- Added Error Handling & Fallback section
- Added Comparison Summary with multi-role-reviewer
- Added Version History and Future Roadmap
- Fixed description field to include "skill" keyword

**v1.3.1**
- Added Score Validation Rules with calculation formula
- Added Progress Feedback with time/token estimation
- Added Token Estimation for resource planning

**v1.3.0**
- Added Skill Selection Priority for conflict resolution
- Added Negative Constraints to prevent scope violations
- Added Few-shot Examples for better AI understanding
- Added Quick Start section for faster onboarding

## Future Roadmap

### Short-term (v1.4.0)

| Feature | Description | Priority |
|---------|-------------|----------|
| Custom dimension weights | Allow users to customize dimension weights | High |
| Review templates | Pre-defined review templates for common document types | Medium |
| Batch review | Review multiple documents in one session | Medium |
| Export formats | Support PDF/HTML export for audit reports | Low |

### Mid-term (v1.5.0)

| Feature | Description | Priority |
|---------|-------------|----------|
| Learning from feedback | Improve scoring based on user corrections | High |
| Domain-specific checklists | Pre-built checklists for specific domains (GIS, Web, Mobile) | Medium |
| Integration with CI/CD | Automated review as part of CI pipeline | Medium |
| Review history analytics | Track review patterns and common issues | Low |

### Long-term (v2.0.0)

| Feature | Description | Priority |
|---------|-------------|----------|
| Multi-language support | Support for non-English documents | High |
| Visual diff reports | Visual comparison of document versions | Medium |
| Collaborative review | Multiple reviewers on same document | Medium |
| AI model fine-tuning | Custom model for specific domains | Low |

### Contributing

To suggest new features or report issues:
1. Check existing roadmap for duplicates
2. Provide clear use case and expected behavior
3. Consider impact on existing functionality
