#include "imageviewer.h"

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

void ImageViewer::setImage(const QImage &image){
    scene->clear();

    imageItem = scene->addPixmap(QPixmap::fromImage(image));
    scene->setSceneRect(imageItem->boundingRect());

    resetTransform();
    zoomFactor = 1.0;

    fitToWindow();
}

void ImageViewer::zoomIn(){
    scale(1.25, 1.25);
    zoomFactor *= 1.25;
}

void ImageViewer::zoomOut(){
    scale(0.8, 0.8);
    zoomFactor *= 0.8;
}

void ImageViewer::fitToWindow(){
    if (!imageItem)
        return;

    fitInView(imageItem, Qt::KeepAspectRatio);
}

void ImageViewer::resetView(){
    resetTransform();
    zoomFactor = 1.0;
}

void ImageViewer::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0)
        zoomIn();
    else
        zoomOut();
}

