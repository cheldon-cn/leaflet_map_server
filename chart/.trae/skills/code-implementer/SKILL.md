---
name: "code-implementer"
description: "Guides TDD implementation with >80% coverage for Python/C++/Java. Invoke when user asks to implement tasks from a task list, follow tasks.md plan, or start coding phase. 触发关键词：编码实施、任务清单、tasks.md、按照清单、继续实施、里程碑、实现任务。"
---

# Code Implementer

编码实施Skill，指导按照任务清单进行代码实现，采用TDD（测试驱动开发）流程，确保代码质量和测试覆盖率。

---

## 一、Skill概述

### 1.1 功能定位

本Skill用于在任务规划完成后，指导代码实施阶段的工作，确保：

1. **核心框架先行**：先构建可运行的最小系统
2. **TDD开发流程**：测试先行，编码同步
3. **质量保证**：测试覆盖率 >80%
4. **文档同步**：API文档与代码同步更新

### 1.2 Quick Start (5分钟)

**基本用法:**
```
> 开始实现任务清单中的任务
按照doc\tasks.md此清单进行编码实施，实施前请认真阅读doc\compile_test_problem_summary.md,从中吸取经验教训，不要再犯同样的问题；
```

**指定语言:**
```
> 用Python实现用户认证模块，要求覆盖率>80%
```

**That's it!** 系统会：
1. 分析任务清单，确定优先级
2. 搭建测试环境
3. 编写测试用例（红）
4. 实现代码（绿）
5. 重构优化
6. 验证覆盖率>80%

**快速输出示例:**
```
## 实现进度

### ✅ T1: 用户模型 (已完成)
- 测试用例: 5个 ✅
- 代码覆盖率: 92%
- 文档: 已更新

### 🔄 T2: 认证服务 (进行中)
- 测试用例: 3/8个
- 当前状态: 编写实现代码

### 📋 T3: API端点 (待开始)
- 依赖: T2
```

### 1.3 性能预期

| 项目规模 | 文件数 | 实施时间 | 测试覆盖率 |
|----------|--------|----------|------------|
| 小型 (<20文件) | 10-20 | 10-20分钟 | >85% |
| 中型 (20-50文件) | 20-50 | 20-40分钟 | >80% |
| 大型 (50-100文件) | 50-100 | 40-90分钟 | >80% |
| 企业级 (>100文件) | 100+ | 90-180分钟 | >75% |

**性能影响因素:**
- 语言复杂度: C++ > Java > Python
- 测试框架: Google Test配置较复杂
- 依赖数量: 外部依赖多会增加时间
- 代码复杂度: 算法密集型代码需要更多测试

**优化建议:**
- 先实现核心框架，再扩展功能
- 使用Mock隔离外部依赖
- 并行实现无依赖的任务

### 1.4 触发条件（重要）

**⚠️ 当用户请求符合以下任一模式时，必须立即调用本Skill：**

#### 1.4.1 中文触发模式

| 触发关键词 | 示例请求 |
|------------|----------|
| 编码实施 | "按照tasks.md清单进行编码实施" |
| 任务清单 | "继续按照任务清单实施" |
| tasks.md | "按照tasks.md清单实施" |
| 按照清单 | "按照清单继续实施" |
| 继续实施 | "继续实施下一个任务" |
| 里程碑 | "完成下一个里程碑" |
| 实现任务 | "实现任务清单中的任务" |
| 编码 | "开始编码" |
| 实现 | "实现xxx功能" |
| 开发 | "开始开发" |

#### 1.4.2 英文触发模式

| Trigger Keywords | Example Request |
|------------------|-----------------|
| implement tasks | "implement tasks from the list" |
| follow tasks.md | "follow tasks.md plan" |
| start coding | "start coding phase" |
| task list | "implement according to task list" |

#### 1.4.3 触发判断流程

```
用户输入
    │
    ▼
┌─────────────────────────────────────┐
│ 包含"编码实施"、"任务清单"、        │
│ "tasks.md"、"按照清单"等关键词？    │
├─────────────────────────────────────┤
│    YES → 立即调用code-implementer   │
│    NO  → 继续检查其他模式           │
└─────────────────────────────────────┘
```

#### 1.4.4 触发示例

**应该触发的请求：**
- "按照code\graph\doc\tasks.md清单进行编码实施"
- "继续按照tasks.md清单实施"
- "实施下一个任务"
- "完成里程碑M1的任务"
- "开始编码实现"

**不应触发的请求：**
- "查看任务清单"（只是查看，不是实施）
- "任务清单是什么"（询问性质）
- "帮我规划任务"（应触发task-planner）

### 1.5 核心约束（必读）

**❌ 不要做的事**：
- 不要跳过测试直接编写实现代码
- 不要在测试未通过时提交代码
- 不要忽略代码覆盖率要求
- 不要忘记更新API文档
- 不要跳过代码审查环节
- **不要忘记在每个任务完成后更新tasks.md状态**
- **不要忘记在里程碑完成后更新tasks.md汇总状态**

### 1.6 自动行为（重要）

**⚠️ 以下行为必须在编码实施过程中自动执行，无需用户额外请求：**

1. **任务状态自动更新**：每完成一个任务，立即更新tasks.md中的任务状态
2. **里程碑状态自动更新**：每完成一个里程碑，立即更新tasks.md中的里程碑状态和汇总统计
3. **进度统计自动更新**：同步更新Overview中的完成数、剩余数、进度百分比

**详细流程见6.3节和6.4节。**

### 1.7 与其他Skill的关系

```
┌─────────────────────────────────────────────────────────────┐
│                    Skill协作关系                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  [设计文档] → design-doc-validator → [检查清单]              │
│                    ↓                                        │
│              task-planner → [任务计划]                       │
│                    ↓                                        │
│           code-implementer → [代码实现]                      │
│                    ↓                                        │
│           technical-reviewer → [技术评审]                    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 1.8 Skill边界说明

**Skill选择决策树**：

```
用户需求: "评审xxx"
       │
       ▼
┌─────────────────────────────────────┐
│ 需要多个专家视角？                   │
├─────────────────────────────────────┤
│    YES → multi-role-reviewer        │
│    NO  → 需要编码实施？              │
│           YES → code-implementer    │
│           NO  → technical-reviewer  │
└─────────────────────────────────────┘
```

**边界说明表**：

| 场景 | 使用Skill | 原因 |
|------|-----------|------|
| 设计文档质量检查 | design-doc-validator | 文档验证阶段 |
| 任务分解规划 | task-planner | 任务规划阶段 |
| 编码实施 | code-implementer | 当前Skill |
| 单一专家评审 | technical-reviewer | 评审阶段 |
| 多角度交叉评审 | multi-role-reviewer | 需要投票机制 |

### 1.9 执行资源估算

| 项目类型 | 预估Token | 预估时间 |
|----------|-----------|----------|
| Python小型项目 | 5,000-10,000 | 5-10分钟 |
| C++中型项目 | 10,000-20,000 | 10-20分钟 |
| Java大型项目 | 15,000-30,000 | 15-30分钟 |

---

## 二、执行流程

### 2.1 总体流程图

```
┌─────────────────────────────────────────────────────────────┐
│                  Code Implementer 流程                       │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Step 0: 环境准备与验证                                      │
│  ├── 检查开发环境                                           │
│  ├── 验证依赖安装                                           │
│  └── 确认任务清单                                           │
│                                                             │
│  Step 1: 任务分析与优先级                                    │
│  ├── 识别核心框架任务                                       │
│  ├── 确定任务依赖顺序                                       │
│  └── 制定实施计划                                           │
│                                                             │
│  Step 2: 测试先行（TDD）                                     │
│  ├── 编写单元测试用例                                       │
│  ├── 运行测试（预期失败）                                    │
│  └── 编写最小实现代码                                       │
│                                                             │
│  Step 3: 编码实施                                           │
│  ├── 实现功能代码                                           │
│  ├── 运行测试（预期通过）                                    │
│  └── 重构优化代码                                           │
│                                                             │
│  Step 4: 测试验证                                           │
│  ├── 运行全部测试                                           │
│  ├── 检查覆盖率（目标>80%）                                  │
│  └── 修复失败的测试                                         │
│                                                             │
│  Step 5: 文档更新                                           │
│  ├── 更新API文档                                            │
│  ├── 添加使用示例                                           │
│  └── 更新README                                             │
│                                                             │
│  Step 6: 代码提交                                           │
│  ├── 代码审查（自审/他审）                                   │
│  ├── 提交代码                                               │
│  └── 更新任务状态                                           │
│                                                             │
│  Step 7: 迭代与总结                                         │
│  ├── 回顾实施过程                                           │
│  ├── 记录经验教训                                           │
│  └── 准备下一任务                                           │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## Step 0: 环境准备与验证

### 0.1 开发环境检查

**Python工程检查项**：

```bash
# Python版本
python --version  # >= 3.8

# 包管理器
pip --version
poetry --version  # 或 pipenv --version

# 测试框架
pytest --version
pytest-cov --version  # 覆盖率插件
```

**C++工程检查项**：

```bash
# 编译器版本
gcc --version   # >= 9.0 (Linux)
cl.exe          # MSVC 2019+ (Windows)

# 构建系统
cmake --version  # >= 3.16

# 测试框架
# Google Test 需要安装或作为子模块
```

**Java工程检查项**：

```bash
# JDK版本
java -version   # >= 17 (推荐 LTS)
javac -version

# 构建工具
mvn --version   # Maven >= 3.8
# 或
gradle --version  # Gradle >= 7.0

# 测试框架
# JUnit 5、Mockito、AssertJ 通过依赖管理
```

### 0.2 依赖验证

**Python工程**：

```bash
# 安装依赖
pip install -r requirements.txt
# 或
poetry install

# 验证关键依赖
python -c "import pytest; print(pytest.__version__)"
python -c "import fastapi; print(fastapi.__version__)"  # 如使用FastAPI
```

**C++工程**：

