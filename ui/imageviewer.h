#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPoint>
#include <QRect>
#include <QGraphicsEllipseItem>

class ImageViewer : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ImageViewer(QWidget *parent = nullptr);

    void setImage(const QImage &image, bool resetView = true);

    void zoomIn();
    void zoomOut();
    void fitToWindow();
    void actualSize();

    void setWindowLevelMode(bool enabled);
    bool isWindowLevelMode() const;

    void setRoiMode(bool enabled);
    bool isRoiMode() const;
    void clearRoi();

    void setMagnifierMode(bool enabled);
    bool isMagnifierMode() const;

signals:
    void windowLevelDragged(int deltaX, int deltaY);
    void roiSelected(const QRect &roiRect);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QGraphicsScene *scene;
    QGraphicsPixmapItem *imageItem;
    QGraphicsRectItem *roiItem;
    double zoomFactor;

    bool windowLevelMode = false;
    bool draggingWindowLevel = false;
    QPoint lastMousePos;

    bool roiMode = false;
    bool drawingRoi = false;
    QPointF roiStartScenePos;

    bool magnifierMode = false;
    QGraphicsPixmapItem *magnifierItem = nullptr;
    QGraphicsEllipseItem *magnifierBorderItem = nullptr;

    int magnifierSize = 450;
    double magnifierScale = 2.0;

    void updateMagnifier(const QPoint &viewPos);
    void hideMagnifier();
};