---
name: "rule-validator"
description: "Validates rule logic consistency between long and split versions. Invoke when user asks to validate rules or check rule differences."
---

# Rule Validator

Validates rule logic consistency and supplements missing rules.

## When to Use

- User asks to validate rules
- User mentions "rule differences" or "rule consistency"
- After splitting rules with rule-splitter
- User wants to check if rules are complete

## Input Parameters

| Parameter | Description | Default |
|-----------|-------------|---------|
| `flowname` | Rule file name prefix | Required |
| `prefix` | Sub-rule name prefix | `[flowname]` first letters |

## Execution Flow

```
┌─────────────────────────────────────────────────────────────┐
│                  Rule Validator Flow                        │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Step 1: Read Source Files                                  │
│  ├── Read long version                                      │
│  ├── Read split version                                     │
│  └── Read all sub-rules                                     │
│           │                                                 │
│           ↓                                                 │
│  Step 2: Compare Rule Logic                                 │
│  ├── Technical constraints                                  │
│  ├── Implementation principles                              │
│  ├── Rule ID coverage                                       │
│  └── Example completeness                                   │
│           │                                                 │
│           ↓                                                 │
│  Step 3: Identify Differences                               │
│     ┌───────┴───────┐                                       │
│     ↓               ↓                                       │
│  [Consistent]   [Missing]                                   │
│     │               │                                       │
│     ↓               ↓                                       │
│  [Done]      Step 4: Prioritize                             │
│                   │                                         │
│                   ↓                                         │
│             Step 5: Supplement                              │
│                   │                                         │
│                   ↓                                         │
│             Step 6: Validate                               │
│                   │                                         │
│                   ↓                                         │
│                [Done]                                       │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

## Constraints

**❌ Do NOT**:

1. **Do not skip any dimension**: Must validate all dimensions (technical constraints, principles, rule IDs, examples)
2. **Do not ignore missing content**: All missing content must be identified and prioritized
3. **Do not modify long version**: Long version is the reference, do not modify it
4. **Do not exceed character limits**: Always validate character counts after supplementing
5. **Do not forget to update version numbers**: Update version after changes

**✅ DO**:

1. **Always use long version as reference**: Long version is the source of truth
2. **Prioritize by impact**: P0 > P1 > P2 > P3
3. **Validate after each supplement**: Check character counts immediately
4. **Keep sub-rules focused**: Each sub-rule should address single topic
5. **Document all changes**: Record what was supplemented and why

## Execution Steps

### Step 1: Read Source Files

1. Read long version: `.trae/rules/docs/[flowname]_rules_long.md`
   - If file not found, output error: "Long version not found: [flowname]_rules_long.md"
   - If file format invalid, output error: "Invalid long version format"
2. Read split version: `.trae/rules/[flowname]_rules.md`
   - If file not found, output error: "Split version not found: [flowname]_rules.md"
3. Read all sub-rules: `.trae/rules/[prefix]_*.md`
   - If no sub-rules found, output warning: "No sub-rules found with prefix: [prefix]"

### Step 2: Compare Rule Logic

Compare the following dimensions:

| Dimension | Description | Check Method |
|-----------|-------------|--------------|
| 技术约束 | Technical constraints | Compare constraint items one by one |
| 实施原则 | Implementation principles | Compare principle statements |
| 规则ID覆盖 | Rule ID coverage | Check all IDs are present |
| 示例完整性 | Example completeness | Compare example lists |
| 检查清单 | Check lists | Compare checklist items |

### Step 3: Identify Differences

For each dimension, identify:

| Status | Description | Action |
|--------|-------------|--------|
| ✅ 一致 | Content matches | No action needed |
| ⚠️ 部分缺失 | Partially missing | Add to P1/P2 list |
| ❌ 缺失 | Completely missing | Add to P0 list |

### Step 4: Prioritize Missing Content

Priority levels:

| Priority | Description | Action | Example |
|----------|-------------|--------|---------|
| **P0** | Critical content missing | Immediate supplement | 核心框架约束缺失 |
| **P1** | Important content missing | Supplement soon | API命名对照表缺失项 |
| **P2** | Minor content missing | Supplement if time permits | 示例代码缺失 |

**Decision Point**:
- If P0 items exist, must supplement before proceeding
- If only P1/P2 items exist, ask user if supplement is needed

### Step 5: Supplement Missing Rules

For each missing item:

1. Create or update sub-rule file
2. Add missing content
3. Validate character count immediately
   - If exceeds 1024 chars, split into multiple parts
4. Update references in core rule
5. Update version number

**Error Handling**:
- If sub-rule exceeds 1024 chars after supplement, split into parts
- If reference format incorrect, fix immediately

### Step 6: Validate Character Counts

```powershell
# Check all sub-rules
Get-ChildItem "[prefix]_*.md" | ForEach-Object {
    $content = Get-Content $_.FullName -Raw
    if ($content.Length -gt 1024) {
        Write-Output "WARNING: $($_.Name) exceeds 1024 chars ($($content.Length) chars)"
        Write-Output "Action: Split into multiple sub-rules"
    }
}
```

## Input Validation

Before processing, validate all inputs:

| Input | Validation Rule | Error Message |
|-------|-----------------|---------------|
| `flowname` | Non-empty, alphanumeric + underscore | "Invalid flowname: must be alphanumeric with underscores" |
| `flowname` | Length <= 50 chars | "flowname too long: max 50 characters" |
| `flowname` | No reserved names | "Reserved flowname: cannot use 'con', 'prn', 'aux', 'nul'" |
| `prefix` | Non-empty, alphanumeric + underscore | "Invalid prefix: must be alphanumeric with underscores" |
| `prefix` | Length <= 20 chars | "prefix too long: max 20 characters" |

**Special Character Handling**:
- Trim whitespace from all string inputs
- Convert to lowercase if needed
- Reject special characters: `< > : " | ? *`
- Reject control characters (ASCII 0-31)
- Reject Unicode control characters

