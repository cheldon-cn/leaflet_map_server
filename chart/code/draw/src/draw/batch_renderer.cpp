#include "ogc/draw/batch_renderer.h"
#include "ogc/draw/draw_engine.h"
#include <algorithm>

namespace ogc {
namespace draw {

BatchRenderer::BatchRenderer()
    : m_engine(nullptr)
    , m_batchSize(100)
    , m_inBatch(false)
    , m_autoFlush(true)
    , m_sortByPriority(false)
    , m_maxBatchItems(10000) {
}

BatchRenderer::BatchRenderer(DrawEngine* engine)
    : m_engine(engine)
    , m_batchSize(100)
    , m_inBatch(false)
    , m_autoFlush(true)
    , m_sortByPriority(false)
    , m_maxBatchItems(10000) {
}

BatchRenderer::~BatchRenderer() {
    if (m_inBatch && !m_batchItems.empty()) {
        EndBatch();
    }
}

void BatchRenderer::SetEngine(DrawEngine* engine) {
    if (m_inBatch) {
        EndBatch();
    }
    m_engine = engine;
}

void BatchRenderer::BeginBatch() {
    m_batchItems.clear();
    m_inBatch = true;
}

void BatchRenderer::AddGeometry(const GeometrySharedPtr& geometry, const DrawStyle& style, int priority) {
    if (!m_inBatch) {
        BeginBatch();
    }
    
    if (static_cast<int>(m_batchItems.size()) >= m_maxBatchItems) {
        if (m_autoFlush) {
            Flush();
        } else {
            return;
        }
    }
    
    BatchItem item;
    item.geometry = geometry;
    item.style = style;
    item.priority = priority;
    m_batchItems.push_back(item);
    
    if (m_autoFlush && static_cast<int>(m_batchItems.size()) >= m_batchSize) {
        Flush();
    }
}

void BatchRenderer::EndBatch() {
    if (!m_batchItems.empty()) {
        RenderBatch();
    }
    m_inBatch = false;
}

void BatchRenderer::Flush() {
    if (!m_batchItems.empty()) {
        RenderBatch();
    }
}

void BatchRenderer::SetBatchSize(int size) {
    m_batchSize = size > 0 ? size : 100;
}

void BatchRenderer::SetMaxBatchItems(int maxItems) {
    m_maxBatchItems = maxItems > 0 ? maxItems : 10000;
}

void BatchRenderer::Clear() {
    m_batchItems.clear();
}

int BatchRenderer::GetTotalVertices() const {
    int total = 0;
    for (const auto& item : m_batchItems) {
        if (item.geometry) {
            total += static_cast<int>(item.geometry->GetNumCoordinates());
        }
    }
    return total;
}

int BatchRenderer::GetTotalIndices() const {
    int total = 0;
    for (const auto& item : m_batchItems) {
        if (item.geometry) {
            total += static_cast<int>(item.geometry->GetNumCoordinates());
        }
    }
    return total;
}

void BatchRenderer::RenderBatch() {
    if (!m_engine || m_batchItems.empty()) {
        return;
    }
    
    if (m_sortByPriority) {
        SortBatch();
    }
    
    MergeSimilarItems();
    
    for (const auto& item : m_batchItems) {
        if (item.geometry) {
            m_engine->DrawGeometry(item.geometry.get(), item.style);
        }
    }
    
    m_batchItems.clear();
}

void BatchRenderer::SortBatch() {
    std::sort(m_batchItems.begin(), m_batchItems.end(),
        [](const BatchItem& a, const BatchItem& b) {
            return a.priority < b.priority;
        });
}

void BatchRenderer::MergeSimilarItems() {
}

}
}