```bash
# 配置项目
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# 验证依赖
cmake --build build --target verify_deps  # 如有此目标
```

**Java工程**：

```bash
# Maven
mvn dependency:resolve
mvn dependency:tree

# 验证关键依赖
mvn test -Dtest=DependencyCheckTest  # 如有此测试

# Gradle
gradle dependencies
gradle check
```

### 0.3 任务清单确认

确认 `TASKS.md` 文件存在且格式正确：

```markdown
## 任务清单

### 里程碑 1: 核心框架
- [ ] 任务1.1: 基础数据结构
  - 描述: ...
  - 验收标准: ...
  - 预估工时: ...
```

---

## Step 1: 任务分析与优先级

### 1.1 核心框架识别

识别"核心框架先行"策略中的核心任务：

| 优先级 | 任务类型 | 说明 |
|--------|----------|------|
| P0 | 基础设施 | 项目结构、构建系统、公共定义 |
| P0 | 核心接口 | 抽象基类、接口定义 |
| P1 | 核心实现 | 关键功能的实现 |
| P2 | 扩展功能 | 可选功能、优化 |
| P3 | 辅助工具 | 脚本、工具类 |

### 1.2 依赖顺序分析

```
依赖关系示例：

[公共定义] → [基础接口] → [核心实现]
     ↓              ↓
[错误处理] → [日志系统] → [测试框架]
```

### 1.3 设计文档查找

**重要**：在开始实施之前，必须查找并阅读对应的设计文档：

1. **查找设计文档**：根据任务ID对应的Design Doc列，查找对应的设计文档
   - 任务清单中记录了每个任务对应的设计文档
   - 设计文档路径通常在 `doc/` 或 `design/` 目录下

2. **阅读设计文档**：
   - 仔细阅读设计文档中的功能描述
   - 理解接口定义和数据结构
   - 确认技术实现细节

3. **以设计文档为准**：
   - 实施时以设计文档中的描述为准
   - 如有疑问请查阅设计文档相关内容
   - 不要自行决定与设计文档不一致的实现方式

```markdown
示例：
任务 T4.1 - DE-9IM空间关系判断
  → 对应设计文档: geometry_design.md
  → 实施前应先阅读 geometry_design.md 中关于空间关系的描述
```

### 1.4 实施计划模板

```markdown
## 实施计划

### 第一阶段：核心框架（预计 X 天）
1. 任务A（依赖：无）
2. 任务B（依赖：任务A）
3. 任务C（依赖：任务A）

### 第二阶段：核心功能（预计 Y 天）
1. 任务D（依赖：任务B, 任务C）
2. 任务E（依赖：任务D）

### 第三阶段：扩展功能（预计 Z 天）
...
```

---

## Step 2: 测试先行（TDD）

### 2.1 TDD 红绿重构循环

```
┌─────────────────────────────────────────┐
│           TDD 循环                        │
│                                          │
│    ┌─────────┐                          │
│    │  🔴 RED  │ ← 编写失败的测试          │
│    └────┬────┘                          │
│         │                               │
│         ▼                               │
│    ┌─────────┐                          │
│    │ 🟢 GREEN │ ← 编写最小实现代码        │
│    └────┬────┘                          │
│         │                               │
│         ▼                               │
│    ┌─────────┐                          │
│    │♻️ REFACTOR│ ← 重构优化代码           │
│    └────┬────┘                          │
│         │                               │
│         └──────────────── 循环 ────────→│
│                                          │
└─────────────────────────────────────────┘
```

### 2.2 Python 测试模板

**测试文件结构**：

```
tests/
├── __init__.py
├── conftest.py          # pytest fixtures
├── unit/
│   ├── test_module1.py
│   └── test_module2.py
├── integration/
│   └── test_integration.py
└── fixtures/
    └── test_data.json
```

**单元测试模板**：

```python
import pytest
from module import ClassName


class TestClassName:
    """ClassName 单元测试"""
    
    @pytest.fixture
    def instance(self):
        """测试实例"""
        return ClassName()
    
    def test_method_success(self, instance):
        """测试正常情况"""
        result = instance.method("input")
        assert result == "expected"
    
    def test_method_invalid_input(self, instance):
        """测试无效输入"""
        with pytest.raises(ValueError):
            instance.method(None)
    
    @pytest.mark.parametrize("input,expected", [
        ("case1", "result1"),
        ("case2", "result2"),
        ("case3", "result3"),
    ])
    def test_method_cases(self, instance, input, expected):
        """参数化测试"""
        assert instance.method(input) == expected
```

**运行测试**：

```bash
# 运行所有测试
pytest

# 运行特定测试文件
pytest tests/unit/test_module.py

# 运行特定测试类
pytest tests/unit/test_module.py::TestClassName

# 运行特定测试方法
pytest tests/unit/test_module.py::TestClassName::test_method_success

# 显示覆盖率
pytest --cov=src --cov-report=html --cov-report=term

# 覆盖率目标 >80%
pytest --cov=src --cov-fail-under=80
```

### 2.3 C++ 测试模板

**测试文件结构**：

```
tests/
├── CMakeLists.txt
├── test_main.cpp        # 测试入口
├── unit/
│   ├── test_class1.cpp
│   └── test_class2.cpp
├── integration/
│   └── test_integration.cpp
└── mocks/
    └── mock_interface.h
```

**单元测试模板**：

```cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "module/class_name.h"

using namespace testing;

class ClassNameTest : public Test {
protected:
    void SetUp() override {
        instance_ = std::make_unique<ClassName>();
    }
    
    void TearDown() override {
        instance_.reset();
    }
    
    std::unique_ptr<ClassName> instance_;
};

TEST_F(ClassNameTest, Method_Success) {
    auto result = instance_->Method("input");
    EXPECT_EQ(result, "expected");
}

TEST_F(ClassNameTest, Method_InvalidInput) {
    EXPECT_THROW(instance_->Method(nullptr), std::invalid_argument);
}

TEST_F(ClassNameTest, Method_EdgeCase) {
    auto result = instance_->Method("");
    EXPECT_TRUE(result.empty());
}

TEST_P(ClassNameParamTest, Method_Parameters) {
    struct Param {
        std::string input;
        std::string expected;
    };
    
    auto param = GetParam();
    auto result = instance_->Method(param.input);
    EXPECT_EQ(result, param.expected);
}

INSTANTIATE_TEST_SUITE_P(
    MethodCases,
    ClassNameParamTest,
    Values(
        ClassNameParamTest::Param{"case1", "result1"},
        ClassNameParamTest::Param{"case2", "result2"},
        ClassNameParamTest::Param{"case3", "result3"}
    )
);
```

**Mock 模板**：

```cpp
#include <gmock/gmock.h>

class MockInterface : public Interface {
public:
    MOCK_METHOD(void, Method, (const std::string&), (override));
    MOCK_METHOD(std::string, Query, (int id), (override));
    MOCK_METHOD(bool, Validate, (), (const, override));
};

// 使用 Mock
TEST_F(ClassNameTest, UsesMock) {
    MockInterface mock;
    EXPECT_CALL(mock, Method("expected"))
        .Times(1);
    
    instance_->SetInterface(&mock);
    instance_->DoSomething();
}
```

**运行测试**：

```bash
# 构建测试
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build

# 运行所有测试
cd build && ctest --output-on-failure

# 运行特定测试
./build/tests/unit/test_class_name

# 使用 Google Test 过滤
./build/tests/unit/test_class_name --gtest_filter=ClassNameTest.Method*

# 生成覆盖率报告（需要 gcov/lcov）
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
cmake --build build
cd build && ctest
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_report
```

### 2.4 Java 测试模板

**测试文件结构**：

```
src/
├── main/
│   └── java/
│       └── com/example/
│           ├── MyClass.java
│           └── MyService.java
└── test/
    └── java/
        └── com/example/
            ├── MyClassTest.java
            ├── MyServiceTest.java
            └── integration/
                └── MyIntegrationTest.java
```

**Maven 依赖配置** (pom.xml)：

```xml
<dependencies>
    <!-- JUnit 5 -->
    <dependency>
        <groupId>org.junit.jupiter</groupId>
        <artifactId>junit-jupiter</artifactId>
        <version>5.10.0</version>
        <scope>test</scope>
    </dependency>
    
    <!-- Mockito -->
    <dependency>
        <groupId>org.mockito</groupId>
        <artifactId>mockito-core</artifactId>
        <version>5.5.0</version>
        <scope>test</scope>
    </dependency>
    <dependency>
        <groupId>org.mockito</groupId>
        <artifactId>mockito-junit-jupiter</artifactId>
        <version>5.5.0</version>
        <scope>test</scope>
    </dependency>
    
    <!-- AssertJ -->
    <dependency>
        <groupId>org.assertj</groupId>
        <artifactId>assertj-core</artifactId>
        <version>3.24.2</version>
        <scope>test</scope>
    </dependency>
</dependencies>

<build>
    <plugins>
        <!-- Surefire for running tests -->
        <plugin>
            <groupId>org.apache.maven.plugins</groupId>
            <artifactId>maven-surefire-plugin</artifactId>
            <version>3.1.2</version>
        </plugin>
        
        <!-- JaCoCo for coverage -->
        <plugin>
            <groupId>org.jacoco</groupId>
            <artifactId>jacoco-maven-plugin</artifactId>
            <version>0.8.10</version>
            <executions>
                <execution>
                    <goals>
                        <goal>prepare-agent</goal>
                    </goals>
                </execution>
                <execution>
                    <id>report</id>
                    <phase>test</phase>
                    <goals>
                        <goal>report</goal>
                    </goals>
                </execution>
            </executions>
        </plugin>
    </plugins>
</build>
```

**Gradle 依赖配置** (build.gradle)：

