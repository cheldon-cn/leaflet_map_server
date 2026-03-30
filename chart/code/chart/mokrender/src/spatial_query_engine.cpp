#include "ogc/mokrender/spatial_query_engine.h"
#include "ogc/db/sqlite_connection.h"
#include "ogc/geometry.h"
#include "ogc/envelope.h"
#include <string>
#include <vector>
#include <sqlite3.h>

namespace ogc {
namespace mokrender {

class SpatialQueryEngine::Impl {
public:
    std::string dbPath;
    bool isOpen;
    db::SpatiaLiteConnectionPtr connection;
    
    Impl() : isOpen(false) {}
};

SpatialQueryEngine::SpatialQueryEngine() : m_impl(new Impl()) {
}

SpatialQueryEngine::~SpatialQueryEngine() {
    Close();
}

MokRenderResult SpatialQueryEngine::Initialize(const std::string& dbPath) {
    m_impl->dbPath = dbPath;
    
    m_impl->connection = db::SpatiaLiteConnection::Create();
    if (!m_impl->connection) {
        return MokRenderResult(MokRenderErrorCode::OUT_OF_MEMORY,
                              "Failed to create connection");
    }
    
    db::SpatiaLiteOptions options;
    options.databasePath = dbPath;
    options.readOnly = false;
    
    db::Result result = m_impl->connection->Connect(options);
    if (result.IsError()) {
        return MokRenderResult(MokRenderErrorCode::DATABASE_ERROR,
                              "Failed to connect: " + result.GetMessage());
    }
    
    sqlite3* db = m_impl->connection->GetRawConnection();
    if (db) {
        char* errMsg = nullptr;
        int rc = sqlite3_enable_load_extension(db, 1);
        if (rc == SQLITE_OK) {
            rc = sqlite3_load_extension(db, "mod_spatialite5", nullptr, &errMsg);
            if (rc != SQLITE_OK) {
                sqlite3_free(errMsg);
            }
        }
    }
    
    m_impl->isOpen = true;
    return MokRenderResult();
}

std::vector<void*> SpatialQueryEngine::QueryByExtent(double minX, double minY,
                                                     double maxX, double maxY,
                                                     const std::string& layerName) {
    if (!m_impl->isOpen) {
        return std::vector<void*>();
    }
    
    std::vector<GeometryPtr> geometries;
    db::SpatialOperator op = db::SpatialOperator::kIntersects;
    
    Envelope envelope(minX, minY, maxX, maxY);
    db::Result result = m_impl->connection->SpatialQueryWithEnvelope(
        layerName, "geom", op, nullptr, envelope, geometries);
    
    if (result.IsError()) {
        return std::vector<void*>();
    }
    
    std::vector<void*> voidResults;
    for (auto& geom : geometries) {
        voidResults.push_back(geom.get());
    }
    
    return voidResults;
}

void SpatialQueryEngine::Close() {
    if (m_impl->connection) {
        m_impl->connection->Disconnect();
        m_impl->connection.reset();
    }
    m_impl->isOpen = false;
}

ISpatialQueryEngine* ISpatialQueryEngine::Create() {
    return new SpatialQueryEngine();
}

}
}
