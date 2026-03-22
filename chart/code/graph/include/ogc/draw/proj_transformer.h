#ifndef OGC_DRAW_PROJ_TRANSFORMER_H
#define OGC_DRAW_PROJ_TRANSFORMER_H

#include "ogc/draw/coordinate_transformer.h"
#include <mutex>

namespace ogc {
namespace draw {

class ProjTransformer;
typedef std::shared_ptr<ProjTransformer> ProjTransformerPtr;

class ProjTransformer : public CoordinateTransformer {
public:
    ProjTransformer();
    ProjTransformer(const std::string& sourceCRS, const std::string& targetCRS);
    virtual ~ProjTransformer();
    
    virtual bool IsValid() const override;
    
    virtual Coordinate Transform(const Coordinate& coord) const override;
    virtual Coordinate TransformInverse(const Coordinate& coord) const override;
    
    virtual void Transform(double& x, double& y) const override;
    virtual void TransformInverse(double& x, double& y) const override;
    
    virtual void TransformArray(double* x, double* y, size_t count) const override;
    virtual void TransformArrayInverse(double* x, double* y, size_t count) const override;
    
    virtual Envelope Transform(const Envelope& env) const override;
    virtual Envelope TransformInverse(const Envelope& env) const override;
    
    virtual GeometryPtr Transform(const Geometry* geometry) const override;
    virtual GeometryPtr TransformInverse(const Geometry* geometry) const override;
    
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
    void* m_projContext;
    void* m_projTransform;
    void* m_projInverseTransform;
    std::string m_sourceCRS;
    std::string m_targetCRS;
    std::string m_name;
    std::string m_description;
    bool m_valid;
    mutable std::mutex m_mutex;
    
    bool CreateTransform(const std::string& sourceCRS, const std::string& targetCRS);
    void DestroyTransform();
    
    std::string NormalizeCRS(const std::string& crs) const;
    bool IsEpsgCode(const std::string& crs) const;
    int ParseEpsgCode(const std::string& crs) const;
    
    GeometryPtr TransformGeometry(const Geometry* geometry, bool forward) const;
    GeometryPtr TransformPoint(const Geometry* geometry, bool forward) const;
    GeometryPtr TransformLineString(const Geometry* geometry, bool forward) const;
    GeometryPtr TransformPolygon(const Geometry* geometry, bool forward) const;
    GeometryPtr TransformMultiPoint(const Geometry* geometry, bool forward) const;
    GeometryPtr TransformMultiLineString(const Geometry* geometry, bool forward) const;
    GeometryPtr TransformMultiPolygon(const Geometry* geometry, bool forward) const;
    GeometryPtr TransformGeometryCollection(const Geometry* geometry, bool forward) const;
};

}
}

#endif