```groovy
dependencies {
    testImplementation 'org.junit.jupiter:junit-jupiter:5.10.0'
    testImplementation 'org.mockito:mockito-core:5.5.0'
    testImplementation 'org.mockito:mockito-junit-jupiter:5.5.0'
    testImplementation 'org.assertj:assertj-core:3.24.2'
}

test {
    useJUnitPlatform()
    finalizedBy jacocoTestReport
}

jacoco {
    toolVersion = "0.8.10"
}

jacocoTestReport {
    reports {
        xml.required = true
        html.required = true
    }
}
```

**单元测试模板**：

```java
package com.example;

import org.junit.jupiter.api.*;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.CsvSource;
import org.junit.jupiter.params.provider.ValueSource;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.junit.jupiter.MockitoExtension;
import org.junit.jupiter.api.extension.ExtendWith;

import static org.assertj.core.api.Assertions.*;
import static org.mockito.ArgumentMatchers.*;
import static org.mockito.Mockito.*;

@DisplayName("MyClass 单元测试")
@ExtendWith(MockitoExtension.class)
class MyClassTest {

    @Mock
    private DependencyService dependencyService;
    
    @InjectMocks
    private MyClass myClass;
    
    @BeforeEach
    void setUp() {
        // 初始化测试数据
    }
    
    @AfterEach
    void tearDown() {
        // 清理测试数据
    }
    
    @Test
    @DisplayName("方法正常执行测试")
    void method_shouldReturnExpectedResult_whenValidInput() {
        // Given
        String input = "test";
        String expected = "expected";
        when(dependencyService.process(anyString())).thenReturn("processed");
        
        // When
        String result = myClass.method(input);
        
        // Then
        assertThat(result).isEqualTo(expected);
        verify(dependencyService, times(1)).process(input);
    }
    
    @Test
    @DisplayName("方法异常处理测试")
    void method_shouldThrowException_whenInvalidInput() {
        // Given
        String invalidInput = null;
        
        // When & Then
        assertThatThrownBy(() -> myClass.method(invalidInput))
            .isInstanceOf(IllegalArgumentException.class)
            .hasMessageContaining("Input cannot be null");
    }
    
    @ParameterizedTest
    @DisplayName("参数化测试")
    @CsvSource({
        "input1, result1",
        "input2, result2",
        "input3, result3"
    })
    void method_shouldReturnCorrectResult_forMultipleInputs(String input, String expected) {
        String result = myClass.method(input);
        assertThat(result).isEqualTo(expected);
    }
    
    @ParameterizedTest
    @DisplayName("空值和边界测试")
    @ValueSource(strings = {"", "  ", "a"})
    void method_shouldHandleEdgeCases(String input) {
        assertThatCode(() -> myClass.method(input))
            .doesNotThrowAnyException();
    }
    
    @Test
    @DisplayName("AssertJ 流式断言示例")
    void assertJ_examples() {
        // 字符串断言
        assertThat("Hello World")
            .isNotEmpty()
            .startsWith("Hello")
            .endsWith("World")
            .hasSize(11);
        
        // 集合断言
        assertThat(List.of("a", "b", "c"))
            .hasSize(3)
            .contains("a", "b")
            .doesNotContain("d");
        
        // 对象断言
        MyClass obj = new MyClass();
        assertThat(obj)
            .isNotNull()
            .extracting("field1", "field2")
            .containsExactly("value1", "value2");
        
        // 异常断言
        assertThatThrownBy(() -> { throw new RuntimeException("error"); })
            .isInstanceOf(RuntimeException.class)
            .hasMessage("error");
    }
}
```

**Mock 模板**：

```java
package com.example;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.mockito.Mock;
import org.mockito.junit.jupiter.MockitoExtension;

import static org.assertj.core.api.Assertions.*;
import static org.mockito.ArgumentMatchers.*;
import static org.mockito.Mockito.*;

@ExtendWith(MockitoExtension.class)
class MockExampleTest {

    @Mock
    private Repository repository;
    
    @Mock
    private EventPublisher eventPublisher;
    
    @Test
    void shouldUseMockito() {
        // Given - 配置 Mock 行为
        when(repository.findById(1L)).thenReturn(Optional.of(new Entity(1L, "test")));
        when(repository.save(any(Entity.class))).thenAnswer(invocation -> {
            Entity e = invocation.getArgument(0);
            e.setId(100L);
            return e;
        });
        doNothing().when(eventPublisher).publish(any());
        
        // When - 执行测试
        Optional<Entity> found = repository.findById(1L);
        
        // Then - 验证结果
        assertThat(found).isPresent();
        assertThat(found.get().getName()).isEqualTo("test");
        
        // 验证 Mock 调用
        verify(repository).findById(1L);
        verify(repository, never()).deleteById(anyLong());
        verify(repository, times(1)).findById(anyLong());
        
        // 验证调用顺序
        InOrder inOrder = inOrder(repository, eventPublisher);
        inOrder.verify(repository).findById(1L);
        inOrder.verify(eventPublisher).publish(any());
    }
    
    @Test
    void shouldSpyOnRealObject() {
        // Spy - 部分模拟真实对象
        List<String> realList = new ArrayList<>();
        List<String> spyList = spy(realList);
        
        when(spyList.size()).thenReturn(100);
        spyList.add("test");
        
        assertThat(spyList.get(0)).isEqualTo("test");  // 真实方法
        assertThat(spyList.size()).isEqualTo(100);      // Mock 方法
    }
    
    @Test
    void shouldCaptureArguments() {
        // 参数捕获
        ArgumentCaptor<Entity> captor = ArgumentCaptor.forClass(Entity.class);
        
        repository.save(new Entity(1L, "test"));
        
        verify(repository).save(captor.capture());
        Entity captured = captor.getValue();
        
        assertThat(captured.getId()).isEqualTo(1L);
        assertThat(captured.getName()).isEqualTo("test");
    }
}
```

**集成测试模板**：

```java
package com.example.integration;

import org.junit.jupiter.api.*;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.test.context.ActiveProfiles;
import org.testcontainers.containers.PostgreSQLContainer;
import org.testcontainers.junit.jupiter.Container;
import org.testcontainers.junit.jupiter.Testcontainers;

import static org.assertj.core.api.Assertions.*;

@SpringBootTest
@Testcontainers
@ActiveProfiles("test")
class MyIntegrationTest {

    @Container
    static PostgreSQLContainer<?> postgres = new PostgreSQLContainer<>("postgres:15")
        .withDatabaseName("testdb");
    
    @Autowired
    private MyRepository repository;
    
    @Autowired
    private MyService service;
    
    @BeforeAll
    static void beforeAll() {
        // 全局初始化
    }
    
    @Test
    void shouldPersistAndRetrieve() {
        // Given
        Entity entity = new Entity(null, "test");
        
        // When
        Entity saved = repository.save(entity);
        Entity found = repository.findById(saved.getId()).orElseThrow();
        
        // Then
        assertThat(found.getName()).isEqualTo("test");
    }
}
```

**运行测试**：

```bash
# Maven
# 运行所有测试
mvn test

# 运行特定测试类
mvn test -Dtest=MyClassTest

# 运行特定测试方法
mvn test -Dtest=MyClassTest#method_shouldReturnExpectedResult_whenValidInput

# 运行并生成覆盖率报告
mvn test jacoco:report

# 查看覆盖率报告
# 打开 target/site/jacoco/index.html

# 覆盖率检查（可在 pom.xml 配置规则）
mvn verify

# Gradle
# 运行所有测试
gradle test

# 运行特定测试类
gradle test --tests "com.example.MyClassTest"

# 运行并生成覆盖率报告
gradle test jacocoTestReport

# 查看覆盖率报告
# 打开 build/reports/jacoco/test/html/index.html
```

---

## Step 3: 编码实施

### 3.1 编码规范

**Python 编码规范**：

- 遵循 PEP 8 风格指南
- 使用 Black 格式化代码
- 使用 isort 排序导入
- 使用 mypy 类型检查
- 使用 pylint/flake8 静态分析

```bash
# 代码格式化
black src tests
isort src tests

# 类型检查
mypy src

# 静态分析
pylint src
flake8 src
```

**C++ 编码规范**：

- 遵循 Google C++ Style Guide 或项目约定
- 使用 clang-format 格式化代码
- 使用 clang-tidy 静态分析
- 使用 cppcheck 静态分析

```bash
# 代码格式化
clang-format -i src/*.cpp include/**/*.h

# 静态分析
clang-tidy src/*.cpp -- -I include
cppcheck --enable=all src/
```

**Java 编码规范**：

- 遵循 Google Java Style Guide 或项目约定
- 使用 Checkstyle 检查代码风格
- 使用 SpotBugs 静态分析
- 使用 PMD 代码质量检查

```bash
# Maven 代码检查
mvn checkstyle:check    # Checkstyle
mvn spotbugs:check      # SpotBugs
mvn pmd:check           # PMD

# Gradle 代码检查
gradle checkstyleMain   # Checkstyle
gradle spotbugsMain     # SpotBugs
gradle pmdMain          # PMD

# 格式化（使用 spotless 插件）
mvn spotless:apply      # Maven
gradle spotlessApply    # Gradle
```

### 3.2 代码审查清单

**提交前自查**：

| 检查项 | 说明 | 状态 |
|--------|------|------|
| 编译/运行 | 代码可正常编译/运行 | ☐ |
| 测试通过 | 所有测试通过 | ☐ |
| 覆盖率 | 覆盖率 >80% | ☐ |
| 代码风格 | 符合编码规范 | ☐ |
| 无警告 | 编译/运行无警告 | ☐ |
| 文档更新 | API文档已更新 | ☐ |
| 注释完整 | 关键代码有注释 | ☐ |
| 无硬编码 | 无敏感信息硬编码 | ☐ |

### 3.3 提交信息规范

```
<type>(<scope>): <subject>

<body>

<footer>

类型(type):
- feat: 新功能
- fix: 修复bug
- docs: 文档更新
- style: 代码格式（不影响功能）
- refactor: 重构
- test: 测试相关
- chore: 构建/工具相关

示例:
feat(database): 添加PostGIS连接池支持

- 实现连接池管理器
- 添加连接健康检查
- 支持最大连接数配置

Closes #123
```