**File Path Validation**:
- Check for path traversal attempts (e.g., `../`)
- Validate file extension is `.md`
- Check file size is reasonable (< 10MB)

**Edge Case Handling**:

| Edge Case | Detection | Recovery |
|-----------|-----------|----------|
| Empty long version | File size = 0 | "Long version is empty, cannot validate" |
| Empty split version | File size = 0 | "Split version is empty, run rule-splitter first" |
| No sub-rules | Zero files matching prefix | "No sub-rules found, check prefix or run rule-splitter" |
| Circular references | Reference graph cycle detection | Break cycle, report affected files |
| Duplicate rule IDs | ID collision scan | Renumber or merge duplicates |
| Missing references | Broken @.trae/rules/ links | Create placeholder or remove reference |
| Very long sub-rule | File > 1024 chars | Flag for splitting |
| Mixed encodings | Encoding mismatch detection | Convert all to UTF-8 |
| Concurrent access | File lock check | Wait and retry, max 3 attempts |
| Corrupted markdown | Syntax validation | Attempt repair or report error |

**Circular Reference Detection**:

```powershell
# Detect circular references in rule files
function Test-CircularReference {
    param($FilePath, $Visited = @{})
    
    if ($Visited[$FilePath]) {
        return $true  # Circular reference detected
    }
    
    $Visited[$FilePath] = $true
    $content = Get-Content $FilePath -Raw
    
    # Find all @.trae/rules/ references
    $refs = [regex]::Matches($content, '@\.trae/rules/([^.]+)\.md')
    
    foreach ($ref in $refs) {
        $refPath = ".trae/rules/$($ref.Groups[1].Value).md"
        if (Test-CircularReference $refPath $Visited) {
            Write-Output "WARNING: Circular reference detected: $FilePath -> $refPath"
            return $true
        }
    }
    
    return $false
}
```

## Environment Adaptation

This skill adapts to different environments:

| Environment | Adaptation |
|-------------|------------|
| Windows | Use PowerShell commands, backslash paths |
| Linux/macOS | Use bash commands, forward slash paths |
| CI/CD | Suppress interactive prompts, use exit codes |
| IDE | Enable verbose output for debugging |

