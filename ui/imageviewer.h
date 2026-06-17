#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPoint>

class ImageViewer : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ImageViewer(QWidget *parent = nullptr);

    void setImage(const QImage &image, bool resetView = true);

    void zoomIn();
    void zoomOut();
    void fitToWindow();
    void resetView();

    void setWindowLevelMode(bool enabled);
    bool isWindowLevelMode() const;

signals:
    void windowLevelDragged(int deltaX, int deltaY);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QGraphicsScene *scene;
    QGraphicsPixmapItem *imageItem;
    double zoomFactor;

    bool windowLevelMode = false;
    bool draggingWindowLevel = false;
    QPoint lastMousePos;
};