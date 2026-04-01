#include "ogc/draw/qt_chart_view.h"
#include "ogc/draw/qt_display_device.h"
#include "ogc/draw/qt_engine.h"

#ifdef DRAW_HAS_QT

#include <QPainter>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QtMath>
#include <QDebug>

namespace ogc {
namespace draw {

QtChartView::QtChartView(QWidget* parent)
    : QWidget(parent)
    , m_backgroundColor(Qt::white)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    
    m_defaultCursor = Qt::ArrowCursor;
    m_panCursor = Qt::OpenHandCursor;
    m_zoomInCursor = Qt::CrossCursor;
    m_zoomOutCursor = Qt::CrossCursor;
    m_selectCursor = Qt::CrossCursor;
    
    m_animationTimer = new QTimer(this);
    connect(m_animationTimer, &QTimer::timeout, this, &QtChartView::OnAnimationTimer);
}

QtChartView::~QtChartView() {
    Finalize();
}

bool QtChartView::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    m_displayDevice = std::make_unique<QtDisplayDevice>(this);
    
    m_eventAdapter = std::make_unique<QtEventAdapter>(this);
    m_eventAdapter->SetMouseTracking(true);
    
    m_interactionManager = &InteractionManager::Instance();
    if (!m_interactionManager->IsInitialized()) {
        m_interactionManager->Initialize();
    }
    m_eventAdapter->SetInteractionManager(m_interactionManager);
    
    SetupInteraction();
    SetupConnections();
    
    m_initialized = true;
    return true;
}

void QtChartView::Finalize() {
    if (!m_initialized) {
        return;
    }
    
    StopAnimation();
    
    m_eventAdapter.reset();
    m_displayDevice.reset();
    m_interactionManager = nullptr;
    m_drawFacade = nullptr;
    
    m_initialized = false;
}

QSize QtChartView::sizeHint() const {
    return QSize(800, 600);
}

QSize QtChartView::minimumSizeHint() const {
    return QSize(200, 200);
}

void QtChartView::SetDrawFacade(DrawFacade* facade) {
    m_drawFacade = facade;
}

void QtChartView::SetLayerManager(LayerManager* manager) {
    if (m_interactionManager) {
        m_interactionManager->SetLayerManager(manager);
    }
}

LayerManager* QtChartView::GetLayerManager() const {
    return m_interactionManager ? m_interactionManager->GetLayerManager() : nullptr;
}

void QtChartView::SetExtent(const Envelope& extent) {
    m_extent = extent;
    UpdateTransform();
    emit ExtentChanged(extent);
    refresh();
}

Envelope QtChartView::GetExtent() const {
    return m_extent;
}

void QtChartView::SetFullExtent() {
    if (!m_fullExtent.IsNull()) {
        SetExtent(m_fullExtent);
    }
}

Envelope QtChartView::GetFullExtent() const {
    return m_fullExtent;
}

void QtChartView::SetCenter(double x, double y) {
    m_center = Coordinate(x, y);
    UpdateTransform();
    emit CenterChanged(x, y);
    refresh();
}

Coordinate QtChartView::GetCenter() const {
    return m_center;
}

void QtChartView::SetZoom(double zoom) {
    m_zoom = qBound(0.01, zoom, 100000.0);
    UpdateTransform();
    emit ZoomChanged(m_zoom);
    refresh();
}

double QtChartView::GetZoom() const {
    return m_zoom;
}

void QtChartView::ZoomIn(double factor) {
    SetZoom(m_zoom * factor);
}

void QtChartView::ZoomOut(double factor) {
    SetZoom(m_zoom / factor);
}

void QtChartView::ZoomToExtent(const Envelope& extent) {
    if (extent.IsNull()) {
        return;
    }
    
    double width = extent.GetWidth();
    double height = extent.GetHeight();
    
    double viewWidth = this->width();
    double viewHeight = this->height();
    
    double scaleX = viewWidth / width;
    double scaleY = viewHeight / height;
    double scale = qMin(scaleX, scaleY) * 0.9;
    
    SetZoom(scale);
    SetCenter(extent.GetCenter().x, extent.GetCenter().y);
}

void QtChartView::SetRotation(double degrees) {
    m_rotation = degrees;
    UpdateTransform();
    emit RotationChanged(m_rotation);
    refresh();
}

double QtChartView::GetRotation() const {
    return m_rotation;
}

void QtChartView::Rotate(double degrees) {
    SetRotation(m_rotation + degrees);
}

void QtChartView::ResetRotation() {
    SetRotation(0.0);
}

void QtChartView::Pan(double dx, double dy) {
    double worldDx = dx / m_zoom;
    double worldDy = dy / m_zoom;
    SetCenter(m_center.x - worldDx, m_center.y + worldDy);
}

void QtChartView::PanTo(double x, double y) {
    SetCenter(x, y);
}

void QtChartView::Refresh() {
    update();
}

