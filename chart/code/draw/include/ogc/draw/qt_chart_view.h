#ifndef OGC_DRAW_QT_CHART_VIEW_H
#define OGC_DRAW_QT_CHART_VIEW_H

#include "ogc/draw/export.h"

#ifdef DRAW_HAS_QT

#include "ogc/draw/qt_event_adapter.h"
#include "ogc/draw/draw_facade.h"
#include "ogc/draw/interaction_handler.h"
#include "ogc/draw/hit_test.h"
#include "ogc/draw/interaction_feedback.h"

#include <QWidget>
#include <QTimer>
#include <memory>

namespace ogc {
namespace draw {

class OGC_DRAW_API QtChartView : public QWidget {
    Q_OBJECT
    Q_PROPERTY(double zoom READ GetZoom WRITE SetZoom)
    Q_PROPERTY(double rotation READ GetRotation WRITE SetRotation)
    Q_PROPERTY(bool showGrid READ IsGridVisible WRITE SetGridVisible)
    Q_PROPERTY(bool showScaleBar READ IsScaleBarVisible WRITE SetScaleBarVisible)

public:
    explicit QtChartView(QWidget* parent = nullptr);
    ~QtChartView() override;

    bool Initialize();
    void Finalize();
    bool IsInitialized() const { return m_initialized; }

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void SetDrawFacade(DrawFacade* facade);
    DrawFacade* GetDrawFacade() const { return m_drawFacade; }

    void SetLayerManager(LayerManager* manager);
    LayerManager* GetLayerManager() const;

    void SetExtent(const Envelope& extent);
    Envelope GetExtent() const;
    void SetFullExtent();
    Envelope GetFullExtent() const;

    void SetCenter(double x, double y);
    Coordinate GetCenter() const;

    void SetZoom(double zoom);
    double GetZoom() const;
    void ZoomIn(double factor = 1.2);
    void ZoomOut(double factor = 1.2);
    void ZoomToExtent(const Envelope& extent);

    void SetRotation(double degrees);
    double GetRotation() const;
    void Rotate(double degrees);
    void ResetRotation();

    void Pan(double dx, double dy);
    void PanTo(double x, double y);

    void Refresh();
    void RefreshRect(const QRect& rect);

    void SetGridVisible(bool visible);
    bool IsGridVisible() const { return m_gridVisible; }

    void SetScaleBarVisible(bool visible);
    bool IsScaleBarVisible() const { return m_scaleBarVisible; }

    void SetBackgroundColor(const QColor& color);
    QColor GetBackgroundColor() const { return m_backgroundColor; }

    void SetSelectionMode(SelectionMode mode);
    SelectionMode GetSelectionMode() const { return m_selectionMode; }

    void SetInteractionMode(InteractionMode mode);
    InteractionMode GetInteractionMode() const { return m_interactionMode; }

    void SetCursor(const QCursor& cursor);
    void SetDefaultCursor();
    void SetPanCursor();
    void SetZoomInCursor();
    void SetZoomOutCursor();
    void SetSelectCursor();

    void EnableAntialiasing(bool enable);
    bool IsAntialiasingEnabled() const { return m_antialiasing; }

    void SetAnimationFrameRate(int fps);
    int GetAnimationFrameRate() const { return m_animationFrameRate; }

    void StartAnimation();
    void StopAnimation();
    bool IsAnimationRunning() const { return m_animationRunning; }

    QtEventAdapter* GetEventAdapter() const { return m_eventAdapter.get(); }
    InteractionManager* GetInteractionManager() const { return m_interactionManager; }

signals:
    void ExtentChanged(const Envelope& extent);
    void ZoomChanged(double zoom);
    void RotationChanged(double rotation);
    void CenterChanged(double x, double y);
    void SelectionChanged(const std::vector<int64_t>& featureIds);
    void FeatureClicked(int64_t featureId, const QString& layerName);
    void FeatureDoubleClicked(int64_t featureId, const QString& layerName);
    void RenderCompleted();
    void RenderFailed(const QString& error);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;

private slots:
    void OnAnimationTimer();
    void OnInteractionEvent(const InteractionEvent& event);

private:
    void SetupInteraction();
    void SetupConnections();
    void UpdateTransform();
    void RenderToPainter(QPainter& painter);
    void DrawGrid(QPainter& painter);
    void DrawScaleBar(QPainter& painter);
    void DrawOverviewMap(QPainter& painter);
    void HandleSelection(const QPoint& pos, bool addToSelection);
    void HandlePan(const QPoint& pos);
    void HandleZoom(const QPoint& pos, double delta);
    void HandleMeasure(const QPoint& pos);

    bool m_initialized = false;
    DrawFacade* m_drawFacade = nullptr;
    std::unique_ptr<QtDisplayDevice> m_displayDevice;
    std::unique_ptr<QtEventAdapter> m_eventAdapter;
    InteractionManager* m_interactionManager = nullptr;

    Envelope m_extent;
    Envelope m_fullExtent;
    double m_zoom = 1.0;
    double m_rotation = 0.0;
    Coordinate m_center;

    bool m_gridVisible = false;
    bool m_scaleBarVisible = true;
    bool m_antialiasing = true;
    QColor m_backgroundColor;

    SelectionMode m_selectionMode = SelectionMode::kSingle;
    InteractionMode m_interactionMode = InteractionMode::kPan;

    QPoint m_lastMousePos;
    bool m_isPanning = false;
    bool m_isDragging = false;

    QTimer* m_animationTimer = nullptr;
    int m_animationFrameRate = 60;
    bool m_animationRunning = false;

    QCursor m_defaultCursor;
    QCursor m_panCursor;
    QCursor m_zoomInCursor;
    QCursor m_zoomOutCursor;
    QCursor m_selectCursor;
};

}  
}  

#endif

#endif