---

## Step 4: 测试验证

### 4.1 测试类型

| 测试类型 | 说明 | 覆盖目标 |
|----------|------|----------|
| 单元测试 | 测试单个函数/方法 | >80% |
| 集成测试 | 测试模块间交互 | 关键路径 |
| 端到端测试 | 测试完整流程 | 核心功能 |
| 性能测试 | 测试性能指标 | 关键操作 |

### 4.2 覆盖率要求

```
覆盖率目标：

┌─────────────────────────────────────────┐
│         代码覆盖率要求                    │
├─────────────────────────────────────────┤
│                                          │
│  行覆盖率 (Line Coverage)    >= 80%      │
│  分支覆盖率 (Branch Coverage) >= 70%     │
│  函数覆盖率 (Function Coverage) >= 90%   │
│                                          │
│  核心模块要求更高：                       │
│  - 行覆盖率 >= 90%                       │
│  - 分支覆盖率 >= 80%                     │
│                                          │
└─────────────────────────────────────────┘
```

### 4.3 测试失败处理

```markdown
## 测试失败处理流程

1. 记录失败信息
   - 测试名称
   - 错误信息
   - 堆栈跟踪

2. 分析失败原因
   - 代码bug
   - 测试用例问题
   - 环境问题

3. 修复问题
   - 修复代码bug
   - 或更新测试用例（需确认需求）

4. 重新运行测试
   - 运行失败的测试
   - 运行全部测试（确保无回归）

5. 记录修复过程
   - 更新问题记录
   - 提交修复代码
```

---

## Step 5: 文档更新

### 5.1 Python API 文档（FastAPI + Swagger）

**FastAPI 自动生成 Swagger**：

```python
from fastapi import FastAPI, HTTPException
from pydantic import BaseModel

app = FastAPI(
    title="API名称",
    description="API描述",
    version="1.0.0",
    docs_url="/docs",
    redoc_url="/redoc"
)

class Item(BaseModel):
    """物品模型"""
    id: int
    name: str
    description: str | None = None

@app.get("/items/{item_id}", response_model=Item, 
         summary="获取物品", 
         description="根据ID获取物品详情")
async def get_item(item_id: int):
    """
    获取物品详情
    
    - **item_id**: 物品ID
    """
    # 实现代码
    pass
```

**访问文档**：
- Swagger UI: `http://localhost:8000/docs`
- ReDoc: `http://localhost:8000/redoc`

### 5.2 C++ API 文档（Sphinx + Breathe）

**项目结构**：

```
docs/
├── conf.py
├── index.rst
├── api/
│   └── index.rst
└── examples/
    └── usage.rst
```

**conf.py 配置**：

```python
extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.viewcode',
    'breathe',
    'exhale'
]

breathe_projects = {
    "myproject": "./_build/xml/"
}
breathe_default_project = "myproject"

exhale_args = {
    "containmentFolder": "./api",
    "rootFileName": "library_root.rst",
    "rootFileTitle": "Library API",
    "doxygenStripFromPath": ".."
}
```

**Doxygen 配置** (Doxyfile)：

```
PROJECT_NAME = "MyProject"
INPUT = ../include
OUTPUT_DIRECTORY = ./docs/_build
GENERATE_XML = YES
EXTRACT_ALL = YES
```

**构建文档**：

```bash
# 生成 Doxygen XML
doxygen Doxyfile

# 构建 Sphinx 文档
cd docs
make html
```

### 5.3 使用示例模板

**Python 示例** (`docs/examples/usage.md`)：

```markdown
# 使用示例

## 基本用法

\`\`\`python
from mymodule import ClassName

# 创建实例
instance = ClassName()

# 调用方法
result = instance.method("input")
print(result)  # 输出: expected
\`\`\`

## 高级用法

\`\`\`python
# 配置选项
instance = ClassName(
    option1="value1",
    option2="value2"
)

# 错误处理
try:
    instance.method(None)
except ValueError as e:
    print(f"错误: {e}")
\`\`\`
```

**C++ 示例** (`docs/examples/usage.rst`)：

```rst
使用示例
========

基本用法
--------

.. code-block:: cpp

    #include <mymodule/class_name.h>
    
    int main() {
        // 创建实例
        auto instance = std::make_unique<ClassName>();
        
        // 调用方法
        auto result = instance->Method("input");
        std::cout << result << std::endl;  // 输出: expected
        
        return 0;
    }

高级用法
--------

.. code-block:: cpp

    // 配置选项
    ClassName::Options options;
    options.option1 = "value1";
    options.option2 = "value2";
    
    auto instance = std::make_unique<ClassName>(options);
    
    // 错误处理
    try {
        instance->Method(nullptr);
    } catch (const std::invalid_argument& e) {
        std::cerr << "错误: " << e.what() << std::endl;
    }
```

### 5.4 Java API 文档（Javadoc）

**Javadoc 注释规范**：

```java
package com.example;

/**
 * 用户服务类，提供用户相关的业务操作。
 * 
 * <p>该服务类封装了用户的增删改查操作，支持事务管理和缓存。</p>
 * 
 * <h2>使用示例</h2>
 * <pre>{@code
 * UserService service = new UserService(userRepository);
 * User user = service.findById(1L);
 * }</pre>
 * 
 * <h2>线程安全</h2>
 * <p>该类是线程安全的，可以在多线程环境中使用。</p>
 * 
 * @author 开发者姓名
 * @version 1.0.0
 * @since 1.0.0
 * @see User
 * @see UserRepository
 */
public class UserService {
    
    /**
     * 用户仓库实例。
     */
    private final UserRepository userRepository;
    
    /**
     * 构造函数。
     * 
     * @param userRepository 用户仓库，不能为 null
     * @throws IllegalArgumentException 如果 userRepository 为 null
     */
    public UserService(UserRepository userRepository) {
        this.userRepository = Objects.requireNonNull(userRepository, "userRepository cannot be null");
    }
    
    /**
     * 根据ID查找用户。
     * 
     * <p>如果找到用户，返回包含用户的 Optional；否则返回空的 Optional。</p>
     * 
     * @param id 用户ID，必须为正数
     * @return 包含用户的 Optional，不会返回 null
     * @throws IllegalArgumentException 如果 id 不是正数
     * @see Optional
     */
    public Optional<User> findById(Long id) {
        if (id == null || id <= 0) {
            throw new IllegalArgumentException("id must be positive");
        }
        return userRepository.findById(id);
    }
    
    /**
     * 创建新用户。
     * 
     * @param request 创建用户请求，不能为 null
     * @return 创建的用户对象
     * @throws IllegalArgumentException 如果 request 为 null 或包含无效数据
     * @throws DuplicateUserException 如果用户名已存在
     */
    public User create(CreateUserRequest request) {
        // 实现代码
    }
    
    /**
     * 分页查询用户列表。
     * 
     * @param query 查询条件，可以为 null（查询全部）
     * @param pageable 分页参数，不能为 null
     * @return 分页结果，不会返回 null
     */
    public Page<User> findByQuery(UserQuery query, Pageable pageable) {
        // 实现代码
    }
}
```

**Maven Javadoc 配置** (pom.xml)：

```xml
<build>
    <plugins>
        <plugin>
            <groupId>org.apache.maven.plugins</groupId>
            <artifactId>maven-javadoc-plugin</artifactId>
            <version>3.6.0</version>
            <configuration>
                <encoding>UTF-8</encoding>
                <docencoding>UTF-8</docencoding>
                <charset>UTF-8</charset>
                <show>protected</show>
                <nohelp>true</nohelp>
                <author>true</author>
                <version>true</version>
                <use>true</use>
                <windowtitle>MyProject API</windowtitle>
                <doctitle>MyProject API Documentation</doctitle>
                <bottom>Copyright © 2026 MyCompany. All Rights Reserved.</bottom>
                <links>
                    <link>https://docs.oracle.com/en/java/javase/17/docs/api/</link>
                </links>
                <tags>
                    <tag>
                        <name>implNote</name>
                        <placement>a</placement>
                        <head>Implementation Note:</head>
                    </tag>
                    <tag>
                        <name>apiNote</name>
                        <placement>a</placement>
                        <head>API Note:</head>
                    </tag>
                </tags>
            </configuration>
            <executions>
                <execution>
                    <id>attach-javadocs</id>
                    <goals>
                        <goal>jar</goal>
                    </goals>
                </execution>
            </executions>
        </plugin>
    </plugins>
</build>
```

**Gradle Javadoc 配置** (build.gradle)：

```groovy
javadoc {
    options {
        encoding = 'UTF-8'
        charSet = 'UTF-8'
        author = true
        version = true
        links = ['https://docs.oracle.com/en/java/javase/17/docs/api/']
        windowTitle = 'MyProject API'
        docTitle = 'MyProject API Documentation'
        bottom = 'Copyright © 2026 MyCompany. All Rights Reserved.'
    }
}

task javadocJar(type: Jar, dependsOn: javadoc) {
    archiveClassifier.set('javadoc')
    from javadoc.destinationDir
}

artifacts {
    archives javadocJar
}
```

**构建 Javadoc**：

```bash
# Maven
mvn javadoc:javadoc           # 生成 Javadoc
mvn javadoc:jar               # 打包 Javadoc JAR

# 查看生成的文档
# 打开 target/site/apidocs/index.html

# Gradle
gradle javadoc                # 生成 Javadoc
gradle javadocJar             # 打包 Javadoc JAR

# 查看生成的文档
# 打开 build/docs/javadoc/index.html
```

**Java 使用示例模板** (`docs/examples/usage.md`)：

