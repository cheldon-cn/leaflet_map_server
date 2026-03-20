#pragma once

/**
 * @file coordinate.h
 * @brief 坐标结构定义，支持2D/3D/Measured坐标
 */

#include "common.h"
#include <string>
#include <vector>
#include <algorithm>

namespace ogc {

/**
 * @brief 坐标结构体，表示几何对象的基本坐标点
 * 
 * 支持2D坐标(x, y)、3D坐标(x, y, z)和带测量值的坐标(x, y, z, m)
 */
struct OGC_API Coordinate {
    double x = std::numeric_limits<double>::quiet_NaN();
    double y = std::numeric_limits<double>::quiet_NaN();
    double z = std::numeric_limits<double>::quiet_NaN();
    double m = std::numeric_limits<double>::quiet_NaN();
    
    Coordinate() = default;
    
    /**
     * @brief 构造2D坐标
     */
    Coordinate(double x_, double y_) : x(x_), y(y_) {}
    
    /**
     * @brief 构造3D坐标
     */
    Coordinate(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}
    
    /**
     * @brief 构造带测量值的坐标
     */
    Coordinate(double x_, double y_, double z_, double m_) 
        : x(x_), y(y_), z(z_), m(m_) {}
    
    /**
     * @brief 判断是否为3D坐标
     */
    bool Is3D() const noexcept {
        return !std::isnan(z);
    }
    
    /**
     * @brief 判断是否包含测量值
     */
    bool IsMeasured() const noexcept {
        return !std::isnan(m);
    }
    
    /**
     * @brief 判断是否为空坐标
     */
    bool IsEmpty() const noexcept {
        return std::isnan(x) || std::isnan(y);
    }
    
    /**
     * @brief 计算2D距离
     */
    double Distance(const Coordinate& other) const noexcept {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }
    
    /**
     * @brief 计算3D距离
     */
    double Distance3D(const Coordinate& other) const noexcept {
        double dx = x - other.x;
        double dy = y - other.y;
        double dz = z - other.z;
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }
    
    /**
     * @brief 判断两个坐标是否相等
     */
    bool Equals(const Coordinate& other, double tolerance = DEFAULT_TOLERANCE) const noexcept {
        if (IsEmpty() && other.IsEmpty()) return true;
        if (IsEmpty() || other.IsEmpty()) return false;
        
        if (std::abs(x - other.x) > tolerance) return false;
        if (std::abs(y - other.y) > tolerance) return false;
        if (Is3D() != other.Is3D()) return false;
        if (Is3D() && std::abs(z - other.z) > tolerance) return false;
        
        return true;
    }
    
    Coordinate operator+(const Coordinate& rhs) const noexcept {
        Coordinate result;
        result.x = x + rhs.x;
        result.y = y + rhs.y;
        if (Is3D() && rhs.Is3D()) {
            result.z = z + rhs.z;
        }
        if (IsMeasured() && rhs.IsMeasured()) {
            result.m = m + rhs.m;
        }
        return result;
    }
    
    Coordinate operator-(const Coordinate& rhs) const noexcept {
        Coordinate result;
        result.x = x - rhs.x;
        result.y = y - rhs.y;
        if (Is3D() && rhs.Is3D()) {
            result.z = z - rhs.z;
        }
        if (IsMeasured() && rhs.IsMeasured()) {
            result.m = m - rhs.m;
        }
        return result;
    }
    
    Coordinate operator*(double scalar) const noexcept {
        Coordinate result;
        result.x = x * scalar;
        result.y = y * scalar;
        if (Is3D()) {
            result.z = z * scalar;
        }
        if (IsMeasured()) {
            result.m = m * scalar;
        }
        return result;
    }
    
    bool operator==(const Coordinate& rhs) const noexcept {
        return Equals(rhs);
    }
    
    bool operator!=(const Coordinate& rhs) const noexcept {
        return !Equals(rhs);
    }
    
    /**
     * @brief 计算点积
     */
    double Dot(const Coordinate& other) const noexcept {
        double result = x * other.x + y * other.y;
        if (Is3D() && other.Is3D()) {
            result += z * other.z;
        }
        return result;
    }
    
    /**
     * @brief 计算叉积（2D）
     */
    Coordinate Cross(const Coordinate& other) const noexcept {
        return Coordinate(0, 0, x * other.y - y * other.x);
    }
    
    /**
     * @brief 计算向量长度
     */
    double Length() const noexcept {
        return Distance(Coordinate(0, 0));
    }
    
    /**
     * @brief 返回单位向量
     */
    Coordinate Normalize() const noexcept {
        double len = Length();
        if (len > DEFAULT_TOLERANCE) {
            return *this * (1.0 / len);
        }
        return *this;
    }
    
    /**
     * @brief 转换为WKT格式字符串
     */
    std::string ToWKT(int precision = DEFAULT_WKT_PRECISION) const;
    
    /**
     * @brief 创建空坐标
     */
    static Coordinate Empty() {
        Coordinate c;
        c.x = std::numeric_limits<double>::quiet_NaN();
        c.y = std::numeric_limits<double>::quiet_NaN();
        return c;
    }
    
    /**
     * @brief 从极坐标创建
     */
    static Coordinate FromPolar(double radius, double angle) {
        return Coordinate(radius * std::cos(angle), radius * std::sin(angle));
    }
};

using Coord = Coordinate;
using CoordinateList = std::vector<Coordinate>;

}
