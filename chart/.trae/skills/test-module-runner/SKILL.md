---
name: "test-module-runner"
description: "Compiles and runs module tests with 10-second timeout control (default rule: skip tests exceeding 10s). Invoke when user asks to run tests, execute test cases, or test specific modules like 'code/graph/tests'. Generates detailed test reports with case counts."
---

# Test Module Runner

Automates the compilation, execution, and reporting of module tests with **10-second timeout control** (default rule: skip tests exceeding 10 seconds).

## When to Invoke

Invoke this skill when:
- User asks to "run tests" or "execute test cases"
- User mentions testing a specific module (e.g., "test code/graph/tests")
- User requests to "compile and run tests"
- User wants to "run all test cases" with timeout control

## Constraints

**❌ 不要做的事**：

1. **不要在测试运行时中断**: 测试运行中不要强制中断，可能导致Job残留
2. **不要修改测试输出格式**: 依赖`[PASSED]`和`[FAILED]`关键字解析结果
3. **不要在非Windows平台使用**: 当前脚本仅支持PowerShell 5.0+
4. **不要跳过编译步骤**: 必须先编译再运行测试
5. **不要忽略PATH设置**: DLL依赖问题必须通过PATH环境变量解决

**✅ 要做的事**：

1. **先创建Todo List**: 确保任务可追踪
2. **检查编译结果**: 确认编译成功后再运行测试
3. **及时清理Job**: 测试完成后清理所有后台Job
4. **更新测试报告**: 测试完成后必须更新报告文件
5. **默认超时规则**: 单个用例执行时间超过10秒自动跳过（无需用户显式指定）

## Execution Flow

```
┌─────────────────────────────────────────────────────────────┐
│                  Test Module Runner Flow                     │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Step 1: Parse Input Source                                 │
│  ├── 识别模块列表来源                                        │
│  │   ├── code\modules.md → C++模块列表                      │
│  │   ├── cycle\modules_index.md → Cycle模块列表             │
│  │   └── code\[modulename]\tests → 单个模块                 │
│  └── 确定编译目录和报告路径                                  │
│                                                             │
│  Step 2: Create Todo List                                   │
│  ├── Compile the module tests                               │
│  ├── Run tests with timeout control                         │
│  └── Update test report                                     │
│                                                             │
│  Step 3: Compile Tests                                      │
│  └── cmake --build . --config Release                       │
│                                                             │
│  Step 4: Run Tests with Timeout                             │
│  ├── Set PATH environment variable                          │
│  ├── Execute test in background job                         │
│  ├── Wait-Job -Timeout 10                                   │
│  └── Parse output for [PASSED]/[FAILED]                     │
│                                                             │
│  Step 5: Update Test Report                                 │
│  └── Update doc\0_test_report.md                            │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Step 1: Parse Input Source

**基准目录**: 所有路径均相对于**项目根目录**（即 `e:\program\trae\chart/`）

**统一路径配置**：
- 编译目录：`build/`
- 测试目录：`build/test/`
- 测试报告：`doc/0_test_report.md`

#### 1.1 识别输入类型

| 输入模式 | 说明 |
|----------|------|
| `*.md中所有模块` 或 `*.md文件中所有模块` | 从指定 md 文件解析模块列表 |
| `code\[modulename]\tests` | 单个模块测试 |
| `所有模块` | 默认读取 `code/modules.md` |

#### 1.2 模块列表解析

**从 md 文件解析模块概览**：

```
1. 读取指定的 *.md 文件
2. 定位"模块概览"章节（## 模块概览）
3. 解析表格，提取模块信息：
   - 第一列：模块名称
   - 第二列：模块路径
4. 按依赖顺序编译和测试各模块
```

**表格解析规则**：

```markdown
## 模块概览

| 模块 | 路径 | 说明 |
|------|------|------|
| base | code/base | 基础模块 |
| geom | code/geom | 几何模块 |
```

解析结果：
- 模块名：base, geom, ...
- 模块路径：code/base, code/geom, ...

**解析伪代码**：

```
FUNCTION ParseModuleList(mdFilePath):
    content = ReadFile(mdFilePath)
    
    // 定位"模块概览"章节
    overviewSection = ExtractSection(content, "## 模块概览", "##")
    
    // 解析表格
    modules = []
    FOR each line IN overviewSection:
        IF line matches "| [模块名] | [路径] |" THEN
            moduleName = Column[0]
            modulePath = Column[1]
            modules.Add({name: moduleName, path: modulePath})
        END IF
    END FOR
    
    RETURN modules
