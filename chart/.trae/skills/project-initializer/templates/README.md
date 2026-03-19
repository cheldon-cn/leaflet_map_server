# 项目名称

## 项目简介

[在此添加项目描述]

## 技术栈

- **核心库**: C++ (C++20)
- **桥接层**: JNI
- **Java封装**: Java 17
- **应用框架**: Spring Boot 3.2
- **数据库**: PostgreSQL
- **构建系统**: CMake + Maven

## 快速开始

### 前置要求

- CMake 3.20+
- Maven 3.8+
- Java 17+
- PostgreSQL 14+
- Visual Studio 2022 / GCC 11+ / Clang 15+

### 构建步骤

```bash
# 1. 克隆项目
git clone https://github.com/org/project.git
cd project

# 2. 配置环境
cp .env.example .env
# 编辑 .env 文件

# 3. 构建项目
./scripts/build.sh

# 4. 运行测试
./scripts/test.sh
```

### 运行应用

```bash
# 开发模式
./scripts/run.sh dev

# 生产模式
./scripts/run.sh prod
```

## 项目结构

```
project/
├── src/
│   ├── core/          # C++ 核心库
│   ├── bridge/        # JNI 桥接层
│   ├── java/          # Java 封装层
│   └── app/           # Spring Boot 应用
├── tests/             # 测试代码
├── docs/              # 文档
├── scripts/           # 脚本
└── config/            # 配置文件
```

## 配置说明

### 开发环境

配置 `config/dev/settings.json` 或设置环境变量。

### 数据库初始化

```bash
psql -U postgres -d project_db -f scripts/init.sql
```

## API 文档

访问 http://localhost:8080/swagger-ui.html 查看 API 文档。

## 测试

```bash
# 单元测试
./mvnw test

# 集成测试
./mvnw verify

# 测试覆盖率
./mvnw jacoco:report
```

## 许可证

[MIT License](LICENSE)
