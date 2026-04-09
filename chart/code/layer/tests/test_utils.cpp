#include "test_utils.h"

#include "ogc/feature/feature.h"
#include "ogc/feature/feature_defn.h"
#include "ogc/feature/field_defn.h"
#include "ogc/feature/field_value.h"
#include "ogc/feature/field_type.h"
#include "ogc/geom/geometry.h"
#include "ogc/geom/factory.h"
#include "ogc/geom/point.h"
#include "ogc/geom/linestring.h"
#include "ogc/geom/polygon.h"
#include "ogc/geom/envelope.h"

#include <algorithm>
#include <random>
#include <iostream>
#include <sstream>

namespace ogc {
namespace testing {

MockLayer::MockLayer()
    : feature_defn_(new CNFeatureDefn("mock")) {
}

MockLayer::MockLayer(const std::string& name)
    : name_(name), feature_defn_(new CNFeatureDefn(name.c_str())) {
}

void MockLayer::SetFeatureDefn(CNFeatureDefn* defn) {
    feature_defn_.reset(defn);
}

void MockLayer::AddFeature(std::unique_ptr<CNFeature> feature) {
    if (!feature) return;
    int64_t fid = feature->GetFID();
    if (fid <= 0) {
        fid = next_fid_++;
        feature->SetFID(fid);
    }
    fid_index_[fid] = features_.size();
    features_.push_back(std::move(feature));
    extent_valid_ = false;
}

void MockLayer::ClearFeatures() {
    features_.clear();
    fid_index_.clear();
    current_index_ = 0;
    extent_valid_ = false;
}

CNStatus MockLayer::GetExtent(Envelope& extent, bool force) const {
    if (extent_valid_) {
        extent = cached_extent_;
        return CNStatus::kSuccess;
    }
    if (features_.empty()) {
        return CNStatus::kError;
    }
    extent = Envelope();
    for (const auto& f : features_) {
        auto* geom = f->GetGeometry();
        if (geom) {
            const Envelope& fext = geom->GetEnvelope();
            extent.ExpandToInclude(fext);
        }
    }
    cached_extent_ = extent;
    extent_valid_ = true;
    return CNStatus::kSuccess;
}

int64_t MockLayer::GetFeatureCount(bool force) const {
    return static_cast<int64_t>(features_.size());
}

void MockLayer::ResetReading() {
    current_index_ = 0;
}

std::unique_ptr<CNFeature> MockLayer::GetNextFeature() {
    if (current_index_ >= features_.size()) {
        return nullptr;
    }
    return std::unique_ptr<CNFeature>(features_[current_index_++]->Clone());
}

CNFeature* MockLayer::GetNextFeatureRef() {
    if (current_index_ >= features_.size()) {
        return nullptr;
    }
    return features_[current_index_++].get();
}

std::unique_ptr<CNFeature> MockLayer::GetFeature(int64_t fid) {
    auto it = fid_index_.find(fid);
    if (it == fid_index_.end()) {
        return nullptr;
    }
    return std::unique_ptr<CNFeature>(features_[it->second]->Clone());
}

CNStatus MockLayer::SetFeature(const CNFeature* feature) {
    if (!feature) {
        return CNStatus::kNullPointer;
    }
    int64_t fid = feature->GetFID();
    auto it = fid_index_.find(fid);
    if (it == fid_index_.end()) {
        return CNStatus::kFeatureNotFound;
    }
    features_[it->second].reset(feature->Clone());
    extent_valid_ = false;
    return CNStatus::kSuccess;
}

CNStatus MockLayer::CreateFeature(CNFeature* feature) {
    if (!feature) {
        return CNStatus::kNullPointer;
    }
    int64_t fid = feature->GetFID();
    if (fid <= 0) {
        fid = next_fid_++;
        feature->SetFID(fid);
    }
    fid_index_[fid] = features_.size();
    features_.push_back(std::unique_ptr<CNFeature>(feature->Clone()));
    extent_valid_ = false;
    return CNStatus::kSuccess;
}

CNStatus MockLayer::DeleteFeature(int64_t fid) {
    auto it = fid_index_.find(fid);
    if (it == fid_index_.end()) {
        return CNStatus::kFeatureNotFound;
    }
    features_.erase(features_.begin() + it->second);
    fid_index_.erase(it);
    for (auto& pair : fid_index_) {
        if (pair.second > it->second) {
            pair.second--;
        }
    }
    extent_valid_ = false;
    return CNStatus::kSuccess;
}

int64_t MockLayer::CreateFeatureBatch(const std::vector<CNFeature*>& features) {
    int64_t count = 0;
    for (auto* f : features) {
        if (CreateFeature(f) == CNStatus::kSuccess) {
            count++;
        }
    }
    return count;
}

CNStatus MockLayer::CreateField(const CNFieldDefn* field_defn, bool approx_ok) {
    (void)field_defn;
    (void)approx_ok;
    return CNStatus::kNotSupported;
}

CNStatus MockLayer::DeleteField(int field_index) {
    (void)field_index;
    return CNStatus::kNotSupported;
}

CNStatus MockLayer::ReorderFields(const std::vector<int>& new_order) {
    (void)new_order;
    return CNStatus::kNotSupported;
}

CNStatus MockLayer::AlterFieldDefn(int field_index, const CNFieldDefn* new_defn, int flags) {
    (void)field_index;
    (void)new_defn;
    (void)flags;
    return CNStatus::kNotSupported;
}

void MockLayer::SetSpatialFilterRect(double min_x, double min_y, double max_x, double max_y) {
    auto& factory = GeometryFactory::GetInstance();
    spatial_filter_ = factory.CreateRectangle(min_x, min_y, max_x, max_y);
}

void MockLayer::SetSpatialFilter(const CNGeometry* geometry) {
    if (geometry) {
        spatial_filter_.reset(geometry->Clone());
    } else {
        spatial_filter_.reset();
    }
}

const CNGeometry* MockLayer::GetSpatialFilter() const {
    return spatial_filter_.get();
}

CNStatus MockLayer::SetAttributeFilter(const std::string& query) {
    attribute_filter_ = query;
    return CNStatus::kSuccess;
}

CNStatus MockLayer::StartTransaction() {
    in_transaction_ = true;
    return CNStatus::kSuccess;
}

CNStatus MockLayer::CommitTransaction() {
    in_transaction_ = false;
    return CNStatus::kSuccess;
}

CNStatus MockLayer::RollbackTransaction() {
    in_transaction_ = false;
    return CNStatus::kSuccess;
}

bool MockLayer::TestCapability(CNLayerCapability capability) const {
    auto it = capabilities_.find(capability);
    if (it != capabilities_.end()) {
        return it->second;
    }
    return false;
}

std::unique_ptr<CNLayer> MockLayer::Clone() const {
    auto clone = std::make_unique<MockLayer>(name_);
    clone->SetFeatureDefn(feature_defn_.get()->Clone());
    clone->SetGeomType(geom_type_);
    clone->SetSpatialFilter(spatial_filter_.get());
    for (const auto& f : features_) {
        clone->AddFeature(std::unique_ptr<CNFeature>(f->Clone()));
    }
    return clone;
}

void MockLayer::SetCapability(CNLayerCapability cap, bool value) {
    capabilities_[cap] = value;
}

void MockLayer::SetExtent(const Envelope& extent) {
    cached_extent_ = extent;
    extent_valid_ = true;
}

std::unique_ptr<CNFeature> TestDataGenerator::CreatePointFeature(
    CNFeatureDefn* defn,
    int64_t fid,
    double x, double y,
    const std::string& name) {

    auto feature = std::make_unique<CNFeature>(defn);
    feature->SetFID(fid);

    auto& factory = GeometryFactory::GetInstance();
    auto point = factory.CreatePoint(x, y);
    feature->SetGeometry(std::move(point));

    if (!name.empty() && defn->GetFieldCount() > 0) {
        feature->SetField(0, CNFieldValue(name));
    }

    return feature;
}

std::unique_ptr<CNFeature> TestDataGenerator::CreateLineStringFeature(
    CNFeatureDefn* defn,
    int64_t fid,
    const std::vector<double>& coords,
    const std::string& name) {

    auto feature = std::make_unique<CNFeature>(defn);
    feature->SetFID(fid);

    auto line = LineString::Create();
    for (size_t i = 0; i + 1 < coords.size(); i += 2) {
        line->AddPoint(coords[i], coords[i + 1]);
    }
    feature->SetGeometry(std::move(line));

    if (!name.empty() && defn->GetFieldCount() > 0) {
        feature->SetField(0, CNFieldValue(name));
    }

    return feature;
}

std::unique_ptr<CNFeature> TestDataGenerator::CreatePolygonFeature(
    CNFeatureDefn* defn,
    int64_t fid,
    const std::vector<std::vector<double>>& rings,
    const std::string& name) {

    auto feature = std::make_unique<CNFeature>(defn);
    feature->SetFID(fid);

    auto polygon = Polygon::Create();
    for (const auto& ring_coords : rings) {
        auto ring = LinearRing::Create();
        for (size_t i = 0; i + 1 < ring_coords.size(); i += 2) {
            ring->AddPoint(ring_coords[i], ring_coords[i + 1]);
        }
        polygon->AddRing(std::move(ring));
    }
    feature->SetGeometry(std::move(polygon));

    if (!name.empty() && defn->GetFieldCount() > 0) {
        feature->SetField(0, CNFieldValue(name));
    }

    return feature;
}

CNFeatureDefn* TestDataGenerator::CreateSimpleFeatureDefn(const std::string& name) {
    auto* defn = CNFeatureDefn::Create(name.c_str());

    auto* field = CreateCNFieldDefn("name");
    field->SetType(CNFieldType::kString);
    field->SetWidth(255);
    defn->AddFieldDefn(field);

    return defn;
}

CNFeatureDefn* TestDataGenerator::CreateComplexFeatureDefn(const std::string& name) {
    auto* defn = CNFeatureDefn::Create(name.c_str());

    auto* name_field = CreateCNFieldDefn("name");
    name_field->SetType(CNFieldType::kString);
    name_field->SetWidth(255);
    defn->AddFieldDefn(name_field);

    auto* id_field = CreateCNFieldDefn("id");
    id_field->SetType(CNFieldType::kInteger);
    defn->AddFieldDefn(id_field);

    auto* value_field = CreateCNFieldDefn("value");
    value_field->SetType(CNFieldType::kReal);
    defn->AddFieldDefn(value_field);

    auto* date_field = CreateCNFieldDefn("date");
    date_field->SetType(CNFieldType::kDate);
    defn->AddFieldDefn(date_field);

    return defn;
}

std::vector<std::unique_ptr<CNFeature>> TestDataGenerator::GenerateRandomPoints(
    CNFeatureDefn* defn,
    int count,
    double min_x, double max_x,
    double min_y, double max_y) {

    std::vector<std::unique_ptr<CNFeature>> features;
    features.reserve(count);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis_x(min_x, max_x);
    std::uniform_real_distribution<> dis_y(min_y, max_y);

    for (int i = 0; i < count; ++i) {
        double x = dis_x(gen);
        double y = dis_y(gen);
        features.push_back(CreatePointFeature(defn, i + 1, x, y, "Point_" + std::to_string(i + 1)));
    }

    return features;
}

std::vector<std::unique_ptr<CNFeature>> TestDataGenerator::GenerateGridPoints(
    CNFeatureDefn* defn,
    int rows, int cols,
    double min_x, double min_y,
    double cell_width, double cell_height) {

    std::vector<std::unique_ptr<CNFeature>> features;
    features.reserve(rows * cols);

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            double x = min_x + c * cell_width + cell_width / 2;
            double y = min_y + r * cell_height + cell_height / 2;
            int fid = r * cols + c + 1;
            features.push_back(CreatePointFeature(defn, fid, x, y, "Grid_" + std::to_string(fid)));
        }
    }

