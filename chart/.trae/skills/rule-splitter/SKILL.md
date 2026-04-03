---
name: "rule-splitter"
description: "Splits long rule files into modular sub-rules. Invoke when rule file exceeds 1000 chars or user asks to reorganize rules."
---

# Rule Splitter

Splits long rule files into modular sub-rules with character count control.

## When to Use

- Rule file exceeds 1000 characters
- User asks to reorganize rules
- User mentions "split rules" or "modular rules"
- User wants to create sub-rules

## Input Parameters

| Parameter | Description | Default |
|-----------|-------------|---------|
| `flowname` | Rule file name prefix | Required |
| `prefix` | Sub-rule name prefix | `[flowname]` first letters |
| `max_chars` | Max chars per sub-rule | 1024 |
| `core_max_chars` | Max chars for core rule | 1000 |

## Execution Flow

```
┌─────────────────────────────────────────────────────────────┐
│                  Rule Splitter Flow                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Step 1: Analyze Source Rule                                │
│  ├── Read rule file                                         │
│  ├── Calculate character count                              │
│  └── Check if splitting needed                              │
│           │                                                 │
│     ┌─────┴─────┐                                           │
│     ↓           ↓                                           │
│  [<=1000]    [>1000]                                        │
│     │           │                                           │
│     ↓           ↓                                           │
│  [Skip]     Step 2: Identify Sections                       │
│                 │                                           │
│                 ↓                                           │
│           Step 3: Create Core Rule                          │
│                 │                                           │
│                 ↓                                           │
│           Step 4: Create Sub-Rules                          │
│                 │                                           │
│                 ↓                                           │
│           Step 5: Validate Character Counts                 │
│                 │                                           │
│                 ↓                                           │
│              [Done]                                         │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

## Constraints

**❌ Do NOT**:

1. **Do not split rules under 1000 chars**: If the rule file is already within limit, splitting is unnecessary
2. **Do not modify original long rule file**: Keep the original file as reference
3. **Do not skip character validation**: Always validate character counts after splitting
4. **Do not mix different rule types**: Keep sub-rules focused on single topics
5. **Do not forget to update references**: Ensure all references are updated after splitting

**✅ DO**:

1. **Always backup original file**: Before splitting, backup the original rule file
2. **Validate after each step**: Check character counts after creating each sub-rule
3. **Use consistent naming**: Follow the naming convention strictly
4. **Test references**: Verify all @.trae/rules/ references work correctly

## Execution Steps

### Step 1: Analyze Source Rule

1. Read source rule file: `.trae/rules/[flowname]_rules.md`
   - If file not found, output error: "Rule file not found: [flowname]_rules.md"
   - If file format invalid, output error: "Invalid rule file format"
   - If permission denied, output error: "Permission denied to read rule file"
2. Calculate character count
3. If count <= 1000, output: "Rule file is within limit ([count] chars), no splitting needed" and skip
4. If count > 1000, proceed to split

### Step 2: Identify Rule Sections

Identify sections in the source rule:

| Section Type | Description | Decision Point |
|--------------|-------------|----------------|
| 技术约束 | Technical constraints | Always create separate sub-rule |
| 实施原则 | Implementation principles | If content > 200 chars, create sub-rule |
| 避坑清单 | Pitfall checklists | Always create separate sub-rule |
| 检查清单 | Check lists | Always create separate sub-rule |
| 诊断表 | Diagnosis tables | Always create separate sub-rule |
| 执行流程 | Execution flows | If content > 300 chars, create sub-rule |

### Step 3: Create Core Rule

Create core rule file with:

```markdown
# [Rule Name]（核心）

> **相关文档**: @doc/experience_standard.md

---

## 一、技术约束（强制）

| 约束项 | 要求 | 规则ID |
|--------|------|--------|
| ... | ... | ... |

---

## 二、实施原则

- ...

---

## 三、规则引用

| 文件 | 内容 |
|------|------|
| @.trae/rules/[prefix]_xxx.md | ... |

**版本**: v1.0
```

### Step 4: Create Sub-Rules

For each section:

1. Create sub-rule file: `[prefix]_[section_name].md`
2. Add header: `# [Section Title]`
3. Add reference: `> **主规则**: @.trae/rules/[flowname]_rules.md`
4. Add content in table format
5. Add version footer
6. Validate character count immediately

**Error Handling**:
- If sub-rule exceeds 1024 chars, split into multiple parts
- If reference format incorrect, output warning

### Step 5: Validate Character Counts

```powershell
# Check core rule
$content = Get-Content "[flowname]_rules.md" -Raw
if ($content.Length -gt 1000) {
    Write-Output "WARNING: Core rule exceeds 1000 chars"
    Write-Output "Action: Review core rule content and move details to sub-rules"
}

# Check sub-rules
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
| `max_chars` | Integer, 100 <= value <= 10000 | "Invalid max_chars: must be 100-10000" |
| `core_max_chars` | Integer, 100 <= value <= 10000 | "Invalid core_max_chars: must be 100-10000" |

**Special Character Handling**:
- Trim whitespace from all string inputs
- Convert to lowercase if needed
- Reject special characters: `< > : " | ? *`
- Reject control characters (ASCII 0-31)
- Reject Unicode control characters

**Edge Case Handling**:

| Edge Case | Detection | Recovery |
|-----------|-----------|----------|
| Empty file | File size = 0 | "File is empty, nothing to split" |
| Very long filename | Length > 200 chars | Truncate to 200 chars, add hash suffix |
| Special characters in content | Regex scan | Escape or replace with safe alternatives |
| Mixed line endings | Detect CRLF/LF mix | Normalize to LF |
| BOM marker | Detect UTF-8 BOM | Remove BOM before processing |
| Duplicate section names | Name collision | Add numeric suffix (e.g., `_2`) |
| Circular references | Reference graph scan | Break cycle, report to user |
| Concurrent access | File lock check | Wait and retry, max 3 attempts |

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
| File not found | Check file path, create file if needed |
| Permission denied | Check file permissions, run as admin if needed |
| Character limit exceeded | Split content into smaller sub-rules |
| Invalid format | Validate markdown syntax, fix formatting |
| Encoding error | Detect encoding, convert to UTF-8 |
| Disk full | Clean temp files, alert user |
| Network error (if applicable) | Retry with exponential backoff |
| File locked | Wait 1s, retry up to 3 times |
| Memory allocation failed | Free cache, reduce batch size |
| Invalid UTF-8 sequence | Replace with U+FFFD, log warning |
| Section too large | Split into multiple parts automatically |
| Reference resolution failed | Skip reference, add to error list |

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
# Monitor memory usage
$memory = [System.GC]::GetTotalMemory($false)
if ($memory -gt 500MB) {
    Write-Output "INFO: High memory usage ($([math]::Round($memory/1MB, 2))MB)"
    [System.GC]::Collect()
}
```

## Output Format

### Core Rule Structure

```
[flowname]_rules.md (核心规则)
├── [prefix]_std_compat.md (C++标准兼容性)
├── [prefix]_header.md (头文件管理)
├── [prefix]_api_naming.md (API命名一致性)
├── [prefix]_const.md (const正确性)
├── [prefix]_pointer.md (智能指针转换)
├── [prefix]_interface_design.md (接口设计避坑清单)
├── [prefix]_data_structure.md (数据结构与算法避坑清单)
├── [prefix]_build_config.md (构建配置避坑清单)
├── [prefix]_checklist.md (编码实施检查清单)
├── [prefix]_diagnosis.md (快速诊断表)
├── [prefix]_execution_flow.md (执行流程约束)
└── [prefix]_top_issues.md (高频问题Top 10)
```

## Performance Optimization

### Large File Handling

For rule files > 50KB:

| Optimization | Description |
|--------------|-------------|
| Stream reading | Read file in chunks, not all at once |
| Lazy parsing | Parse sections only when needed |
| Parallel processing | Create sub-rules in parallel (if supported) |
| Memory monitoring | Monitor memory usage, alert if > 100MB |

### Batch Processing

When splitting multiple rule files:

```powershell
# Process multiple files efficiently
$files = @("project_rules", "task_rules", "flow_rules")
foreach ($file in $files) {
    # Process each file
    Write-Output "Processing $file..."
    # ... splitting logic ...
}
```

### Caching Strategy

Cache frequently accessed data:

| Data Type | Cache Duration | Invalidation |
|-----------|----------------|--------------|
| File content | Until file modified | File timestamp change |
| Character count | Until content change | Content hash change |
| Section structure | Until structure change | Structure hash change |

## Use Cases

### Use Case 1: Initial Rule Splitting

**Scenario**: User has a large monolithic rule file.

**Steps**:
1. Run rule-splitter with `flowname="project"`
2. Review generated sub-rules
3. Run rule-validator to check consistency
4. Adjust as needed

### Use Case 2: Rule Maintenance

**Scenario**: User needs to update existing split rules.

**Steps**:
1. Update the long version file
2. Run rule-splitter again
3. Compare changes with previous version
4. Update only changed sub-rules

### Use Case 3: Rule Migration

**Scenario**: User migrates rules from another project.

**Steps**:
1. Copy long version to `.trae/rules/docs/`
2. Run rule-splitter
3. Run rule-validator
4. Fix any inconsistencies

## Best Practices

1. **Use tables instead of long sentences**: Tables are more compact and readable
2. **Move examples to external files**: `rules/docs/examples/`
3. **Keep each sub-rule focused on one topic**: Single responsibility principle
4. **Use @.trae/rules/xxx.md format for references**: AI can auto-load referenced files
5. **Add version number to all files**: Track changes over time
6. **Test after splitting**: Always run rule-validator after splitting
7. **Backup before changes**: Keep backup of original files
8. **Document changes**: Record what was changed and why

## Example Usage

### Basic Usage

```
User: Split project_rules.md into sub-rules

AI: 
1. Reading project_rules.md (8000+ chars)
2. Creating core rule (879 chars)
3. Creating 13 sub-rules (all < 1024 chars)
4. Validating character counts
5. Done!
```

### Advanced Usage with Error Handling

```
User: Split large_rules.md into sub-rules

AI:
1. Reading large_rules.md (15000+ chars)
2. Creating core rule (1200 chars)
   WARNING: Core rule exceeds 1000 chars
   Action: Moving detailed constraints to sub-rules
3. Creating core rule (950 chars) ✓
4. Creating 20 sub-rules
   WARNING: sub_rule_15.md exceeds 1024 chars (1100 chars)
   Action: Splitting into sub_rule_15a.md and sub_rule_15b.md
5. Validating character counts
   - Core rule: 950 chars ✓
   - All sub-rules: < 1024 chars ✓
6. Done!
```

## Integration with Other Skills

This skill works well with:

| Skill | Integration |
|-------|-------------|
| rule-validator | After splitting, use rule-validator to check consistency |
| skill-creator | Use skill-creator to create new skills from templates |
| technical-reviewer | Use technical-reviewer to review split rules |

---

**Version**: v1.3