```markdown
# 使用示例

## 基本用法

\`\`\`java
import com.example.UserService;
import com.example.User;

// 创建服务实例
UserService service = new UserService(userRepository);

// 查找用户
Optional<User> user = service.findById(1L);
if (user.isPresent()) {
    System.out.println("Found: " + user.get().getName());
}

// 创建用户
CreateUserRequest request = new CreateUserRequest("John", "john@example.com");
User created = service.create(request);
\`\`\`

## 高级用法

\`\`\`java
// 分页查询
UserQuery query = UserQuery.builder()
    .nameContains("John")
    .status(UserStatus.ACTIVE)
    .build();

Pageable pageable = PageRequest.of(0, 20, Sort.by("name").ascending());
Page<User> page = service.findByQuery(query, pageable);

// 处理结果
page.getContent().forEach(user -> {
    System.out.println(user.getName());
});

// 错误处理
try {
    service.create(invalidRequest);
} catch (IllegalArgumentException e) {
    System.err.println("Invalid request: " + e.getMessage());
} catch (DuplicateUserException e) {
    System.err.println("User already exists: " + e.getUsername());
}
\`\`\`
```

---

## Step 6: 代码提交

### 6.1 提交前检查

```bash
# Python 项目
# 1. 运行测试
pytest --cov=src --cov-fail-under=80

# 2. 代码格式化
black src tests
isort src tests

# 3. 类型检查
mypy src

# 4. 静态分析
pylint src

# C++ 项目
# 1. 运行测试
cd build && ctest --output-on-failure

# 2. 代码格式化
clang-format -i src/*.cpp include/**/*.h

# 3. 静态分析
clang-tidy src/*.cpp -- -I include

# Java 项目
# 1. 运行测试
mvn test
# 或
gradle test

# 2. 代码检查
mvn checkstyle:check spotbugs:check pmd:check
# 或
gradle check

# 3. 覆盖率检查
mvn verify
# 或
gradle jacocoTestReport
```

### 6.2 提交流程

```bash
# 1. 查看变更
git status
git diff

# 2. 暂存变更
git add .

# 3. 提交（使用规范信息）
git commit -m "feat(module): 添加XXX功能"

# 4. 推送到远程
git push origin feature/xxx
```

### 6.3 任务状态更新（重要）

**⚠️ 关键步骤：任务完成后必须更新tasks.md中的任务状态！**

#### 6.3.1 任务状态更新流程

```
┌─────────────────────────────────────────────────────────────┐
│              任务状态更新流程                                │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. 验证任务完成                                             │
│     ├── 所有测试通过                                        │
│     ├── 代码覆盖率达标 (>80%)                               │
│     └── 代码审查通过                                        │
│                                                             │
│  2. 更新任务清单文件 (tasks.md)                              │
│     ├── 更新Task Summary表格中的Status列                    │
│     ├── 更新Overview中的完成统计                            │
│     └── 记录完成时间和备注                                  │
│                                                             │
│  3. 验证更新                                                 │
│     └── 确认tasks.md格式正确                                │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

#### 6.3.2 Task Summary表格状态更新

**状态标记说明：**

| 状态 | 标记 | 说明 |
|------|------|------|
| 待开始 | 📋 Todo | 任务尚未开始 |
| 进行中 | 🔄 In Progress | 任务正在实施 |
| 已完成 | ✅ Done | 任务已完成并通过验收 |
| 已阻塞 | 🚫 Blocked | 任务被阻塞，需要解决依赖问题 |
| 已取消 | ❌ Cancelled | 任务已取消 |

**更新示例：**

更新前：
```markdown
| Task ID | Task Name | Priority | Milestone | Effort | Status | Dependencies |
|---------|-----------|----------|-----------|--------|--------|--------------|
| T1.1 | 枚举和常量定义 | P0 | M1 | 12h | 📋 Todo | - |
```

更新后：
```markdown
| Task ID | Task Name | Priority | Milestone | Effort | Status | Dependencies |
|---------|-----------|----------|-----------|--------|--------|--------------|
| T1.1 | 枚举和常量定义 | P0 | M1 | 12h | ✅ Done | - |
```

#### 6.3.3 Overview统计更新

**必须同时更新Overview部分的统计数据：**

更新前：
```markdown
## Overview

| 项目 | 数值 |
|------|------|
| Total Tasks | 28 |
| Total Estimated Hours | 约680小时 (PERT expected) |
```

更新后：
```markdown
## Overview

| 项目 | 数值 |
|------|------|
| Total Tasks | 28 |
| Completed Tasks | 20 (71.4%) |
| Remaining Tasks | 8 (28.6%) |
| Total Estimated Hours | 约680小时 (PERT expected) |
```

#### 6.3.4 任务状态更新检查清单

完成任务后，必须执行以下检查：

```markdown
## 任务状态更新检查清单

- [ ] 1. 确认所有测试通过
- [ ] 2. 确认代码覆盖率达标 (>80%)
- [ ] 3. 更新Task Summary表格中的Status列
- [ ] 4. 更新Overview中的Completed Tasks计数
- [ ] 5. 更新Overview中的Remaining Tasks计数
- [ ] 6. 计算并更新完成百分比
- [ ] 7. 验证tasks.md格式正确
```

#### 6.3.5 完整更新示例

**场景：完成T1.1和T1.2两个任务**

更新tasks.md文件：

```markdown
## Overview

| 项目 | 数值 |
|------|------|
| Total Tasks | 28 |
| Completed Tasks | 2 (7.1%) |
| Remaining Tasks | 26 (92.9%) |
| Total Estimated Hours | 约680小时 (PERT expected) |

## Task Summary

| Task ID | Task Name | Priority | Milestone | Effort | Status | Dependencies |
|---------|-----------|----------|-----------|--------|--------|--------------|
| T1.1 | 枚举和常量定义 | P0 | M1 | 12h | ✅ Done | - |
| T1.2 | 异常类实现 | P0 | M1 | 8h | ✅ Done | T1.1 |
| T1.3 | Coordinate坐标结构 | P0 | M1 | 16h | 📋 Todo | T1.1 |
```

#### 6.3.6 注意事项

**⚠️ 重要提醒：**

1. **必须更新**：任务完成后，必须更新tasks.md中的状态，否则会影响项目进度跟踪
2. **同步更新**：Task Summary表格和Overview统计必须同步更新
3. **百分比计算**：完成百分比 = (已完成任务数 / 总任务数) × 100%
4. **格式一致**：保持tasks.md文件的格式一致性
5. **及时更新**：任务完成后立即更新，不要批量更新

### 6.4 里程碑完成状态更新（重要）

**⚠️ 关键步骤：每当完成一个里程碑，必须据实更新tasks.md中任务汇总单元对应任务状态！**

#### 6.4.1 里程碑完成判断标准

一个里程碑被认为"已完成"需要满足以下条件：

```
┌─────────────────────────────────────────────────────────────┐
│              里程碑完成判断标准                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ✅ 所有任务状态为"✅ Done"                                  │
│  ✅ 所有单元测试通过                                         │
│  ✅ 代码覆盖率达标 (>80%)                                   │
│  ✅ 无编译警告和错误                                         │
│  ✅ 代码审查通过                                             │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

#### 6.4.2 里程碑状态更新流程

```
┌─────────────────────────────────────────────────────────────┐
│              里程碑状态更新流程                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Step 1: 验证里程碑内所有任务完成                            │
│     ├── 检查所有任务状态是否为 ✅ Done                       │
│     ├── 确认所有测试通过                                    │
│     └── 确认覆盖率达标                                      │
│                                                             │
│  Step 2: 更新tasks.md                                       │
│     ├── 更新Milestones章节中对应里程碑的任务状态             │
│     ├── 更新Task Summary表格中所有相关任务状态               │
│     ├── 更新Overview统计（完成数、剩余数、进度百分比）       │
│     └── 更新Critical Path进度（如适用）                      │
│                                                             │
│  Step 3: 记录里程碑完成信息                                  │
│     ├── 记录完成日期                                        │
│     ├── 记录实际工时（与预估对比）                           │
│     └── 记录遇到的问题和解决方案                             │
│                                                             │
│  Step 4: 验证更新                                            │
│     └── 确认tasks.md格式正确、数据一致                       │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

#### 6.4.3 里程碑状态更新示例

**场景：完成M1里程碑（包含T1.1、T1.2、T1.3三个任务）**

更新前（tasks.md）：
```markdown
## Overview

| 项目 | 数值 |
|------|------|
| Total Tasks | 35 |
| Completed Tasks | 0 (0%) |
| Remaining Tasks | 35 (100%) |
| **Current Progress** | **0%** |

## Task Summary

| Task ID | Task Name | Priority | Milestone | Effort | Status | Dependencies |
|---------|-----------|----------|-----------|--------|--------|--------------|
| T1.1 | CNLayer公共定义枚举 | P0 | M1 | 8h | 📋 Todo | - |
| T1.2 | CNLayer抽象基类 | P0 | M1 | 24h | 📋 Todo | T1.1 |
| T1.3 | CNLayer能力测试实现 | P0 | M1 | 8h | 📋 Todo | T1.2 |

### M1: 基础框架 (Week 1-2)

| Task | Priority | Effort | Status | Assignee |
|------|----------|--------|--------|----------|
| T1.1: CNLayer公共定义枚举 | P0 | 8h | 📋 Todo | Dev A |
| T1.2: CNLayer抽象基类 | P0 | 24h | 📋 Todo | Dev A |
| T1.3: CNLayer能力测试实现 | P0 | 8h | 📋 Todo | Dev A |
```

更新后（tasks.md）：
```markdown
## Overview

| 项目 | 数值 |
|------|------|
| Total Tasks | 35 |
| Completed Tasks | 3 (8.6%) |
| Remaining Tasks | 32 (91.4%) |
| **Current Progress** | **8.6%** |

## Task Summary

| Task ID | Task Name | Priority | Milestone | Effort | Status | Dependencies |
|---------|-----------|----------|-----------|--------|--------|--------------|
| T1.1 | CNLayer公共定义枚举 | P0 | M1 | 8h | ✅ Done | - |
| T1.2 | CNLayer抽象基类 | P0 | M1 | 24h | ✅ Done | T1.1 |
| T1.3 | CNLayer能力测试实现 | P0 | M1 | 8h | ✅ Done | T1.2 |

