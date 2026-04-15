#ifndef OGC_PROJ_PROJ_TRANSFORMER_H
#define OGC_PROJ_PROJ_TRANSFORMER_H

#include "ogc/proj/coordinate_transformer.h"
#include <mutex>

namespace ogc {
namespace proj {

class ProjTransformer;
typedef std::shared_ptr<ProjTransformer> ProjTransformerPtr;

class OGC_PROJ_API ProjTransformer : public CoordinateTransformer {
public:
    ProjTransformer();
    ProjTransformer(const std::string& sourceCRS, const std::string& targetCRS);
    virtual ~ProjTransformer();
    
    virtual bool IsValid() const override;
    
    virtual ogc::Coordinate Transform(const ogc::Coordinate& coord) const override;
    virtual ogc::Coordinate TransformInverse(const ogc::Coordinate& coord) const override;
    
    virtual void Transform(double& x, double& y) const override;
    virtual void TransformInverse(double& x, double& y) const override;
    
    virtual void TransformArray(double* x, double* y, size_t count) const override;
    virtual void TransformArrayInverse(double* x, double* y, size_t count) const override;
    
    virtual ogc::Envelope Transform(const ogc::Envelope& env) const override;
    virtual ogc::Envelope TransformInverse(const ogc::Envelope& env) const override;
    
    virtual ogc::GeometryPtr Transform(const ogc::Geometry* geometry) const override;
    virtual ogc::GeometryPtr TransformInverse(const ogc::Geometry* geometry) const override;
    
    virtual std::string GetSourceCRS() const override;
    virtual std::string GetTargetCRS() const override;
    
    virtual std::string GetName() const override;
    virtual std::string GetDescription() const override;
    
    virtual CoordinateTransformerPtr Clone() const override;
    
    bool Initialize(const std::string& sourceCRS, const std::string& targetCRS);
    
    void SetDescription(const std::string& description);
    
    static ProjTransformerPtr Create(const std::string& sourceCRS, const std::string& targetCRS);
    static ProjTransformerPtr CreateFromEpsg(int sourceEpsg, int targetEpsg);
    static ProjTransformerPtr CreateFromWkt(const std::string& sourceWkt, const std::string& targetWkt);
    static ProjTransformerPtr CreateFromProjString(const std::string& sourceProj, const std::string& targetProj);
    
    static bool IsProjAvailable();
    static std::string GetProjVersion();
    
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    
    bool CreateTransform(const std::string& sourceCRS, const std::string& targetCRS);
    void DestroyTransform();
    
    std::string NormalizeCRS(const std::string& crs) const;
    bool IsEpsgCode(const std::string& crs) const;
    int ParseEpsgCode(const std::string& crs) const;
    
    ogc::GeometryPtr TransformGeometry(const ogc::Geometry* geometry, bool forward) const;
    ogc::GeometryPtr TransformPoint(const ogc::Geometry* geometry, bool forward) const;
    ogc::GeometryPtr TransformLineString(const ogc::Geometry* geometry, bool forward) const;
    ogc::GeometryPtr TransformPolygon(const ogc::Geometry* geometry, bool forward) const;
    ogc::GeometryPtr TransformMultiPoint(const ogc::Geometry* geometry, bool forward) const;
    ogc::GeometryPtr TransformMultiLineString(const ogc::Geometry* geometry, bool forward) const;
    ogc::GeometryPtr TransformMultiPolygon(const ogc::Geometry* geometry, bool forward) const;
    ogc::GeometryPtr TransformGeometryCollection(const ogc::Geometry* geometry, bool forward) const;
};

}
}

#endif
