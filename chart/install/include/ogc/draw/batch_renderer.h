#ifndef OGC_DRAW_BATCH_RENDERER_H
#define OGC_DRAW_BATCH_RENDERER_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_style.h"
#include <ogc/geometry.h>
#include <vector>
#include <memory>

namespace ogc {
namespace draw {

class DrawEngine;

struct BatchItem {
    GeometrySharedPtr geometry;
    DrawStyle style;
    int priority;
};

class OGC_DRAW_API BatchRenderer {
public:
    BatchRenderer();
    explicit BatchRenderer(DrawEngine* engine);
    ~BatchRenderer();
    
    void SetEngine(DrawEngine* engine);
    DrawEngine* GetEngine() const { return m_engine; }
    
    void BeginBatch();
    void AddGeometry(const GeometrySharedPtr& geometry, const DrawStyle& style, int priority = 0);
    void EndBatch();
    void Flush();
    
    void SetBatchSize(int size);
    int GetBatchSize() const { return m_batchSize; }
    int GetBatchCount() const { return static_cast<int>(m_batchItems.size()); }
    
    void SetAutoFlush(bool autoFlush) { m_autoFlush = autoFlush; }
    bool IsAutoFlush() const { return m_autoFlush; }
    
    void SetSortByPriority(bool sort) { m_sortByPriority = sort; }
    bool IsSortByPriority() const { return m_sortByPriority; }
    
    void Clear();
    bool IsEmpty() const { return m_batchItems.empty(); }
    
    void SetMaxBatchItems(int maxItems);
    int GetMaxBatchItems() const { return m_maxBatchItems; }
    
    int GetTotalVertices() const;
    int GetTotalIndices() const;

private:
    void RenderBatch();
    void SortBatch();
    void MergeSimilarItems();
    
    DrawEngine* m_engine;
    std::vector<BatchItem> m_batchItems;
    int m_batchSize;
    bool m_inBatch;
    bool m_autoFlush;
    bool m_sortByPriority;
    int m_maxBatchItems;
};

}
}

#endif