END FUNCTION
```

#### 1.3 Input Validation

```
IF input matches "*.md中所有模块" THEN
  ├── 提取 md 文件路径
  ├── IF 文件不存在 THEN 报错
  ├── 解析模块列表（调用 ParseModuleList）
  └── 设置 reportPath = "doc/0_test_report.md"
ELSE IF input matches "code\[modulename]\tests" THEN
  ├── 提取模块名称
  └── 设置 reportPath = "doc/0_test_report.md"
ELSE IF input contains "所有模块" THEN
  ├── 默认读取 code/modules.md
  └── 设置 reportPath = "doc/0_test_report.md"
ELSE
  └── 尝试从上下文推断，或询问用户
END IF
```

**Error Messages**:
- "无法识别模块来源，请指定 md 文件路径，如: code/modules.md"
- "文件 [path] 不存在"
- "模块概览表格格式不正确，请检查 md 文件"

### Step 2: Create Todo List

Create a todo list with the following items:
1. Compile the module tests
2. Run tests with timeout control
3. Update test report

### Step 3: Compile Tests

```powershell
cd e:\program\trae\chart\code\build
cmake --build . --config Release
```

Verify compilation success by checking exit code.

### Step 4: Run Tests with Timeout

Use the following PowerShell script pattern:

```powershell
$testDir = "e:\program\trae\chart\code\build\test"
$tests = Get-ChildItem -Path $testDir -Filter "test_*.exe" | Sort-Object Name
$passed = 0; $failed = 0; $skipped = 0

foreach ($t in $tests) {
    $name = $t.Name
    $job = Start-Job -ScriptBlock {
        param($exe, $dir)
        $env:PATH = "$dir;$env:PATH"
        Set-Location $dir
        try {
            $output = & $exe 2>&1 | Out-String
            if ($output -match '\[\s*PASSED\s*\]\s*(\d+)\s*tests?') {
                return @{Success=$true; Count=$matches[1]; Output=$output}
            } elseif ($output -match '\[\s*FAILED\s*\]') {
                return @{Success=$false; Output=$output}
            } else {
                return @{Success=$false; Output="No test output. Exit code: $LASTEXITCODE"}
            }
        } catch {
            return @{Success=$false; Output=$_.Exception.Message}
        }
    } -ArgumentList $t.FullName, $testDir
    
    $completed = Wait-Job $job -Timeout 10
    if (-not $completed) {
        Stop-Job $job; Remove-Job $job
        Write-Host "$name : SKIPPED (>5s)"
        $skipped++
    } else {
        $result = Receive-Job $job
        Remove-Job $job
        if ($result.Success) {
            Write-Host "$name : PASSED ($($result.Count) tests)"
            $passed++
        } else {
            Write-Host "$name : FAILED"
            $failed++
        }
    }
}

Write-Host "`n========== Summary =========="
Write-Host "Passed: $passed, Failed: $failed, Skipped: $skipped"
```

### Step 5: Update Test Report

**重要**: 测试完成后**必须**更新测试报告文件。

#### 5.1 统一报告路径

**报告路径**（相对于项目根目录）：`doc/0_test_report.md`

无论输入来源是哪个 md 文件，测试报告统一输出到此路径。

#### 5.2 报告内容

更新测试报告文件，包含：
1. Test summary table
2. Detailed test results
3. Skipped tests with reasons
4. Any new tests added

#### 5.3 增量更新策略

**当对多个模块依次执行测试时**，采用增量更新策略：
- 每完成2~3个模块测试后，立即更新报告
- 防止因内容过多导致生成报告失败
- 确保每个模块的测试结果都被记录

## Decision Guide

### 编译失败时

```
IF 编译失败 THEN
  ├── 检查CMake配置
  ├── 检查依赖库路径
  └── 输出编译错误信息
END IF
```

**处理步骤**:
1. 检查CMakeLists.txt配置是否正确
2. 检查依赖库路径是否设置
3. 输出编译错误信息供用户分析

### 测试全部跳过时

```
IF 所有测试都被跳过 THEN
  ├── 检查测试文件是否存在
  ├── 检查超时设置是否过短
  └── 建议增加超时时间或单独运行慢测试