### M1: 基础框架 (Week 1-2) ✅

| Task | Priority | Effort | Status | Assignee |
|------|----------|--------|--------|----------|
| T1.1: CNLayer公共定义枚举 | P0 | 8h | ✅ Done | Dev A |
| T1.2: CNLayer抽象基类 | P0 | 24h | ✅ Done | Dev A |
| T1.3: CNLayer能力测试实现 | P0 | 8h | ✅ Done | Dev A |
```

#### 6.4.4 里程碑完成检查清单

每个里程碑完成后，必须执行以下检查：

```markdown
## 里程碑完成检查清单

### M1: 基础框架

- [ ] 1. 确认所有任务状态已更新为 ✅ Done
- [ ] 2. 确认所有单元测试通过
- [ ] 3. 确认代码覆盖率达标 (>80%)
- [ ] 4. 更新Task Summary表格中的Status列
- [ ] 5. 更新Overview中的Completed Tasks计数
- [ ] 6. 更新Overview中的Remaining Tasks计数
- [ ] 7. 计算并更新Current Progress百分比
- [ ] 8. 在Milestones章节标题添加 ✅ 标记
- [ ] 9. 验证tasks.md格式正确
- [ ] 10. 记录里程碑完成日期和实际工时
```

#### 6.4.5 自动触发条件

**⚠️ 当用户按照任务清单执行时，系统应自动检测里程碑完成状态：**

1. **任务完成时**：检查该任务所属里程碑是否所有任务都已完成
2. **里程碑完成时**：自动更新tasks.md中的相关状态
3. **进度更新时**：同步更新Overview统计和进度百分比

**触发逻辑伪代码：**

```python
def on_task_completed(task_id):
    # 1. 更新单个任务状态
    update_task_status(task_id, "✅ Done")
    
    # 2. 检查所属里程碑是否完成
    milestone = get_milestone_for_task(task_id)
    all_tasks_in_milestone = get_tasks_in_milestone(milestone)
    
    if all(task.status == "✅ Done" for task in all_tasks_in_milestone):
        # 3. 里程碑完成，更新整体状态
        update_milestone_status(milestone, "✅")
        update_overview_statistics()
        log_milestone_completion(milestone)
```

#### 6.4.6 注意事项

**⚠️ 重要提醒：**

1. **实时更新**：每当完成一个任务，立即检查里程碑状态，不要等到所有任务完成
2. **数据一致性**：确保Task Summary、Milestones、Overview三处数据一致
3. **进度可见**：让用户能够清晰看到项目进度
4. **问题记录**：如果里程碑未能按期完成，记录原因和调整计划
5. **文档同步**：里程碑完成后，更新相关设计文档和API文档

---

## Step 7: 迭代与总结

### 7.1 每日回顾

```markdown
## 每日开发日志

### 日期: 2026-03-19

#### 完成任务
- [x] 任务A: 基础接口定义
- [x] 任务B: 单元测试编写

#### 遇到问题
1. 问题: ...
   解决: ...

#### 经验教训
- ...

#### 明日计划
- [ ] 任务C: ...
```

### 7.2 里程碑总结

```markdown
## 里程碑总结

### 里程碑 1: 核心框架

#### 完成情况
- 计划任务: 5
- 完成任务: 5
- 完成率: 100%

#### 代码统计
- 新增代码: 2000 行
- 测试代码: 1500 行
- 测试覆盖率: 85%

#### 遇到的问题
1. ...

#### 改进建议
1. ...
```

---

## 三、负面约束（What NOT to Do）

### ❌ 范围约束

| 约束 | 原因 |
|------|------|
| 不要跳过测试直接编码 | TDD流程保证质量 |
| 不要在测试未通过时提交 | 确保代码质量 |
| 不要忽略覆盖率要求 | 保证测试充分性 |
| 不要忘记更新文档 | 保持文档同步 |

### ❌ 行为约束

| 约束 | 原因 |
|------|------|
| 不要提交调试代码 | 保持代码整洁 |
| 不要硬编码敏感信息 | 安全考虑 |
| 不要跳过代码审查 | 质量保证 |
| 不要忽略警告 | 潜在问题 |

---

## 四、工具配置参考

### 4.1 Python 项目配置

**pyproject.toml**：

```toml
[tool.pytest.ini_options]
testpaths = ["tests"]
python_files = ["test_*.py"]
python_classes = ["Test*"]
python_functions = ["test_*"]
addopts = "-v --cov=src --cov-report=term-missing"

[tool.coverage.run]
source = ["src"]
branch = true
omit = ["tests/*", "*/__pycache__/*"]

[tool.coverage.report]
fail_under = 80
exclude_lines = [
    "pragma: no cover",
    "def __repr__",
    "raise NotImplementedError"
]

[tool.black]
line-length = 88
target-version = ["py38", "py39", "py310"]

[tool.isort]
profile = "black"
line_length = 88

[tool.mypy]
python_version = "3.8"
warn_return_any = true
warn_unused_configs = true
```

### 4.2 C++ 项目配置

**CMakeLists.txt**：

```cmake
# 测试配置
option(BUILD_TESTING "Build tests" ON)
option(ENABLE_COVERAGE "Enable coverage" OFF)

if(BUILD_TESTING)
    enable_testing()
    find_package(GTest REQUIRED)
    
    add_subdirectory(tests)
endif()

