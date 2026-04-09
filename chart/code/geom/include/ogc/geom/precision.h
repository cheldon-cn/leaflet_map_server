#pragma once

#include "common.h"
#include "coordinate.h"
#include <cmath>
#include <functional>

namespace ogc {

enum class PrecisionType {
    Floating,
    FloatingSingle,
    Fixed,
    Unknown
};

class OGC_GEOM_API PrecisionModel {
public:
    PrecisionModel() : m_type(PrecisionType::Floating), m_scale(0.0) {}
    
    explicit PrecisionModel(PrecisionType type) 
        : m_type(type), m_scale(0.0) {
        if (type == PrecisionType::FloatingSingle) {
            m_scale = 1e6;
        }
    }
    
    explicit PrecisionModel(double scale) 
        : m_type(PrecisionType::Fixed), m_scale(std::abs(scale)) {}
    
    PrecisionType GetType() const noexcept { return m_type; }
    
    double GetScale() const noexcept { return m_scale; }
    
    bool IsFloating() const noexcept { 
        return m_type == PrecisionType::Floating; 
    }
    
    bool IsFixed() const noexcept { 
        return m_type == PrecisionType::Fixed; 
    }
    
    double MakePrecise(double value) const {
        if (m_type == PrecisionType::Floating) {
            return value;
        }
        
        if (m_scale <= 0.0) {
            return value;
        }
        
        return std::round(value * m_scale) / m_scale;
    }
    
    Coordinate MakePrecise(const Coordinate& coord) const {
        if (coord.IsEmpty()) return coord;
        
        Coordinate result;
        result.x = MakePrecise(coord.x);
        result.y = MakePrecise(coord.y);
        
        if (coord.Is3D()) {
            result.z = MakePrecise(coord.z);
        }
        if (coord.IsMeasured()) {
            result.m = coord.m;
        }
        
        return result;
    }
    
    bool Equals(const PrecisionModel& other, double tolerance = DEFAULT_TOLERANCE) const noexcept {
        if (m_type != other.m_type) return false;
        
        if (m_type == PrecisionType::Fixed) {
            return std::abs(m_scale - other.m_scale) <= tolerance;
        }
        
        return true;
    }
    
    int Compare(double a, double b) const {
        double preciseA = MakePrecise(a);
        double preciseB = MakePrecise(b);
        
        if (preciseA < preciseB - DEFAULT_TOLERANCE) return -1;
        if (preciseA > preciseB + DEFAULT_TOLERANCE) return 1;
        return 0;
    }
    
    static PrecisionModel Floating() {
        return PrecisionModel(PrecisionType::Floating);
    }
    
    static PrecisionModel FloatingSingle() {
        return PrecisionModel(PrecisionType::FloatingSingle);
    }
    
    static PrecisionModel Fixed(double scale) {
        return PrecisionModel(scale);
    }
    
    bool operator==(const PrecisionModel& rhs) const noexcept {
        return Equals(rhs);
    }
    
    bool operator!=(const PrecisionModel& rhs) const noexcept {
        return !Equals(rhs);
    }
    
private:
    PrecisionType m_type;
    double m_scale;
};

class OGC_GEOM_API PrecisionSettings {
public:
    static PrecisionSettings& Instance() {
        static PrecisionSettings instance;
        return instance;
    }
    
    const PrecisionModel& GetModel() const noexcept { return m_model; }
    
    void SetModel(const PrecisionModel& model) { m_model = model; }
    
    void SetFloating() { m_model = PrecisionModel::Floating(); }
    void SetFloatingSingle() { m_model = PrecisionModel::FloatingSingle(); }
    void SetFixed(double scale) { m_model = PrecisionModel::Fixed(scale); }
    
    double MakePrecise(double value) const { return m_model.MakePrecise(value); }
    Coordinate MakePrecise(const Coordinate& coord) const { return m_model.MakePrecise(coord); }
    
    PrecisionSettings(const PrecisionSettings&) = delete;
    PrecisionSettings& operator=(const PrecisionSettings&) = delete;
    
private:
    PrecisionSettings() : m_model(PrecisionType::Floating) {}
    
    PrecisionModel m_model;
};

}