void QtChartView::RefreshRect(const QRect& rect) {
    update(rect);
}

void QtChartView::SetGridVisible(bool visible) {
    m_gridVisible = visible;
    refresh();
}

void QtChartView::SetScaleBarVisible(bool visible) {
    m_scaleBarVisible = visible;
    refresh();
}

void QtChartView::SetBackgroundColor(const QColor& color) {
    m_backgroundColor = color;
    refresh();
}

void QtChartView::SetSelectionMode(SelectionMode mode) {
    m_selectionMode = mode;
}

SelectionMode QtChartView::GetSelectionMode() const {
    return m_selectionMode;
}

void QtChartView::SetInteractionMode(InteractionMode mode) {
    m_interactionMode = mode;
    switch (mode) {
        case InteractionMode::kPan:
            setCursor(m_panCursor);
            break;
        case InteractionMode::kZoomIn:
            setCursor(m_zoomInCursor);
            break;
        case InteractionMode::kZoomOut:
            setCursor(m_zoomOutCursor);
            break;
        case InteractionMode::kSelect:
            setCursor(m_selectCursor);
            break;
        default:
            setCursor(m_defaultCursor);
            break;
    }
}

InteractionMode QtChartView::GetInteractionMode() const {
    return m_interactionMode;
}

void QtChartView::SetCursor(const QCursor& cursor) {
    QWidget::setCursor(cursor);
}

void QtChartView::SetDefaultCursor() {
    setCursor(m_defaultCursor);
}

void QtChartView::SetPanCursor() {
    setCursor(m_panCursor);
}

void QtChartView::SetZoomInCursor() {
    setCursor(m_zoomInCursor);
}

void QtChartView::SetZoomOutCursor() {
    setCursor(m_zoomOutCursor);
}

void QtChartView::SetSelectCursor() {
    setCursor(m_selectCursor);
}

void QtChartView::EnableAntialiasing(bool enable) {
    m_antialiasing = enable;
    refresh();
}

bool QtChartView::IsAntialiasingEnabled() const {
    return m_antialiasing;
}

void QtChartView::SetAnimationFrameRate(int fps) {
    m_animationFrameRate = qBound(1, fps, 120);
    if (m_animationRunning) {
        m_animationTimer->setInterval(1000 / m_animationFrameRate);
    }
}

int QtChartView::GetAnimationFrameRate() const {
    return m_animationFrameRate;
}

void QtChartView::StartAnimation() {
    if (!m_animationRunning) {
        m_animationTimer->start(1000 / m_animationFrameRate);
        m_animationRunning = true;
    }
}

void QtChartView::StopAnimation() {
    if (m_animationRunning) {
        m_animationTimer->stop();
        m_animationRunning = false;
    }
}

bool QtChartView::IsAnimationRunning() const {
    return m_animationRunning;
}

void QtChartView::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    
    QPainter painter(this);
    
    if (m_antialiasing) {
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::TextAntialiasing);
    }
    
    painter.fillRect(rect(), m_backgroundColor);
    
    RenderToPainter(painter);
    
    if (m_gridVisible) {
        DrawGrid(painter);
    }
    
    if (m_scaleBarVisible) {
        DrawScaleBar(painter);
    }
    
    emit RenderCompleted();
}

void QtChartView::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    
    if (m_interactionManager) {
        m_interactionManager->SetViewportSize(event->size().width(), 
                                              event->size().height());
    }
    
    UpdateTransform();
}

void QtChartView::mousePressEvent(QMouseEvent* event) {
    m_lastMousePos = event->pos();
    
    if (m_interactionMode == InteractionMode::kPan && 
        event->button() == Qt::LeftButton) {
        m_isPanning = true;
        setCursor(Qt::ClosedHandCursor);
    }
    
    QWidget::mousePressEvent(event);
}

void QtChartView::mouseReleaseEvent(QMouseEvent* event) {
    if (m_isPanning) {
        m_isPanning = false;
        setCursor(m_panCursor);
    }
    
    QWidget::mouseReleaseEvent(event);
}

void QtChartView::mouseMoveEvent(QMouseEvent* event) {
    if (m_isPanning) {
        QPoint delta = event->pos() - m_lastMousePos;
        Pan(-delta.x(), -delta.y());
        m_lastMousePos = event->pos();
    }
    
    QWidget::mouseMoveEvent(event);
}

void QtChartView::mouseDoubleClickEvent(QMouseEvent* event) {
    Q_UNUSED(event);
    QWidget::mouseDoubleClickEvent(event);
}

void QtChartView::wheelEvent(QWheelEvent* event) {
    double delta = event->angleDelta().y() / 120.0;
    
    if (delta > 0) {
        ZoomIn(1.1);
    } else if (delta < 0) {
        ZoomOut(1.1);
    }
    
    QWidget::wheelEvent(event);
}