    return features;
}

bool LayerTestUtils::CompareFeatures(const CNFeature* f1, const CNFeature* f2, bool compare_fid) {
    if (!f1 || !f2) return false;

    if (compare_fid && f1->GetFID() != f2->GetFID()) {
        return false;
    }

    if (!CompareGeometries(f1->GetGeometry(), f2->GetGeometry())) {
        return false;
    }

    if (f1->GetFieldCount() != f2->GetFieldCount()) {
        return false;
    }

    for (size_t i = 0; i < f1->GetFieldCount(); ++i) {
        if (!CompareFieldValues(f1->GetField(i), f2->GetField(i))) {
            return false;
        }
    }

    return true;
}

bool LayerTestUtils::CompareGeometries(const CNGeometry* g1, const CNGeometry* g2) {
    if (!g1 || !g2) return g1 == g2;
    if (g1->GetGeomType() != g2->GetGeomType()) return false;
    if (g1->GetNumPoints() != g2->GetNumPoints()) return false;

    for (size_t i = 0; i < g1->GetNumPoints(); ++i) {
        auto p1 = g1->GetPoint(i);
        auto p2 = g2->GetPoint(i);
        if (std::abs(p1->GetX() - p2->GetX()) > 1e-10 ||
            std::abs(p1->GetY() - p2->GetY()) > 1e-10) {
            return false;
        }
    }

    return true;
}

