#include "ui/imageviewer.h"

#include <QBrush>
#include <QPen>
#include <QPainterPath>

ImageViewer::ImageViewer(QWidget *parent)
    : QGraphicsView(parent),
    scene(new QGraphicsScene(this)),
    imageItem(nullptr),
    roiItem(nullptr),
    zoomFactor(1.0)
{
    setScene(scene);

    setDragMode(QGraphicsView::ScrollHandDrag);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    setMouseTracking(true);
    viewport()->setMouseTracking(true);
}

void ImageViewer::setImage(const QImage &image, bool resetViewEnabled)
{
    if (image.isNull())
        return;

    QPixmap pixmap = QPixmap::fromImage(image);

    if (!imageItem) {
        imageItem = scene->addPixmap(pixmap);
        scene->setSceneRect(imageItem->boundingRect());
    } else {
        imageItem->setPixmap(pixmap);
    }

    clearRoi();

    if (resetViewEnabled) {
        scene->setSceneRect(imageItem->boundingRect());
        resetTransform();
        zoomFactor = 1.0;
        fitToWindow();
    }
}

void ImageViewer::zoomIn()
{
    scale(1.25, 1.25);
    zoomFactor *= 1.25;
}

void ImageViewer::zoomOut()
{
    scale(0.8, 0.8);
    zoomFactor *= 0.8;
}

void ImageViewer::fitToWindow()
{
    if (!imageItem)
        return;

    fitInView(imageItem, Qt::KeepAspectRatio);
}

void ImageViewer::actualSize()
{
    resetTransform();
    zoomFactor = 1.0;
}

void ImageViewer::setWindowLevelMode(bool enabled)
{
    windowLevelMode = enabled;
    draggingWindowLevel = false;

    if (windowLevelMode) {
        setDragMode(QGraphicsView::NoDrag);
        setCursor(Qt::CrossCursor);
    } else if(!roiMode) {
        setDragMode(QGraphicsView::ScrollHandDrag);
        unsetCursor();
    }
}

bool ImageViewer::isWindowLevelMode() const
{
    return windowLevelMode;
}

void ImageViewer::setRoiMode(bool enabled)
{
    roiMode = enabled;
    drawingRoi = false;

    if (roiMode) {
        windowLevelMode = false;
        draggingWindowLevel = false;
        setDragMode(QGraphicsView::NoDrag);
        setCursor(Qt::CrossCursor);
    } else if (!windowLevelMode) {
        setDragMode(QGraphicsView::ScrollHandDrag);
        unsetCursor();
    }
}

bool ImageViewer::isRoiMode() const
{
    return roiMode;
}

void ImageViewer::clearRoi()
{
    if (!roiItem)
        return;

    scene->removeItem(roiItem);
    delete roiItem;
    roiItem = nullptr;
}

void ImageViewer::setMagnifierMode(bool enabled)
{
    magnifierMode = enabled;

    if (magnifierMode) {
        roiMode = false;
        windowLevelMode = false;
        draggingWindowLevel = false;
        drawingRoi = false;

        setDragMode(QGraphicsView::NoDrag);
        setCursor(Qt::BlankCursor);
    } else {
        hideMagnifier();

        if (!roiMode && !windowLevelMode) {
            setDragMode(QGraphicsView::ScrollHandDrag);
            unsetCursor();
        }
    }
}

bool ImageViewer::isMagnifierMode() const
{
    return magnifierMode;
}

void ImageViewer::hideMagnifier()
{
    if (magnifierItem)
        magnifierItem->hide();

    if (magnifierBorderItem)
        magnifierBorderItem->hide();
}

