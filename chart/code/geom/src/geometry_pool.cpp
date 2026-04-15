#include "ogc/geom/geometry_pool.h"
#include "ogc/geom/point.h"
#include "ogc/geom/linestring.h"
#include "ogc/geom/polygon.h"
#include "geometry_impl_internal.h"

namespace ogc {

template<typename T>
ObjectPool<T>::~ObjectPool() = default;

template class OGC_GEOM_API ObjectPool<Point>;
template class OGC_GEOM_API ObjectPool<LineString>;
template class OGC_GEOM_API ObjectPool<Polygon>;

}
