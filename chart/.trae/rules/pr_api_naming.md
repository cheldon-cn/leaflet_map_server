# API命名一致性

> **主规则**: @.trae/rules/project_rules.md

---

## 一、命名规则

| 规则ID | 规则内容 | 错误示例 | 正确示例 |
|--------|----------|----------|----------|
| API-01 | Getter方法使用Get前缀 | `Size()` | `GetSize()` |
| API-02 | 索引访问使用N后缀 | `GetCoordinateAt(i)` | `GetCoordinateN(i)` |
| API-03 | 遵循OGC标准方法名 | `AsWKT()` | `AsText()` |

---

## 二、API命名对照表

| 错误调用 | 正确调用 |
|----------|----------|
| RenderQueue::Size() | RenderQueue::GetSize() |
| LineString::GetCoordinateAt | LineString::GetCoordinateN |
| Polygon::GetInteriorRing | Polygon::GetInteriorRingN |
| Geometry::AsWKT() | Geometry::AsText() |
| TileKey::GetX() | TileKey::x (公开成员) |
| Color::ToRGB() | Color::GetRGB() |
| Envelope::Contains(x, y) | Envelope::Contains(Coordinate(x, y)) |

---

**版本**: v3.0