void ImageViewer::updateMagnifier(const QPoint &viewPos)
{
    if (!magnifierMode || !imageItem)
        return;

    QPointF scenePos = mapToScene(viewPos);

    if (!imageItem->boundingRect().contains(scenePos)) {
        hideMagnifier();
        return;
    }

    QPixmap sourcePixmap = imageItem->pixmap();
    if (sourcePixmap.isNull())
        return;

    const int sourceSize = static_cast<int>(magnifierSize / magnifierScale);

    QRectF sourceRect(
        scenePos.x() - sourceSize / 2.0,
        scenePos.y() - sourceSize / 2.0,
        sourceSize,
        sourceSize
        );

    sourceRect = sourceRect.intersected(imageItem->boundingRect());

    if (sourceRect.width() <= 1 || sourceRect.height() <= 1) {
        hideMagnifier();
        return;
    }

    QPixmap cropped = sourcePixmap.copy(sourceRect.toRect());
    QPixmap scaled = cropped.scaled(
        magnifierSize,
        magnifierSize,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    if (!magnifierItem) {
        magnifierItem = scene->addPixmap(scaled);
        magnifierItem->setZValue(100.0);
    } else {
        magnifierItem->setPixmap(scaled);
    }

    QRectF targetRect(
        scenePos.x() + 20,
        scenePos.y() + 20,
        magnifierSize,
        magnifierSize
        );

    magnifierItem->setPos(targetRect.topLeft());
    magnifierItem->show();

    QPainterPath clipPath;
    clipPath.addEllipse(QRectF(0, 0, magnifierSize, magnifierSize));
    magnifierItem->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);

    if (!magnifierBorderItem) {
        magnifierBorderItem = scene->addEllipse(
            QRectF(0, 0, magnifierSize, magnifierSize),
            QPen(Qt::yellow, 2),
            QBrush(Qt::NoBrush)
            );
        magnifierBorderItem->setZValue(101.0);
    }

    magnifierBorderItem->setRect(targetRect);
    magnifierBorderItem->show();
}

void ImageViewer::wheelEvent(QWheelEvent *event)
{
    if (magnifierMode) {
        if (event->angleDelta().y() > 0)
            magnifierScale += 0.25;
        else
            magnifierScale -= 0.25;

        if (magnifierScale < 1.25)
            magnifierScale = 1.25;

        if (magnifierScale > 5.0)
            magnifierScale = 5.0;

        updateMagnifier(event->position().toPoint());
        event->accept();
        return;
    }

    if (event->angleDelta().y() > 0)
        zoomIn();
    else
        zoomOut();

    event->accept();
}

void ImageViewer::mousePressEvent(QMouseEvent *event)
{
    if (magnifierMode) {
        updateMagnifier(event->pos());
        event->accept();
        return;
    }

    if (windowLevelMode && event->button() == Qt::RightButton) {
        draggingWindowLevel = true;
        lastMousePos = event->pos();
        event->accept();
        return;
    }

    if (roiMode && event->button() == Qt::LeftButton && imageItem) {
        drawingRoi = true;
        roiStartScenePos = mapToScene(event->pos());

        clearRoi();

        roiItem = scene->addRect(
            QRectF(roiStartScenePos, roiStartScenePos),
            QPen(Qt::yellow, 6, Qt::DashLine),
            QBrush(Qt::NoBrush)
            );
        roiItem->setZValue(10.0);

        event->accept();
        return;
    }

    QGraphicsView::mousePressEvent(event);
}

void ImageViewer::mouseMoveEvent(QMouseEvent *event)
{
    if (magnifierMode) {
        updateMagnifier(event->pos());
        event->accept();
        return;
    }

    if (windowLevelMode && draggingWindowLevel) {
        QPoint delta = event->pos() - lastMousePos;
        lastMousePos = event->pos();

        emit windowLevelDragged(delta.x(), delta.y());

        event->accept();
        return;
    }

    if (roiMode && drawingRoi && roiItem) {
        QPointF currentScenePos = mapToScene(event->pos());
        QRectF roiRect(roiStartScenePos, currentScenePos);
        roiItem->setRect(roiRect.normalized());

        event->accept();
        return;
    }

    QGraphicsView::mouseMoveEvent(event);
}

void ImageViewer::mouseReleaseEvent(QMouseEvent *event)
{
    if (windowLevelMode && event->button() == Qt::RightButton) {
        draggingWindowLevel = false;
        event->accept();
        return;
    }

    if (roiMode && drawingRoi && event->button() == Qt::LeftButton && imageItem) {
        drawingRoi = false;

        QRectF roiRect = roiItem->rect().normalized();
        roiRect = roiRect.intersected(imageItem->boundingRect());

        if (roiRect.width() < 1.0 || roiRect.height() < 1.0) {
            clearRoi();
            event->accept();
            return;
        }

        roiItem->setRect(roiRect);
        emit roiSelected(roiRect.toAlignedRect());

        event->accept();
        return;
    }

    QGraphicsView::mouseReleaseEvent(event);
}