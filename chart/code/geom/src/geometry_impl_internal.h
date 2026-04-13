#pragma once

#include <mutex>
#include <memory>
#include <atomic>
#include "ogc/geom/envelope.h"
#include "ogc/geom/coordinate.h"

namespace ogc {

struct Geometry::Impl {
    mutable std::mutex cache_mutex;
    mutable std::unique_ptr<Envelope> envelope_cache;
    mutable std::unique_ptr<Coordinate> centroid_cache;
    std::atomic<int> srid{0};
};

}
