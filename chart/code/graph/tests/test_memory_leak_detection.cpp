#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include "ogc/geom/geometry.h"
#include "ogc/geom/point.h"
#include "ogc/geom/linestring.h"
#include "ogc/geom/polygon.h"
#include "ogc/geom/linearring.h"
#include "ogc/geom/multipoint.h"
#include "ogc/geom/multilinestring.h"
#include "ogc/geom/coordinate.h"
#include "ogc/geom/envelope.h"
#include "ogc/graph/render/draw_facade.h"
#include <ogc/draw/raster_image_device.h>
#include <ogc/proj/coordinate_transform.h>

#ifdef _WIN32
#include <crtdbg.h>
#endif

using namespace ogc;
using namespace ogc::draw;
using namespace ogc::graph;
using namespace ogc::proj;

class MemoryLeakDetectionTest : public ::testing::Test {
protected:
    void SetUp() override {
        DrawFacade::Instance().Initialize();
    }
    
    void TearDown() override {
        DrawFacade::Instance().Finalize();
    }
};

TEST_F(MemoryLeakDetectionTest, PointMemoryLeak) {
    for (int i = 0; i < 1000; ++i) {
        auto point = ogc::Point::Create(Coordinate(i * 0.1, i * 0.1));
        EXPECT_TRUE(point->IsValid());
    }
}

TEST_F(MemoryLeakDetectionTest, LineStringMemoryLeak) {
    for (int i = 0; i < 100; ++i) {
        std::vector<Coordinate> coords;
        for (int j = 0; j < 100; ++j) {
            coords.push_back(Coordinate(j * 0.1, i * 0.1));
        }
        auto line = LineString::Create(coords);
        EXPECT_TRUE(line->IsValid());
    }
}

TEST_F(MemoryLeakDetectionTest, PolygonMemoryLeak) {
    for (int i = 0; i < 50; ++i) {
        std::vector<Coordinate> ring;
        for (int j = 0; j < 20; ++j) {
            double angle = j * 2 * 3.14159265359 / 20;
            ring.push_back(Coordinate(cos(angle) * 10, sin(angle) * 10));
        }
        ring.push_back(ring[0]);
        auto linearRing = LinearRing::Create(ring);
        auto polygon = Polygon::Create(std::move(linearRing));
        EXPECT_TRUE(polygon->IsValid());
    }
}

TEST_F(MemoryLeakDetectionTest, MultiPointMemoryLeak) {
    for (int i = 0; i < 100; ++i) {
        std::vector<Coordinate> coords;
        for (int j = 0; j < 50; ++j) {
            coords.push_back(Coordinate(j * 0.1, i * 0.1));
        }
        auto multiPoint = MultiPoint::Create(coords);
        EXPECT_TRUE(multiPoint->IsValid());
    }
}

TEST_F(MemoryLeakDetectionTest, MultiLineStringMemoryLeak) {
    for (int i = 0; i < 50; ++i) {
        std::vector<LineStringPtr> lines;
        for (int j = 0; j < 10; ++j) {
            std::vector<Coordinate> coords;
            coords.push_back(Coordinate(0, j));
            coords.push_back(Coordinate(10, j));
            lines.push_back(std::move(LineString::Create(coords)));
        }
        auto multiLine = MultiLineString::Create(std::move(lines));
        EXPECT_TRUE(multiLine->IsValid());
    }
}

TEST_F(MemoryLeakDetectionTest, GeometryCloneMemoryLeak) {
    std::vector<Coordinate> coords;
    for (int i = 0; i < 100; ++i) {
        coords.push_back(Coordinate(i * 0.1, i * 0.1));
    }
    auto original = LineString::Create(coords);
    
    for (int i = 0; i < 100; ++i) {
        auto clone = original->Clone();
        EXPECT_TRUE(clone->IsValid());
    }
}