**Cross-Platform Compatibility**:
- Use `Get-Content` on Windows, `cat` on Linux/macOS
- Use `Get-ChildItem` on Windows, `ls` on Linux/macOS
- Use `Join-Path` for path construction (handles both `/` and `\`)
- Use `[Environment]::NewLine` for line endings

## Error Recovery

| Error Type | Recovery Action |
|------------|-----------------|
| Long version not found | Check file path, provide correct path |
| Split version not found | Run rule-splitter first |
| No sub-rules found | Check prefix, run rule-splitter if needed |
| Character limit exceeded | Split content into smaller sub-rules |
| Invalid format | Validate markdown syntax, fix formatting |
| Encoding error | Detect encoding, convert to UTF-8 |
| Disk full | Clean temp files, alert user |
| Circular reference | Detect and break circular references |
| File locked | Wait 1s, retry up to 3 times |
| Memory allocation failed | Free cache, reduce batch size |
| Invalid UTF-8 sequence | Replace with U+FFFD, log warning |
| Rule ID collision | Renumber duplicates, log warning |
| Missing reference | Create placeholder file, alert user |
| Validation timeout | Reduce scope, validate incrementally |

**Recovery Strategy**:

```powershell
# Retry with exponential backoff
function Retry-Operation {
    param($Operation, $MaxRetries = 3)
    $delay = 1
    for ($i = 1; $i -le $MaxRetries; $i++) {
        try {
            return & $Operation
        } catch {
            if ($i -eq $MaxRetries) { throw }
            Start-Sleep -Seconds $delay
            $delay *= 2
        }
    }
}

# Validate incrementally for large rule sets
function Invoke-IncrementalValidation {
    param($RuleSet, $BatchSize = 10)
    $results = @()
    $batches = [Math]::Ceiling($RuleSet.Count / $BatchSize)
    
    for ($i = 0; $i -lt $batches; $i++) {
        $batch = $RuleSet[($i * $BatchSize)..([Math]::Min(($i + 1) * $BatchSize - 1, $RuleSet.Count - 1))]
        $results += Validate-Batch $batch
        Write-Progress -Activity "Validating" -Status "Batch $($i + 1)/$batches" -PercentComplete (($i + 1) / $batches * 100)
    }
    
    return $results
}
```

**Disk Space Check**:

```powershell
# Check available disk space before writing
$drive = (Get-Location).Drive
if ($drive.AvailableFreeSpace -lt 10MB) {
    Write-Output "WARNING: Low disk space (< 10MB)"
    Write-Output "Action: Clean temp files or free up space"
    exit 1
}
```

**Memory Management**:

```powershell
# Monitor memory usage during validation
$memory = [System.GC]::GetTotalMemory($false)
if ($memory -gt 500MB) {
    Write-Output "INFO: High memory usage ($([math]::Round($memory/1MB, 2))MB)"
    [System.GC]::Collect()
}
```

## Output Format

### Validation Report

```markdown
## Rule Logic Validation Report

### 1. Technical Constraints

| Item | Long Version | Split Version | Status |
|------|--------------|---------------|--------|
| 编程语言 | C++11 | C++11 | ✅ 一致 |
| 核心框架 | 有 | 缺失 | ❌ 需补充 |

### 2. Missing Content Priority

| Priority | Content | Impact |
|----------|---------|--------|
| P0 | 核心框架约束 | 技术约束不完整 |

### 3. Supplement Actions

| Action | File | Status |
|--------|------|--------|
| Add core framework | project_rules.md | ✅ Done |
```

## Validation Checklist

### Technical Constraints

```markdown
□ 编程语言版本一致
□ 核心框架约束一致
□ 代码规范一致
□ 外部依赖一致
```

### Implementation Principles

```markdown
□ 以设计文档为准原则一致
□ 疑问处理原则一致
□ 文档优先原则一致
□ 规则索引一致
```

### Rule ID Coverage

```markdown
□ STD-01~05 覆盖完整
□ HDR-01~04 覆盖完整
□ API-01~03 覆盖完整
□ CONST-01~03 覆盖完整
□ PTR-01~05 覆盖完整
□ INT-01~03 覆盖完整
□ DLL-01~02 覆盖完整
□ GEO-01~03 覆盖完整
□ INIT-01~03 覆盖完整
□ CMAKE-01~04 覆盖完整
□ TEST-01, UT-01~05 覆盖完整
```

## Performance Optimization

### Large Rule Set Handling

For projects with > 50 sub-rules:

| Optimization | Description |
|--------------|-------------|
| Incremental validation | Validate only changed files |
| Parallel comparison | Compare multiple dimensions in parallel |
| Lazy loading | Load sub-rules only when needed |
| Memory monitoring | Monitor memory usage, alert if > 100MB |

### Batch Validation

When validating multiple rule sets:

```powershell
# Validate multiple rule sets efficiently
$ruleSets = @("project", "task", "flow")
foreach ($set in $ruleSets) {
    Write-Output "Validating $set rules..."
    # ... validation logic ...
}
```

### Caching Strategy

Cache validation results:

| Data Type | Cache Duration | Invalidation |
|-----------|----------------|--------------|
| Long version content | Until file modified | File timestamp change |
| Validation results | Until content change | Content hash change |
| Missing content list | Until supplement | Manual clear |

## Use Cases

### Use Case 1: Post-Split Validation

**Scenario**: After running rule-splitter, validate the results.

**Steps**:
1. Run rule-validator with `flowname="project"`
2. Review validation report
3. Supplement missing content if any
4. Re-validate until all rules consistent

### Use Case 2: Pre-Commit Validation

**Scenario**: Before committing rule changes, validate consistency.

**Steps**:
1. Make changes to long version
2. Run rule-validator
3. Fix any inconsistencies
4. Commit both long and split versions

### Use Case 3: Rule Audit

**Scenario**: Audit existing rules for completeness.

**Steps**:
1. Run rule-validator on all rule sets
2. Generate comprehensive report
3. Prioritize missing content by impact
4. Create action plan for fixes

## Best Practices

1. **Always compare with long version as reference**: Long version is the source of truth
2. **Prioritize missing content by impact**: P0 > P1 > P2 > P3
3. **Validate character counts after each supplement**: Immediate validation prevents issues
4. **Keep sub-rules focused on single topic**: Single responsibility principle
5. **Update version numbers after changes**: Track all modifications
6. **Run validation after any change**: Always validate after modifications
7. **Keep validation reports**: Store reports for historical tracking
8. **Automate validation**: Integrate into CI/CD pipeline

## Example Usage

### Basic Usage

```
User: Validate project_rules.md against project_rules_long.md

AI:
1. Reading both files
2. Comparing rule logic
3. Found 3 differences:
   - Missing: 核心框架约束 (P0)
   - Missing: 外部依赖 (P0)
   - Missing: 文档优先原则 (P0)
4. Supplementing missing content
5. Validating character counts
6. All rules now consistent!
```

### Advanced Usage with Error Handling

```
User: Validate large_rules.md

AI:
1. Reading long version: large_rules_long.md
2. Reading split version: large_rules.md
3. Reading 20 sub-rules
4. Comparing rule logic
   WARNING: 5 sub-rules exceed 1024 chars
   Action: Will need to split after validation
5. Found 8 differences:
   - P0: 核心框架约束缺失
   - P0: 外部依赖缺失
   - P1: API命名对照表缺失3项
   - P1: 检查清单缺失2项
   - P2: 示例代码缺失
6. Supplementing P0 items first
7. Supplementing P1 items
8. Validating character counts
   WARNING: pr_api_naming.md now 1100 chars
   Action: Splitting into pr_api_naming_basic.md and pr_api_naming_advanced.md
9. All rules now consistent!
```

## Integration with Other Skills

This skill works well with:

| Skill | Integration |
|-------|-------------|
| rule-splitter | Use rule-splitter first to split long rules, then validate |
| skill-creator | Use skill-creator to create new validation skills |
| technical-reviewer | Use technical-reviewer to review validation reports |

---

**Version**: v1.2