if(ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
endif()
```

**.clang-format**：

```yaml
BasedOnStyle: Google
IndentWidth: 4
ColumnLimit: 100
AllowShortFunctionsOnASingleLine: Empty
```

**.clang-tidy**：

```yaml
Checks: '-*,bugprone-*,modernize-*,performance-*,readability-*'
WarningsAsErrors: '*'
```

### 4.3 Java 项目配置

**Maven pom.xml**：

```xml
<?xml version="1.0" encoding="UTF-8"?>
<project>
    <modelVersion>4.0.0</modelVersion>
    
    <groupId>com.example</groupId>
    <artifactId>myproject</artifactId>
    <version>1.0.0-SNAPSHOT</version>
    <packaging>jar</packaging>
    
    <properties>
        <maven.compiler.source>17</maven.compiler.source>
        <maven.compiler.target>17</maven.compiler.target>
        <project.build.sourceEncoding>UTF-8</project.build.sourceEncoding>
        <junit.version>5.10.0</junit.version>
        <mockito.version>5.5.0</mockito.version>
        <assertj.version>3.24.2</assertj.version>
    </properties>
    
    <dependencies>
        <!-- 测试依赖 -->
        <dependency>
            <groupId>org.junit.jupiter</groupId>
            <artifactId>junit-jupiter</artifactId>
            <version>${junit.version}</version>
            <scope>test</scope>
        </dependency>
        <dependency>
            <groupId>org.mockito</groupId>
            <artifactId>mockito-junit-jupiter</artifactId>
            <version>${mockito.version}</version>
            <scope>test</scope>
        </dependency>
        <dependency>
            <groupId>org.assertj</groupId>
            <artifactId>assertj-core</artifactId>
            <version>${assertj.version}</version>
            <scope>test</scope>
        </dependency>
    </dependencies>
    
    <build>
        <plugins>
            <!-- 编译插件 -->
            <plugin>
                <groupId>org.apache.maven.plugins</groupId>
                <artifactId>maven-compiler-plugin</artifactId>
                <version>3.11.0</version>
                <configuration>
                    <source>17</source>
                    <target>17</target>
                    <compilerArgs>
                        <arg>-parameters</arg>
                    </compilerArgs>
                </configuration>
            </plugin>
            
            <!-- 测试插件 -->
            <plugin>
                <groupId>org.apache.maven.plugins</groupId>
                <artifactId>maven-surefire-plugin</artifactId>
                <version>3.1.2</version>
            </plugin>
            
            <!-- 覆盖率插件 -->
            <plugin>
                <groupId>org.jacoco</groupId>
                <artifactId>jacoco-maven-plugin</artifactId>
                <version>0.8.10</version>
                <executions>
                    <execution>
                        <goals>
                            <goal>prepare-agent</goal>
                        </goals>
                    </execution>
                    <execution>
                        <id>report</id>
                        <phase>test</phase>
                        <goals>
                            <goal>report</goal>
                        </goals>
                    </execution>
                    <execution>
                        <id>check</id>
                        <goals>
                            <goal>check</goal>
                        </goals>
                        <configuration>
                            <rules>
                                <rule>
                                    <element>BUNDLE</element>
                                    <limits>
                                        <limit>
                                            <counter>LINE</counter>
                                            <value>COVEREDRATIO</value>
                                            <minimum>0.80</minimum>
                                        </limit>
                                        <limit>
                                            <counter>BRANCH</counter>
                                            <value>COVEREDRATIO</value>
                                            <minimum>0.70</minimum>
                                        </limit>
                                    </limits>
                                </rule>
                            </rules>
                        </configuration>
                    </execution>
                </executions>
            </plugin>
            
            <!-- Checkstyle 插件 -->
            <plugin>
                <groupId>org.apache.maven.plugins</groupId>
                <artifactId>maven-checkstyle-plugin</artifactId>
                <version>3.3.0</version>
                <configuration>
                    <configLocation>google_checks.xml</configLocation>
                    <failOnViolation>true</failOnViolation>
                </configuration>
                <executions>
                    <execution>
                        <phase>validate</phase>
                        <goals>
                            <goal>check</goal>
                        </goals>
                    </execution>
                </executions>
            </plugin>
            
            <!-- SpotBugs 插件 -->
            <plugin>
                <groupId>com.github.spotbugs</groupId>
                <artifactId>spotbugs-maven-plugin</artifactId>
                <version>4.7.3</version>
                <executions>
                    <execution>
                        <phase>verify</phase>
                        <goals>
                            <goal>check</goal>
                        </goals>
                    </execution>
                </executions>
            </plugin>
            
            <!-- PMD 插件 -->
            <plugin>
                <groupId>org.apache.maven.plugins</groupId>
                <artifactId>maven-pmd-plugin</artifactId>
                <version>3.21.0</version>
                <executions>
                    <execution>
                        <phase>verify</phase>
                        <goals>
                            <goal>check</goal>
                        </goals>
                    </execution>
                </executions>
            </plugin>
        </plugins>
    </build>
</project>
```

**Gradle build.gradle**：

```groovy
plugins {
    id 'java'
    id 'jacoco'
    id 'checkstyle'
    id 'pmd'
    id 'com.github.spotbugs' version '5.2.1'
    id 'com.diffplug.spotless' version '6.21.0'
}

group = 'com.example'
version = '1.0.0-SNAPSHOT'

java {
    sourceCompatibility = JavaVersion.VERSION_17
    targetCompatibility = JavaVersion.VERSION_17
}

repositories {
    mavenCentral()
}

dependencies {
    testImplementation 'org.junit.jupiter:junit-jupiter:5.10.0'
    testImplementation 'org.mockito:mockito-junit-jupiter:5.5.0'
    testImplementation 'org.assertj:assertj-core:3.24.2'
}

test {
    useJUnitPlatform()
    finalizedBy jacocoTestReport
}

// JaCoCo 配置
jacoco {
    toolVersion = '0.8.10'
}

jacocoTestReport {
    reports {
        xml.required = true
        html.required = true
    }
}

jacocoTestCoverageVerification {
    violationRules {
        rule {
            limit {
                minimum = 0.80
            }
        }
        rule {
            element = 'CLASS'
            limit {
                counter = 'LINE'
                value = 'COVEREDRATIO'
                minimum = 0.80
            }
        }
    }
}

// Checkstyle 配置
checkstyle {
    toolVersion = '10.12.0'
    configFile = file('config/checkstyle/google_checks.xml')
    ignoreFailures = false
}

// PMD 配置
pmd {
    toolVersion = '6.55.0'
    ignoreFailures = false
    ruleSets = ['category/java/bestpractices.xml', 'category/java/codestyle.xml']
}

// SpotBugs 配置
spotbugs {
    ignoreFailures = false
    showStackTraces = true
    showProgress = true
    effort = 'max'
    reportLevel = 'medium'
}

// Spotless 配置（代码格式化）
spotless {
    java {
        googleJavaFormat('1.17.0')
        removeUnusedImports()
        trimTrailingWhitespace()
        endWithNewline()
    }
}

// 任务依赖
check.dependsOn jacocoTestCoverageVerification
```

**checkstyle.xml** (Google Style)：

```xml
<?xml version="1.0"?>
<!DOCTYPE module PUBLIC
    "-//Checkstyle//DTD Checkstyle Configuration 1.3//EN"
    "https://checkstyle.org/dtds/configuration_1_3.dtd">

<module name="Checker">
    <module name="TreeWalker">
        <!-- 命名规范 -->
        <module name="ConstantName"/>
        <module name="LocalFinalVariableName"/>
        <module name="LocalVariableName"/>
        <module name="MemberName"/>
        <module name="MethodName"/>
        <module name="PackageName"/>
        <module name="ParameterName"/>
        <module name="StaticVariableName"/>
        <module name="TypeName"/>
        
        <!-- 代码风格 -->
        <module name="AvoidStarImport"/>
        <module name="EmptyLineSeparator"/>
        <module name="Indentation">
            <property name="basicOffset" value="4"/>
            <property name="braceAdjustment" value="0"/>
        </module>
        <module name="LineLength">
            <property name="max" value="100"/>
        </module>
        
        <!-- 代码质量 -->
        <module name="EmptyBlock"/>
        <module name="LeftCurly"/>
        <module name="RightCurly"/>
        <module name="SimplifyBooleanExpression"/>
        <module name="SimplifyBooleanReturn"/>
    </module>
</module>
```

---

## 五、错误处理与恢复

### 5.1 执行失败处理策略

| 失败类型 | 处理策略 | 恢复操作 |
|----------|----------|----------|
| 测试失败 | 记录失败测试，继续其他任务 | 生成失败报告，提供修复建议 |
| 覆盖率不达标 | 生成覆盖率报告 | 标记未覆盖代码，提供测试建议 |
| 文档生成失败 | 跳过文档步骤 | 记录失败原因，继续提交流程 |
| 代码审查不通过 | 阻止提交 | 列出审查问题，提供修复指导 |
| 环境检查失败 | 阻止执行 | 提供环境配置指导 |

### 5.2 失败恢复流程

```
┌─────────────────────────────────────────────────────────────┐
│                    失败恢复流程                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. 记录失败详情                                            │
│     ├── 错误类型                                            │
│     ├── 错误信息                                            │
│     └── 上下文（文件、行号、测试名等）                       │
│                                                             │
│  2. 分析失败原因                                            │
│     ├── 代码bug                                             │
│     ├── 测试用例问题                                        │
│     ├── 环境问题                                            │
│     └── 依赖问题                                            │
│                                                             │
│  3. 提供修复建议                                            │
│     ├── 自动生成修复建议                                    │
│     ├── 提供相关文档链接                                    │
│     └── 标记需要用户确认的操作                              │
│                                                             │
│  4. 用户确认后继续或重试                                    │
│     ├── 自动修复（低风险）                                  │
│     ├── 用户确认后修复（中风险）                            │
│     └── 用户手动修复（高风险）                              │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 5.3 测试失败处理模板

```markdown
## 测试失败报告

### 失败测试
- 测试名称: `test_function_name`
- 测试文件: `tests/unit/test_module.py`
- 错误类型: AssertionError

### 错误信息
\`\`\`
AssertionError: Expected 'value1' but got 'value2'
\`\`\`

### 分析
- 可能原因: 函数返回值与预期不符
- 相关代码: `src/module.py:42`

### 修复建议
1. 检查函数逻辑是否正确
2. 验证测试用例预期值是否正确
3. 检查是否有副作用影响结果
```

---

## 六、CI/CD 集成

### 6.1 GitHub Actions 配置

```yaml
# .github/workflows/ci.yml
name: CI

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  test:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        python-version: ['3.10', '3.11']
    
    steps:
      - uses: actions/checkout@v3
      
      - name: Set up Python ${{ matrix.python-version }}
        uses: actions/setup-python@v4
        with:
          python-version: ${{ matrix.python-version }}
      
      - name: Install dependencies
        run: |
          pip install -r requirements.txt
          pip install pytest pytest-cov
      
      - name: Run tests
        run: pytest --cov=src --cov-fail-under=80 --cov-report=xml
      
      - name: Upload coverage
        uses: codecov/codecov-action@v3
        with:
          file: ./coverage.xml

  lint:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Set up Python
        uses: actions/setup-python@v4
        with:
          python-version: '3.10'
      
      - name: Install linters
        run: pip install black isort mypy pylint
      
      - name: Run Black
        run: black --check src tests
      
      - name: Run isort
        run: isort --check-only src tests
      
      - name: Run mypy
        run: mypy src
      
      - name: Run pylint
        run: pylint src
```

### 6.2 GitLab CI 配置

```yaml
# .gitlab-ci.yml
stages:
  - test
  - build
  - deploy

variables:
  PIP_CACHE_DIR: "$CI_PROJECT_DIR/.cache/pip"

cache:
  paths:
    - .cache/pip
    - venv/

test:
  stage: test
  image: python:3.10
  script:
    - pip install -r requirements.txt
    - pip install pytest pytest-cov
    - pytest --cov=src --cov-fail-under=80 --cov-report=xml --cov-report=term
  coverage: '/TOTAL.*\s+(\d+%)/'
  artifacts:
    reports:
      coverage_report:
        coverage_format: cobertura
        path: coverage.xml

lint:
  stage: test
  image: python:3.10
  script:
    - pip install black isort mypy pylint
    - black --check src tests
    - isort --check-only src tests
    - mypy src
    - pylint src

build:
  stage: build
  image: python:3.10
  script:
    - pip install build
    - python -m build
  artifacts:
    paths:
      - dist/
  only:
    - main
```

### 6.3 C++ CI 配置

```yaml
# .github/workflows/ci-cpp.yml
name: C++ CI

on: [push, pull_request]

jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest]
        build_type: [Debug, Release]
    
    steps:
      - uses: actions/checkout@v3
      
      - name: Configure CMake
        run: cmake -B build -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} -DBUILD_TESTING=ON
      
      - name: Build
        run: cmake --build build --config ${{ matrix.build_type }}
      
      - name: Test
        working-directory: build
        run: ctest --build-config ${{ matrix.build_type }} --output-on-failure
      
      - name: Coverage (Linux only)
        if: matrix.os == 'ubuntu-latest' && matrix.build_type == 'Debug'
        run: |
          lcov --capture --directory . --output-file coverage.info
          lcov --remove coverage.info '/usr/*' --output-file coverage.info
          bash <(curl -s https://codecov.io/bash) -f coverage.info
```

### 6.4 Java CI 配置

```yaml
# .github/workflows/ci-java.yml
name: Java CI

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        java: ['17', '21']
    
    steps:
      - uses: actions/checkout@v3
      
      - name: Set up JDK ${{ matrix.java }}
        uses: actions/setup-java@v3
        with:
          java-version: ${{ matrix.java }}
          distribution: 'temurin'
          cache: maven
      
      - name: Build with Maven
        run: mvn -B package --file pom.xml
      
      - name: Run tests
        run: mvn test
      
      - name: Generate coverage report
        run: mvn jacoco:report
      
      - name: Upload coverage
        uses: codecov/codecov-action@v3
        with:
          file: ./target/site/jacoco/jacoco.xml
      
      - name: Run static analysis
        run: mvn checkstyle:check spotbugs:check pmd:check
```

---

## 七、代码审查

### 7.1 代码审查检查清单

```markdown
## 代码审查检查清单

### 功能正确性
- [ ] 代码实现了预期功能
- [ ] 边界情况已处理
- [ ] 错误处理完整
- [ ] 日志记录适当

### 代码质量
- [ ] 命名清晰有意义
- [ ] 无重复代码
- [ ] 函数/方法长度合理
- [ ] 注释适当且准确
- [ ] 无魔法数字/字符串

### 测试充分性
- [ ] 单元测试覆盖核心逻辑
- [ ] 测试用例有代表性
- [ ] 覆盖率达标 (>80%)
- [ ] 边界条件已测试
- [ ] 异常情况已测试

### 安全性
- [ ] 无敏感信息硬编码
- [ ] 输入验证完整
- [ ] 无SQL注入风险
- [ ] 无XSS风险

### 性能
- [ ] 无明显性能问题
- [ ] 无内存泄漏风险
- [ ] 无不必要的循环/递归

### 可维护性
- [ ] 代码结构清晰
- [ ] 依赖关系合理
- [ ] 易于扩展
```

### 7.2 Pull Request 模板

```markdown
## PR 描述

### 变更类型
- [ ] 新功能 (feat)
- [ ] Bug修复 (fix)
- [ ] 重构 (refactor)
- [ ] 文档更新 (docs)
- [ ] 测试相关 (test)

### 变更说明
<!-- 描述本次变更的内容和原因 -->

### 关联Issue
Closes #

### 测试情况
- [ ] 已添加单元测试
- [ ] 已添加集成测试
- [ ] 所有测试通过
- [ ] 覆盖率达标

### 检查清单
- [ ] 代码符合编码规范
- [ ] 已更新相关文档
- [ ] 无新增警告
- [ ] 已自审代码
```

---

## 八、测试数据管理

### 8.1 测试数据策略

```
┌─────────────────────────────────────────────────────────────┐
│                    测试数据管理策略                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. 测试数据隔离                                            │
│     ├── 每个测试独立数据                                    │
│     ├── 使用setUp/tearDown清理                             │
│     └── 避免共享可变状态                                    │
│                                                             │
│  2. 测试数据生成                                            │
│     ├── 使用Factory模式                                     │
│     ├── 使用Fixture模式                                     │
│     └── 使用Faker库生成假数据                               │
│                                                             │
│  3. 测试数据存储                                            │
│     ├── 内存数据库（SQLite）                                │
│     ├── 测试专用数据库                                      │
│     └── Mock外部依赖                                        │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 8.2 Python 测试数据管理

```python
# tests/conftest.py
import pytest
from factory import Factory, Faker, Sequence
from myapp.models import User, Order

class UserFactory(Factory):
    class Meta:
        model = User
    
    id = Sequence(lambda n: n)
    name = Faker('name')
    email = Faker('email')
    status = 'active'

class OrderFactory(Factory):
    class Meta:
        model = Order
    
    id = Sequence(lambda n: n)
    user = Factory.SubFactory(UserFactory)
    amount = Faker('pydecimal', left_digits=4, right_digits=2, positive=True)
    status = 'pending'

@pytest.fixture
def user():
    return UserFactory()

@pytest.fixture
def users():
    return UserFactory.create_batch(10)

@pytest.fixture
def order(user):
    return OrderFactory(user=user)
```

### 8.3 Java 测试数据管理

```java
// src/test/java/com/example/fixtures/UserFixture.java
package com.example.fixtures;

import com.example.User;
import java.util.UUID;

public class UserFixture {
    
    public static User.UserBuilder aUser() {
        return User.builder()
            .id(UUID.randomUUID())
            .name("Test User")
            .email("test@example.com")
            .status(UserStatus.ACTIVE);
    }
    
    public static User aDefaultUser() {
        return aUser().build();
    }
    
    public static User anInactiveUser() {
        return aUser()
            .status(UserStatus.INACTIVE)
            .build();
    }
}

// 使用示例
@Test
void shouldCreateOrder() {
    User user = UserFixture.aDefaultUser();
    Order order = OrderFixture.anOrder().user(user).build();
    
    assertThat(order.getUser()).isEqualTo(user);
}
```

### 8.4 测试隔离原则

```markdown
## 测试隔离原则

### 1. 独立性原则
- 每个测试独立运行，不依赖其他测试
- 测试执行顺序不影响结果
- 测试之间无共享状态

### 2. 清理原则
- 使用 setUp() 初始化测试数据
- 使用 tearDown() 清理测试数据
- 数据库测试使用事务回滚

### 3. Mock原则
- 外部依赖使用Mock
- 网络请求使用Mock
- 文件系统使用临时目录

### 4. 数据原则
- 测试数据最小化
- 使用有意义的数据
- 避免生产数据
```

---

## 九、完整项目示例

### 9.1 Python项目示例：用户认证模块

**任务清单输入:**
```markdown
# 用户认证模块任务清单

## M1: 核心功能
- [ ] T1: 用户模型 (4h)
- [ ] T2: 密码哈希 (2h)
- [ ] T3: 用户仓储 (4h)
- [ ] T4: 认证服务 (6h)
- [ ] T5: API端点 (4h)
```

**TDD实施过程:**

**Step 1: T1 用户模型 - 编写测试**
```python
# tests/unit/test_user.py
import pytest
from datetime import datetime
from src.models.user import User, UserStatus

class TestUser:
    def test_create_user_with_valid_data(self):
        user = User(
            id=1,
            name="Test User",
            email="test@example.com",
            password_hash="hashed",
            status=UserStatus.ACTIVE
        )
        assert user.id == 1
        assert user.name == "Test User"
        assert user.status == UserStatus.ACTIVE

    def test_user_created_at_is_set(self):
        user = User(id=1, name="Test", email="test@example.com")
        assert user.created_at is not None
        assert isinstance(user.created_at, datetime)

    def test_user_default_status_is_pending(self):
        user = User(id=1, name="Test", email="test@example.com")
        assert user.status == UserStatus.PENDING
```

**Step 2: T1 用户模型 - 实现代码**
```python
# src/models/user.py
from datetime import datetime
from enum import Enum
from dataclasses import dataclass

class UserStatus(Enum):
    PENDING = "pending"
    ACTIVE = "active"
    INACTIVE = "inactive"

@dataclass
class User:
    id: int
    name: str
    email: str
    password_hash: str = ""
    status: UserStatus = UserStatus.PENDING
    created_at: datetime = None
    
    def __post_init__(self):
        if self.created_at is None:
            self.created_at = datetime.utcnow()
```

**Step 3: T2 密码哈希 - 编写测试**
```python
# tests/unit/test_password_hasher.py
import pytest
from src.services.password_hasher import PasswordHasher

class TestPasswordHasher:
    @pytest.fixture
    def hasher(self):
        return PasswordHasher()

    def test_hash_password_returns_hashed_string(self, hasher):
        password = "myPassword123"
        hashed = hasher.hash(password)
        assert hashed != password
        assert len(hashed) > 0

    def test_verify_correct_password(self, hasher):
        password = "myPassword123"
        hashed = hasher.hash(password)
        assert hasher.verify(password, hashed) is True

    def test_verify_wrong_password(self, hasher):
        password = "myPassword123"
        hashed = hasher.hash(password)
        assert hasher.verify("wrongPassword", hashed) is False

    def test_different_passwords_have_different_hashes(self, hasher):
        hash1 = hasher.hash("password1")
        hash2 = hasher.hash("password2")
        assert hash1 != hash2
```

**Step 4: T2 密码哈希 - 实现代码**
```python
# src/services/password_hasher.py
import bcrypt

class PasswordHasher:
    def __init__(self, rounds: int = 12):
        self.rounds = rounds

    def hash(self, password: str) -> str:
        salt = bcrypt.gensalt(rounds=self.rounds)
        return bcrypt.hashpw(password.encode(), salt).decode()

    def verify(self, password: str, hashed: str) -> bool:
        return bcrypt.checkpw(password.encode(), hashed.encode())
```

**运行测试验证覆盖率:**
```bash
$ pytest --cov=src --cov-report=term-missing

---------- coverage: platform win32 ----------
Name                              Stmts   Miss  Cover
-----------------------------------------------------
src/__init__.py                       0      0   100%
src/models/__init__.py                0      0   100%
src/models/user.py                   15      0   100%
src/services/__init__.py              0      0   100%
src/services/password_hasher.py      10      0   100%
-----------------------------------------------------
TOTAL                                25      0   100%

8 passed in 0.45s
```

**输出格式:**
```markdown
## 实现进度报告

### ✅ T1: 用户模型 (已完成)
- 测试用例: 3个 ✅
- 代码覆盖率: 100%
- 文件: src/models/user.py
- 状态: 已通过代码审查

### ✅ T2: 密码哈希 (已完成)
- 测试用例: 4个 ✅
- 代码覆盖率: 100%
- 文件: src/services/password_hasher.py
- 状态: 已通过代码审查

### 📋 T3: 用户仓储 (待开始)
- 依赖: T1
- 预估工时: 4h

### 📋 T4: 认证服务 (待开始)
- 依赖: T2, T3
- 预估工时: 6h

### 📋 T5: API端点 (待开始)
- 依赖: T4
- 预估工时: 4h

### 总体统计
- 已完成任务: 2/5
- 总测试用例: 7个
- 平均覆盖率: 100%
- 预计剩余时间: 14h
```

---

## 十、版本历史

| 版本 | 日期 | 变更内容 |
|------|------|----------|
| v1.0 | 2026-03-19 | 初始版本（Python + C++） |
| v1.1 | 2026-03-19 | 添加Java支持（JUnit 5 + Mockito + AssertJ + Javadoc） |
| v1.2 | 2026-03-19 | 添加CI/CD集成、错误恢复、Skill边界、代码审查、测试数据管理 |
| v1.3 | 2026-03-21 | 添加里程碑完成状态自动更新逻辑（6.4节） |
| v1.4 | 2026-03-22 | 增强触发条件：添加中文触发关键词、触发判断流程、自动行为说明、readme.md |