void QtChartView::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_Plus:
        case Qt::Key_Equal:
            ZoomIn();
            break;
        case Qt::Key_Minus:
            ZoomOut();
            break;
        case Qt::Key_Left:
            Pan(width() * 0.1, 0);
            break;
        case Qt::Key_Right:
            Pan(-width() * 0.1, 0);
            break;
        case Qt::Key_Up:
            Pan(0, height() * 0.1);
            break;
        case Qt::Key_Down:
            Pan(0, -height() * 0.1);
            break;
        case Qt::Key_R:
            ResetRotation();
            break;
        case Qt::Key_F:
            SetFullExtent();
            break;
        default:
            QWidget::keyPressEvent(event);
            break;
    }
}

void QtChartView::keyReleaseEvent(QKeyEvent* event) {
    QWidget::keyReleaseEvent(event);
}

void QtChartView::focusInEvent(QFocusEvent* event) {
    QWidget::focusInEvent(event);
}

void QtChartView::focusOutEvent(QFocusEvent* event) {
    QWidget::focusOutEvent(event);
}

void QtChartView::enterEvent(QEvent* event) {
    QWidget::enterEvent(event);
}

void QtChartView::leaveEvent(QEvent* event) {
    QWidget::leaveEvent(event);
}

void QtChartView::OnAnimationTimer() {
    refresh();
}

void QtChartView::OnInteractionEvent(const InteractionEvent& event) {
    Q_UNUSED(event);
}

void QtChartView::SetupInteraction() {
    if (!m_eventAdapter || !m_interactionManager) {
        return;
    }
    
    connect(m_eventAdapter.get(), &QtEventAdapter::InteractionEventTriggered,
            this, &QtChartView::OnInteractionEvent);
}

void QtChartView::SetupConnections() {
}

void QtChartView::UpdateTransform() {
    if (m_interactionManager) {
        double halfWidth = width() / (2.0 * m_zoom);
        double halfHeight = height() / (2.0 * m_zoom);
        
        m_extent = Envelope(
            m_center.x - halfWidth,
            m_center.y - halfHeight,
            m_center.x + halfWidth,
            m_center.y + halfHeight
        );
        
        m_interactionManager->SetExtent(m_extent);
    }
}

void QtChartView::RenderToPainter(QPainter& painter) {
    if (!m_drawFacade) {
        return;
    }
    
    DrawContext context;
    context.width = width();
    context.height = height();
    context.extent = m_extent;
    context.zoom = m_zoom;
    context.rotation = m_rotation;
    
    auto result = m_drawFacade->Render(&painter, context);
    if (!result.IsSuccess()) {
        emit RenderFailed(QString::fromStdString(result.GetError().message));
    }
}

void QtChartView::DrawGrid(QPainter& painter) {
    painter.setPen(QPen(QColor(200, 200, 200, 128), 1));
    
    double gridSize = 100.0 / m_zoom;
    double startX = qFloor(m_extent.GetMinX() / gridSize) * gridSize;
    double startY = qFloor(m_extent.GetMinY() / gridSize) * gridSize;
    
    for (double x = startX; x <= m_extent.GetMaxX(); x += gridSize) {
        int screenX = static_cast<int>((x - m_extent.GetMinX()) * m_zoom);
        painter.drawLine(screenX, 0, screenX, height());
    }
    
    for (double y = startY; y <= m_extent.GetMaxY(); y += gridSize) {
        int screenY = static_cast<int>((m_extent.GetMaxY() - y) * m_zoom);
        painter.drawLine(0, screenY, width(), screenY);
    }
}

void QtChartView::DrawScaleBar(QPainter& painter) {
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 10));
    
    double scaleBarLength = 100;
    double worldLength = scaleBarLength / m_zoom;
    
    QString unit = "m";
    if (worldLength >= 1000) {
        worldLength /= 1000;
        unit = "km";
    }
    
    QString text = QString("%1 %2").arg(worldLength, 0, 'f', 1).arg(unit);
    
    int x = 20;
    int y = height() - 30;
    
    painter.drawLine(x, y, x + static_cast<int>(scaleBarLength), y);
    painter.drawLine(x, y - 5, x, y + 5);
    painter.drawLine(x + static_cast<int>(scaleBarLength), y - 5, 
                    x + static_cast<int>(scaleBarLength), y + 5);
    painter.drawText(x, y - 10, text);
}

void QtChartView::HandleSelection(const QPoint& pos, bool addToSelection) {
    Q_UNUSED(pos);
    Q_UNUSED(addToSelection);
}

void QtChartView::HandlePan(const QPoint& pos) {
    Q_UNUSED(pos);
}

void QtChartView::HandleZoom(const QPoint& pos, double delta) {
    Q_UNUSED(pos);
    Q_UNUSED(delta);
}

void QtChartView::HandleMeasure(const QPoint& pos) {
    Q_UNUSED(pos);
}

}  
}  

#endif