END IF
```

**处理步骤**:
1. 检查测试目录是否存在test_*.exe文件
2. 检查10秒超时是否过短
3. 建议用户单独运行慢测试或增加超时时间

### 测试失败时

```
IF 测试失败 THEN
  ├── 输出失败测试的详细信息
  ├── 建议单独运行失败测试查看完整输出
  └── 记录到问题跟踪系统
END IF
```

**处理步骤**:
1. 输出失败测试的名称和错误信息
2. 建议用户单独运行失败测试查看完整输出
3. 将失败信息记录到测试报告中

## Report Format

### Test Summary

```markdown
### 1.1 总体统计

| 项目 | 测试文件数 | 用例数 |
|------|------------|--------|
| 总计 | [TotalFiles] | [TotalCases] |
| ✅ 通过 | [PassedFiles] | [PassedCases] |
| ❌ 失败 | [FailedFiles] | [FailedCases] |
| ⏭️ 跳过 | [SkippedFiles] | [SkippedCases] |

**文件通过率**: [Percentage]% ([PassedFiles]/[TotalFiles])  
**用例通过率**: [Percentage]% ([PassedCases]/[TotalCases])

### 1.2 模块统计

| 模块 | 测试文件数 | 用例数 | 通过(用例) | 失败(用例) | 跳过(用例) | 状态 |
|------|------------|--------|------------|------------|------------|------|
| [Module] | [TotalFiles] | [TotalCases] | [PassedCases] | [FailedCases] | [SkippedCases] | [Status] |
```

**重要说明**：
- 总体统计表必须同时包含测试文件数和用例数
- 模块统计表中的"通过 | 失败 | 跳过"列必须统计用例数，而不是文件数
- 状态列使用：✅（全部通过）、⚠️（有跳过）、❌（有失败）

### Module Introduction

每个模块需要包含以下介绍信息：

```markdown
### X.1 模块介绍

**功能职责**：
- [功能1]：[详细描述]
- [功能2]：[详细描述]
- [功能3]：[详细描述]

**依赖模块**：[依赖的模块列表，如：geom, feature]

**外部依赖**：[外部库列表，如：libpq, sqlite3]
```

### Module Test Summary

```markdown
### X.2 测试汇总

| 状态 | 文件数 | 用例数 |
|------|--------|--------|
| ✅ PASSED | [FileCount] | [PassedCases] |
| ❌ FAILED | [FileCount] | [FailedCases] |
| ⏭️ SKIPPED | [FileCount] | [SkippedCases] |
```

**注意**：汇总表必须同时包含文件数和用例数统计。

### Detailed Test Results

```markdown
### X.3 详细测试结果

| 测试文件 | 测试数 | 状态 | 测试描述 | 备注 |
|----------|--------|------|----------|------|
| test_xxx.exe | N | ✅ PASSED | [测试功能描述] | |
| test_yyy.exe | M | ⏭️ SKIPPED | [测试功能描述] | 执行超过10秒，见问题分析 |
| test_zzz.exe | K | ❌ FAILED | [测试功能描述] | 断言失败，见问题分析 |
```

**注意**：
- 表头必须为：`| 测试文件 | 测试数 | 状态 | 测试描述 | 备注 |`
- "测试数"列统计每个测试文件的用例数量
- 必须包含"测试描述"列，简要描述测试了什么功能
- 对于跳过或失败的测试，需要在"备注"列标注原因
- 对于跳过或失败的测试，需要提供详细的问题分析

### Multi-Case Detail Table

**当测试exe文件对应多个测试用例时**，需以表格形式列出每个用例的详细信息：

```markdown
#### test_xxx.exe 用例详情

| 用例名称 | 所属文件 | 执行状态 | 测试描述 |
|----------|----------|----------|----------|
| TestCase_Constructor | test_xxx.cpp | ✅ PASSED | 测试构造函数 |
| TestCase_SetValue | test_xxx.cpp | ✅ PASSED | 测试设置值 |
| TestCase_InvalidInput | test_xxx.cpp | ❌ FAILED | 测试无效输入处理 |
```

**说明**：
- 当测试文件包含多个用例时，必须在测试文件行下方添加用例详情表格
- 用例名称使用Google Test的测试用例全名
- 所属文件指明测试用例定义的源文件
- 执行状态使用：✅ PASSED、❌ FAILED、⏭️ SKIPPED

### Incremental Report Update

**当对多个模块依次执行测试时**，采用增量更新策略：

```
┌─────────────────────────────────────────────────────────────┐
│              Incremental Report Update Flow                  │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Step 1: 执行2~3个模块测试                                   │
│  ├── 运行模块A测试                                          │
│  ├── 运行模块B测试                                          │
│  └── 运行模块C测试                                          │
│                                                             │
│  Step 2: 立即整理并补充测试结果                              │
│  ├── 更新模块A测试结果到0_test_report.md                    │
│  ├── 更新模块B测试结果到0_test_report.md                    │
│  └── 更新模块C测试结果到0_test_report.md                    │
│                                                             │
│  Step 3: 继续执行其他模块                                    │
│  └── 重复Step 1~2，直到所有模块测试完成                      │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**重要规则**：
- 每完成2~3个模块测试后，立即更新报告
- 防止因内容过多导致生成报告失败
- 确保每个模块的测试结果都被记录
- 最后汇总所有模块的统计数据

