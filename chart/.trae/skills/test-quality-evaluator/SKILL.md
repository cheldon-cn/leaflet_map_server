---
name: "test-quality-evaluator"
description: "Evaluates unit/integration test quality and auto-improves until score>95. Invoke when user asks to evaluate test quality, improve tests, test improvement, or mentions test quality assessment."
---

# Test Quality Evaluator

Automates test quality evaluation and improvement workflow for C++ projects based on standardized evaluation criteria.

## When to Invoke

Invoke this skill when:
- User asks to evaluate test quality for a module
- User wants to improve test coverage or quality
- User mentions test quality assessment or evaluation
- User requests test quality report generation
- User wants to fix test quality issues

## Workflow

### Phase 1: Test Quality Evaluation

1. **Read Evaluation Criteria**
   - Read `doc/tests_quality_evaluate.md` to understand evaluation standards
   - Extract scoring dimensions, weights, and thresholds

2. **Analyze Test Files**
   - Scan `code/[modulename]/tests/` directory
   - Identify unit test files (pattern: `test_*.cpp`, `*_test.cpp`)
   - Identify integration test files (pattern: `test_it_*.cpp`, `*_integration_test.cpp`)

3. **Evaluate Each Dimension**

   **Unit Test Dimensions (Total: 100 points)**
   - Code Coverage (30%): Line, branch, function, class coverage
   - Test Case Design (25%): Boundary values, equivalence classes, exception paths
   - Test Independence (15%): Isolation, resource management
   - Test Readability (15%): Naming, assertions, structure
   - Test Maintainability (15%): Code reuse, mock usage, data management

   **Integration Test Dimensions (Total: 100 points)**
   - Code Coverage (15%): Interface coverage
   - Test Case Design (20%): Scenario coverage
   - Test Independence (10%): Environment isolation
   - Test Readability (10%): Documentation
   - Test Maintainability (10%): Setup/teardown
   - Interface Compatibility (15%): Contract validation
   - Module Collaboration (10%): Workflow completeness
   - Performance Metrics (10%): Response time, resource usage

4. **Generate Evaluation Report**
   - Create `code/[modulename]/tests/[modulename]_test_quality.md`
   - Include detailed scores for each dimension
   - List specific issues found
   - Provide prioritized improvement recommendations
   - **Validate report format** against template structure

5. **Fallback Behavior**
   - If unable to generate complete report, at minimum output:
     - Problem list with priorities
     - Top 3 critical issues
     - Basic coverage statistics

### Phase 2: Iterative Improvement

1. **Parse Problem List**
   - Extract issues from evaluation report
   - Categorize by priority (P0-Critical, P1-Important, P2-General, P3-Low)

2. **Implement Improvements**
   - Process issues in priority order
   - For each issue:
     - Analyze root cause
     - Implement test improvements
     - Verify fix effectiveness

3. **Re-evaluate and Iterate**
   - Re-run evaluation after improvements
   - Update evaluation report
   - Continue iteration if score < 95
   - Stop when comprehensive score ≥ 95
   - **Maximum iterations: 6** - If score still < 95 after 6 iterations, recommend manual intervention

## Evaluation Criteria Details

### Code Coverage Standards

| Metric | Excellent (≥90%) | Good (80-89%) | Pass (70-79%) | Fail (<70%) |
|--------|------------------|---------------|---------------|-------------|
| Line Coverage | Core modules | General modules | Needs improvement | Critical gap |
| Branch Coverage | All branches | Most branches | Partial coverage | Missing tests |
| Function Coverage | 100% public | ≥95% | ≥80% | Inadequate |
| Class Coverage | 100% | ≥95% | ≥80% | Incomplete |

### Test Design Standards

**Boundary Value Testing**
- [ ] Min value, min-1
- [ ] Max value, max+1
- [ ] Empty/null/zero values
- [ ] Type boundaries

**Equivalence Class Partitioning**
- [ ] Valid equivalence classes
- [ ] Invalid equivalence classes
- [ ] Edge cases

**Exception Path Testing**
- [ ] Invalid input parameters
- [ ] Resource exhaustion
- [ ] Error state recovery
- [ ] Error code validation

**Positive/Negative Ratio**
- Positive tests: 60-70%
- Negative tests: 30-40%

### Test Independence Standards

**Isolation Requirements**
- [ ] No execution order dependencies
- [ ] No shared mutable state
- [ ] Independent test execution

**Resource Management**
- [ ] Proper SetUp/TearDown
- [ ] No memory leaks
- [ ] Environment state restoration

### Test Readability Standards

**Naming Convention**
- Pattern: `MethodName_Scenario_ExpectedResult`
- Clear test intent
- Consistent style

**Assertion Clarity**
- Semantic assertion macros
- Descriptive messages
- One focus per test

**Structure**
- AAA pattern (Arrange-Act-Assert)
- Concise and clear
- No complex logic

### Test Maintainability Standards

**Code Reuse**
- [ ] Common logic in helper functions
- [ ] Proper fixture usage
- [ ] No code duplication

**Mock Usage**
- [ ] External dependencies mocked
- [ ] Mock behavior realistic
- [ ] No over-mocking

**Data Management**
- [ ] Test data separated from code
- [ ] Easy to understand/modify
- [ ] Large data in external files

## Output Format

### Evaluation Report Structure