bool LayerTestUtils::CompareFieldValues(const CNFieldValue& v1, const CNFieldValue& v2) {
    if (v1.IsNull() && v2.IsNull()) return true;
    if (v1.IsNull() || v2.IsNull()) return false;
    if (v1.GetType() != v2.GetType()) return false;

    switch (v1.GetType()) {
        case CNFieldType::kInteger:
            return v1.GetInteger() == v2.GetInteger();
        case CNFieldType::kInteger64:
            return v1.GetInteger64() == v2.GetInteger64();
        case CNFieldType::kReal:
            return std::abs(v1.GetDouble() - v2.GetDouble()) < 1e-10;
        case CNFieldType::kString:
            return v1.GetString() == v2.GetString();
        case CNFieldType::kBoolean:
            return v1.GetBoolean() == v2.GetBoolean();
        default:
            return true;
    }
}

void LayerTestUtils::PrintFeature(const CNFeature* feature) {
    if (!feature) {
        std::cout << "Feature: nullptr" << std::endl;
        return;
    }

    std::cout << "Feature[FID=" << feature->GetFID() << "]" << std::endl;

    auto* geom = feature->GetGeometry();
    if (geom) {
        std::cout << "  Geometry: " << static_cast<int>(geom->GetGeomType()) << std::endl;
    }

    for (size_t i = 0; i < feature->GetFieldCount(); ++i) {
        const auto& value = feature->GetField(i);
        if (value.IsNull()) {
            std::cout << "  Field[" << i << "]: NULL" << std::endl;
        } else {
            std::cout << "  Field[" << i << "]: " << value.ToString() << std::endl;
        }
    }
}

void LayerTestUtils::PrintLayerSummary(const CNLayer* layer) {
    if (!layer) {
        std::cout << "Layer: nullptr" << std::endl;
        return;
    }

    std::cout << "Layer: " << layer->GetName() << std::endl;
    std::cout << "  Type: " << static_cast<int>(layer->GetLayerType()) << std::endl;
    std::cout << "  Features: " << layer->GetFeatureCount() << std::endl;

    Envelope extent;
    if (layer->GetExtent(extent) == CNStatus::kSuccess) {
        std::cout << "  Extent: [" << extent.GetMinX() << ", " << extent.GetMaxX()
                  << ", " << extent.GetMinY() << ", " << extent.GetMaxY() << "]" << std::endl;
    }
}

std::string LayerTestUtils::GetTempFilePath(const std::string& prefix, const std::string& ext) {
    static int counter = 0;
    std::ostringstream oss;
    oss << prefix << "_" << counter++ << "." << ext;
    return oss.str();
}

} // namespace testing
} // namespace ogc