### Problem Analysis (for Failed/Skipped Tests)

对于失败或跳过的测试，必须在"问题与建议"章节后提供详细分析：

```markdown
## X、问题与建议

### X.1 失败的测试

以下测试因断言失败或运行时错误而失败：

| 测试文件 | 模块 | 用例数 | 失败原因 |
|----------|------|--------|----------|
| test_xxx.exe | [Module] | [CaseCount] | [失败原因简述] |

#### test_xxx.exe

**失败原因**: [具体失败原因]

**问题分析**:

经过代码分析，发现以下导致测试失败的原因：

1. **[问题类型1]**：
   - [具体问题描述]
   - [影响分析]

2. **[问题类型2]**：
   - [具体问题描述]
   - [影响分析]

**修复建议**:

1. **[修复方案1]**：
   ```cpp
   // 错误代码
   [问题代码示例]
   
   // 建议修改为
   [修复代码示例]
   ```

2. **[修复方案2]**：
   - [具体建议]

### X.2 跳过的测试

以下测试因执行时间超过10秒而被跳过：

| 测试文件 | 模块 | 用例数 | 跳过原因 |
|----------|------|--------|----------|
| test_yyy.exe | [Module] | [CaseCount] | 执行超过10秒 |

#### test_yyy.exe

**跳过原因**: 执行时间超过10秒

**问题分析**:

经过代码分析，发现以下导致执行时间过长的原因：

1. **[问题类型1]**：
   - [具体问题描述]
   - [影响分析]

2. **[问题类型2]**：
   - [具体问题描述]
   - [影响分析]

**优化建议**:

1. **[优化方案1]**：
   ```cpp
   // 错误代码
   [问题代码示例]
   
   // 建议修改为
   [优化代码示例]
   ```

2. **[优化方案2]**：
   - [具体建议]

**预期效果**:

实施以上优化后，预计测试执行时间可从超过10秒降低到[X]秒以内。
```

**重要说明**：
- 失败或跳过的测试必须在"问题与建议"章节后详述
- 必须包含用例数统计
- 必须提供详细的问题分析和修复/优化建议

## Key Technical Points

### 1. PATH Environment Variable

**Critical**: PowerShell background jobs do not inherit environment variables. Must set PATH inside the job:

```powershell
$env:PATH = "$dir;$env:PATH"
```

This ensures DLL dependencies are resolved correctly.

### 2. Working Directory

Always set the working directory inside the job:

```powershell
Set-Location $dir
```

### 3. Timeout Control

Use `Wait-Job -Timeout 10` to implement 10-second timeout:

```powershell
$completed = Wait-Job $job -Timeout 10
if (-not $completed) {
    Stop-Job $job; Remove-Job $job
    # Mark as skipped
}
```

### 4. Result Parsing

Parse test output instead of relying on exit codes:

```powershell
if ($output -match '\[\s*PASSED\s*\]\s*(\d+)\s*tests?') {
    # Test passed
}
```

### 5. Exception Handling

Always wrap test execution in try-catch:

```powershell
try {
    $output = & $exe 2>&1 | Out-String
    # Parse output
} catch {
    return @{Success=$false; Output=$_.Exception.Message}
}
```

## Common Issues and Solutions

| Issue | Cause | Solution |
|-------|-------|----------|
| DLL not found | PATH not set in job | Set `$env:PATH` inside job |
| All tests fail | Working directory wrong | Use `Set-Location $dir` |
| Timeout not working | Not using Wait-Job | Use `Wait-Job -Timeout 10` |
| False failures | Exit code unreliable | Parse output for `[PASSED]` |

## Output Requirements

