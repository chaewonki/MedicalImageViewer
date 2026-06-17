#include "ui/imageviewer.h"

ImageViewer::ImageViewer(QWidget *parent)
    : QGraphicsView(parent),
    scene(new QGraphicsScene(this)),
    imageItem(nullptr),
    zoomFactor(1.0)
{
    setScene(scene);

    setDragMode(QGraphicsView::ScrollHandDrag);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
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

void ImageViewer::resetView()
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
    } else {
        setDragMode(QGraphicsView::ScrollHandDrag);
        unsetCursor();
    }
}

bool ImageViewer::isWindowLevelMode() const
{
    return windowLevelMode;
}

void ImageViewer::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0)
        zoomIn();
    else
        zoomOut();

    event->accept();
}

void ImageViewer::mousePressEvent(QMouseEvent *event)
{
    if (windowLevelMode && event->button() == Qt::RightButton) {
        draggingWindowLevel = true;
        lastMousePos = event->pos();
        event->accept();
        return;
    }

    QGraphicsView::mousePressEvent(event);
}

void ImageViewer::mouseMoveEvent(QMouseEvent *event)
{
    if (windowLevelMode && draggingWindowLevel) {
        QPoint delta = event->pos() - lastMousePos;
        lastMousePos = event->pos();

        emit windowLevelDragged(delta.x(), delta.y());

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

    QGraphicsView::mouseReleaseEvent(event);
}