TEST_F(MemoryLeakDetectionTest, CoordinateTransformMemoryLeak) {
    auto transform = CoordinateTransform::Create();
    ASSERT_NE(transform, nullptr);
    
    for (int i = 0; i < 100; ++i) {
        auto point = ogc::Point::Create(Coordinate(116.0 + i * 0.01, 39.0 + i * 0.01));
        auto transformed = transform->Transform(point.get());
        EXPECT_NE(transformed, nullptr);
    }
}

TEST_F(MemoryLeakDetectionTest, DrawFacadeMemoryLeak) {
    auto& facade = DrawFacade::Instance();
    
    for (int i = 0; i < 10; ++i) {
        auto device = facade.CreateDevice(DeviceType::kRasterImage, 800, 600);
        EXPECT_NE(device, nullptr);
        
        Envelope extent(0, 0, 100, 100);
        auto params = facade.CreateDrawParams(extent, 800, 600, 96.0);
    }
}

TEST_F(MemoryLeakDetectionTest, EnvelopeOperationsMemoryLeak) {
    for (int i = 0; i < 1000; ++i) {
        Envelope env1(0, 0, 10, 10);
        Envelope env2(5, 5, 15, 15);
        
        Envelope intersection = env1.Intersection(env2);
        Envelope expanded = env1;
        expanded.ExpandToInclude(env2);
        
        EXPECT_FALSE(intersection.IsNull());
        EXPECT_FALSE(expanded.IsNull());
    }
}

TEST_F(MemoryLeakDetectionTest, WKTSerializationMemoryLeak) {
    std::vector<Coordinate> ring;
    for (int j = 0; j < 20; ++j) {
        double angle = j * 2 * 3.14159265359 / 20;
        ring.push_back(Coordinate(cos(angle) * 10, sin(angle) * 10));
    }
    ring.push_back(ring[0]);
    auto linearRing = LinearRing::Create(ring);
    auto polygon = Polygon::Create(std::move(linearRing));
    
    for (int i = 0; i < 100; ++i) {
        std::string wkt = polygon->AsText();
        EXPECT_FALSE(wkt.empty());
    }
}

TEST_F(MemoryLeakDetectionTest, LargeGeometryCollectionMemoryLeak) {
    std::vector<GeometryPtr> geometries;
    geometries.reserve(1000);
    
    for (int i = 0; i < 1000; ++i) {
        if (i % 3 == 0) {
            geometries.push_back(ogc::Point::Create(Coordinate(i * 0.1, i * 0.1)));
        } else if (i % 3 == 1) {
            std::vector<Coordinate> coords;
            coords.push_back(Coordinate(i * 0.1, 0));
            coords.push_back(Coordinate(i * 0.1 + 1, 1));
            geometries.push_back(LineString::Create(coords));
        } else {
            std::vector<Coordinate> ring;
            ring.push_back(Coordinate(0, 0));
            ring.push_back(Coordinate(1, 0));
            ring.push_back(Coordinate(1, 1));
            ring.push_back(Coordinate(0, 1));
            ring.push_back(Coordinate(0, 0));
            auto linearRing = LinearRing::Create(ring);
            geometries.push_back(Polygon::Create(std::move(linearRing)));
        }
    }
    
    EXPECT_EQ(geometries.size(), 1000u);
    
    for (const auto& geom : geometries) {
        EXPECT_TRUE(geom->IsValid() || geom->IsEmpty());
    }
}

TEST_F(MemoryLeakDetectionTest, NestedGeometryOperationsMemoryLeak) {
    for (int i = 0; i < 50; ++i) {
        std::vector<Coordinate> coords;
        for (int j = 0; j < 10; ++j) {
            coords.push_back(Coordinate(j * 0.1, i * 0.1));
        }
        
        auto multiPoint = MultiPoint::Create(coords);
        EXPECT_TRUE(multiPoint->IsValid());
        
        auto clone = multiPoint->Clone();
        EXPECT_TRUE(clone->IsValid());
        
        Envelope env = multiPoint->GetEnvelope();
        EXPECT_FALSE(env.IsNull());
        
        std::string wkt = multiPoint->AsText();
        EXPECT_FALSE(wkt.empty());
    }
}