1. Display test progress in real-time
2. Show summary at the end
3. **必须更新测试报告文件**（根据输入类型确定路径）
4. Mark todos as completed

## Example Usage

### Example 1: 从指定 md 文件测试所有模块

User input:
```
对code\modules.md中所有模块分别进行编译，然后执行测试用例
```

Expected behavior:
1. 读取 `code/modules.md`
2. 解析"模块概览"表格，提取模块列表
3. 编译目录：`build/`
4. 报告路径：`doc/0_test_report.md`
5. 按依赖顺序编译和测试各模块
6. 更新 `doc/0_test_report.md`

### Example 2: 从其他 md 文件测试模块

User input:
```
对cycle\modules_index.md中所有模块进行编译测试
```

Expected behavior:
1. 读取 `cycle/modules_index.md`
2. 解析"模块概览"表格，提取模块列表
3. 编译目录：`build/`
4. 报告路径：`doc/0_test_report.md`
5. 按依赖顺序编译和测试各模块
6. 更新 `doc/0_test_report.md`

### Example 3: 测试单个模块

User input:
```
编译运行code\graph\tests中所有用例，单个用例执行时间超过10s就跳过
```

Expected output:
1. 提取模块名称 = "graph"
2. 编译目录：`build/`
3. 报告路径：`doc/0_test_report.md`
4. Compile graph module tests
5. Run all test_*.exe files
6. Skip tests exceeding 10 seconds
7. Update `doc/0_test_report.md` with results

### Example 4: 默认测试所有模块

User input:
```
编译测试所有模块
```

Expected behavior:
1. 默认读取 `code/modules.md`
2. 解析"模块概览"表格，提取模块列表
3. 编译目录：`build/`
4. 报告路径：`doc/0_test_report.md`
5. 按依赖顺序编译和测试各模块
6. 更新 `doc/0_test_report.md`

### Complete Report Example

生成的测试报告应包含以下完整格式：

```markdown
## 七、Graph模块测试结果

### 7.1 模块介绍

**功能职责**：
- 图形绘制：集成几何、要素、图层、绘制功能
- 边界测试：各类边界值测试（颜色、瓦片键、缓存、样式、变换矩阵等）
- 并发测试：多线程并发绘制测试
- 性能测试：性能基准测试、性能监控

**依赖模块**：geom, feature, layer, draw

**外部依赖**：sqlite3, curl

### 7.2 测试汇总

| 状态 | 文件数 | 用例数 |
|------|--------|--------|
| ✅ PASSED | 2 | 18 |
| ❌ FAILED | 0 | 0 |
| ⏭️ SKIPPED | 1 | 1 |

### 7.3 详细测试结果

| 测试文件 | 用例数 | 状态 | 测试描述 | 备注 |
|----------|--------|------|----------|------|
| test_graph_boundary.exe | 1 | ⏭️ SKIPPED | 边界值测试 | 执行超过10秒，见问题分析 |
| test_graph_concurrent.exe | 13 | ✅ PASSED | 并发绘制测试 | |
| test_graph_performance.exe | 5 | ✅ PASSED | 性能测试 | |

---

## 九、问题与建议

### 9.1 跳过的测试

以下测试因执行时间超过10秒而被跳过：

| 测试文件 | 模块 | 用例数 | 跳过原因 |
|----------|------|--------|----------|
| test_graph_boundary.exe | graph | 1 | 执行超过10秒 |

#### test_graph_boundary.exe

**跳过原因**: 执行时间超过10秒

**问题分析**:

经过代码分析，发现以下导致执行时间过长的原因：

1. **超大循环次数**：
   - `LargeCache` 测试中存在 `for (size_t i = 0; i < std::numeric_limits<size_t>::max(); ++i)` 循环
   - 在64位系统上，`std::numeric_limits<size_t>::max()` = 18,446,744,073,709,551,615
   - 这个循环将执行约1844京次，导致测试永远无法完成

2. **大量重复操作**：
   - 多个测试中存在 `for (int i = 0; i < 10000; ++i)` 循环
   - 每次循环都进行TileKey创建、数据分配、缓存操作等

**优化建议**:

1. **修复超大循环**：
   ```cpp
   // 错误代码
   for (size_t i = 0; i < std::numeric_limits<size_t>::max(); ++i) {
       // ...
   }
   
   // 建议修改为
   for (size_t i = 0; i < 100; ++i) {  // 使用合理的循环次数
       // ...
   }
   ```

2. **减少循环次数**：
   - 将 `for (int i = 0; i < 10000; ++i)` 改为 `for (int i = 0; i < 100; ++i)`
   - 边界测试不需要大量重复，只需验证边界值即可

**预期效果**:

实施以上优化后，预计测试执行时间可从超过10秒降低到1秒以内。
```

