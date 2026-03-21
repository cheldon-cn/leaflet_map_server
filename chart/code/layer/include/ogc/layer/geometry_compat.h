#pragma once

#include "ogc/geometry.h"

namespace ogc {
    using CNGeometry = Geometry;
    using CNGeometryPtr = std::unique_ptr<Geometry>;
    using CNGeometryConstPtr = std::unique_ptr<const Geometry>;
}