```markdown
# [ModuleName] Test Quality Evaluation Report

**Evaluation Date**: YYYY-MM-DD
**Module Path**: code/[modulename]
**Comprehensive Score**: XX/100 (Grade: A/B/C/D/E)

## 1. Executive Summary

- Overall assessment
- Key findings
- Critical issues

## 2. Unit Test Evaluation

### 2.1 Code Coverage (Score: XX/30)
- Line coverage: XX%
- Branch coverage: XX%
- Function coverage: XX%
- Class coverage: XX%

### 2.2 Test Case Design (Score: XX/25)
- Boundary value testing: [details]
- Equivalence class partitioning: [details]
- Exception path testing: [details]
- Positive/negative ratio: XX%/XX%

### 2.3 Test Independence (Score: XX/15)
- Test isolation: [assessment]
- Resource management: [assessment]

### 2.4 Test Readability (Score: XX/15)
- Naming convention: [assessment]
- Assertion clarity: [assessment]
- Structure quality: [assessment]

### 2.5 Test Maintainability (Score: XX/15)
- Code reuse: [assessment]
- Mock usage: [assessment]
- Data management: [assessment]

## 3. Integration Test Evaluation

[Similar structure as Unit Test section]

## 4. Problem List

### P0 - Critical Issues
1. [Issue description]
   - Location: [file:line]
   - Impact: [description]
   - Recommendation: [action]

### P1 - Important Issues
[...]

### P2 - General Issues
[...]

### P3 - Low Priority Issues
[...]

## 5. Improvement Recommendations

### Immediate Actions (P0)
1. [Recommendation]

### Short-term Actions (P1)
[...]

### Long-term Actions (P2/P3)
[...]

## 6. Next Steps

- [ ] Action item 1
- [ ] Action item 2
```

## Improvement Process

### Priority-Based Implementation

**P0 - Critical (Must Fix Immediately)**
- Missing tests for core functionality
- Coverage < 70% for critical modules
- Test failures in CI/CD

**P1 - Important (Fix Within Sprint)**
- Coverage gaps in important features
- Missing boundary tests
- Poor test isolation

**P2 - General (Fix When Possible)**
- Naming convention violations
- Documentation gaps
- Minor code duplication

**P3 - Low (Nice to Have)**
- Optimization opportunities
- Style improvements
- Non-critical enhancements

### Implementation Steps

For each improvement:
1. **Analyze**: Understand the issue and its impact
2. **Design**: Plan the test improvement approach
3. **Implement**: Write or modify test code
4. **Verify**: Run tests and check coverage
5. **Document**: Update test documentation if needed

### Verification Checklist

After each improvement iteration:
- [ ] All tests pass
- [ ] Coverage increased
- [ ] No new issues introduced
- [ ] Evaluation score improved
- [ ] Report updated

## Usage Example

```
User: Evaluate test quality for the geom module

Skill Actions:
1. Read doc/tests_quality_evaluate.md
2. Scan code/geom/tests/
3. Evaluate all dimensions
4. Generate code/geom/tests/geom_test_quality.md
5. If score < 95:
   - Parse problem list
   - Implement improvements by priority
   - Re-evaluate
   - Update report
   - Repeat until score ≥ 95
```

## Important Notes

1. **Always read evaluation criteria first** - Standards may evolve
2. **Preserve existing tests** - Only enhance, don't remove without reason
3. **Maintain test independence** - New tests must be isolated
4. **Follow project conventions** - Match existing test style
5. **Document changes** - Update report after each iteration
6. **Stop at score ≥ 95** - Avoid over-engineering

## Success Criteria

- Comprehensive evaluation score ≥ 95
- All P0 and P1 issues resolved
- Test coverage meets project standards
- Tests are maintainable and readable
- No regression in existing functionality

## Related Skills

This skill can work collaboratively with other skills:

| Skill | Collaboration Scenario | Priority |
|-------|----------------------|----------|
| **code-implementer** | After implementing code, use this skill to evaluate test quality | High |
| **problem-tracker** | Track issues found during test evaluation and improvement | Medium |
| **skill-validator** | Validate this skill's quality before release | Low |

### Skill Combination Workflow

```
Typical workflow:
1. code-implementer → Implement new features
2. test-quality-evaluator → Evaluate test quality
3. problem-tracker → Track test issues
4. test-quality-evaluator → Re-evaluate after fixes
```

### Conflict Resolution

When multiple skills match user request:
- If user asks "implement and test": Use code-implementer first, then test-quality-evaluator
- If user asks "evaluate quality": Use test-quality-evaluator directly
- If user asks "track test issues": Use problem-tracker for tracking, test-quality-evaluator for evaluation

## Performance Estimation

### Execution Time

| Phase | Estimated Time | Notes |
|-------|---------------|-------|
| Read evaluation criteria | 10-30 seconds | Depends on document size |
| Analyze test files | 30-60 seconds | Depends on test file count |
| Evaluate dimensions | 1-3 minutes | Per module |
| Generate report | 30-60 seconds | Standard format |
| Implement improvements | 2-5 minutes | Per iteration |
| **Total per iteration** | **4-8 minutes** | Average case |

### Token Consumption

| Component | Token Range | Notes |
|-----------|-------------|-------|
| Skill loading | 2,000-3,000 | One-time |
| Evaluation criteria reading | 500-1,500 | Depends on doc size |
| Test file analysis | 1,000-3,000 | Depends on test count |
| Report generation | 1,500-2,500 | Standard template |
| Improvement implementation | 2,000-5,000 | Per iteration |
| **Total per iteration** | **7,000-15,000** | Average case |

### Resource Optimization

For large projects:
- Focus on high-priority issues first
- Use incremental evaluation (only re-evaluate changed dimensions)
- Limit iterations to 3 for initial evaluation