## File Paths

**基准目录**: 所有路径均相对于**项目根目录**（即 `e:\program\trae\chart/`）

### 统一路径配置

| 路径类型 | 路径 | 说明 |
|----------|------|------|
| 编译目录 | `build/` | CMake构建目录 |
| 测试可执行 | `build/test/` | `.dll`、`.exe` 文件 |
| 静态库/导入库 | `build/lib/` | `.lib` 文件 |
| 测试报告 | `doc/0_test_report.md` | 测试报告文件 |

### 模块列表文件

| 文件 | 说明 |
|------|------|
| `code/modules.md` | C++模块列表（默认） |
| `cycle/modules_index.md` | Cycle模块列表 |
| 其他 `*.md` 文件 | 需包含"模块概览"章节 |

### 模块源码目录

模块测试源码位于：`[模块路径]/tests/`

例如：
- `code/base/tests/` - base 模块测试
- `code/geom/tests/` - geom 模块测试
- `cycle/core/tests/` - cycle core 模块测试

## Performance Optimization

### Parallel Execution

Tests run in parallel using PowerShell background jobs:

```powershell
$job = Start-Job -ScriptBlock { ... } -ArgumentList $t.FullName, $testDir
$completed = Wait-Job $job -Timeout 10
```

**Benefits**:
- Non-blocking execution
- Timeout control per test
- Resource isolation between tests

### Resource Management

| Resource | Management Strategy |
|----------|---------------------|
| Memory | Each job runs in isolated process |
| CPU | Jobs compete for CPU, timeout prevents runaway tests |
| Handles | Jobs are cleaned up with Remove-Job |

### Scalability

| Test Count | Recommended Timeout | Notes |
|------------|---------------------|-------|
| < 50 | 5s | Default configuration |
| 50-200 | 5-10s | Consider increasing timeout |
| > 200 | 10-30s | May need batch processing |

## Extension Points

### Custom Timeout

Modify the timeout value in the script:

```powershell
$completed = Wait-Job $job -Timeout 10  # 10 seconds
```

### Custom Test Filter

Filter tests by name pattern:

```powershell
$tests = Get-ChildItem -Path $testDir -Filter "test_*_integration.exe"
```

### Custom Report Format

Modify the report format in Step 5 to match your needs.

## Best Practices

### 1. Test Organization

```
code/
├── [module]/
│   └── tests/
│       ├── CMakeLists.txt
│       ├── test_unit.cpp
│       ├── test_integration.cpp
│       └── test_performance.cpp
```

### 2. Naming Convention

| Test Type | Pattern | Example |
|-----------|---------|---------|
| Unit Test | test_[feature].exe | test_clipper.exe |
| Integration Test | test_it_[feature].exe | test_it_layer_manager.exe |
| Performance Test | test_[feature]_perf.exe | test_graph_performance.exe |

### 3. Report Maintenance

- Update report after each test run
- Archive old reports with timestamp
- Track test trends over time

## Related Skills

| Skill | Relationship |
|-------|--------------|
| test-quality-evaluator | Evaluates test quality after running |
| problem-tracker | Tracks failed tests as problems |
| test-report-generator | Generates detailed test reports |

## Version History

| 版本 | 日期 | 变更内容 |
|------|------|----------|
| v1.5 | 2026-04-10 | 增强报告格式：1)表头改为"测试数"；2)多用例时添加用例详情表；3)增量更新策略：每2~3模块立即更新报告，防止内容过多失败 |
| v1.4 | 2026-04-08 | 完善报告格式：总体统计表增加用例数列，模块统计表"通过/失败/跳过"统计用例数，问题与建议章节详述失败/跳过用例 |
| v1.3 | 2026-04-03 | 完善报告格式：添加模块介绍、测试描述列、问题分析格式，提供完整报告示例 |
| v1.2 | 2026-04-03 | 添加性能优化、扩展点、最佳实践、相关技能章节 |
| v1.1 | 2026-04-03 | 添加流程图、约束说明、决策指导、输入验证、版本历史 |
| v1.0 | 2026-04-03 | 初始版本，包含编译、运行、超时控制、报告生成功能 |
