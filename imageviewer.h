#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>
#include <QImage>

#include <vector>
#include <cstdint>

class ImageViewer : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ImageViewer(QWidget *parent = nullptr);

    void setImage(const QImage &image);
    void setImage16(const std::vector<uint16_t> &pixels, int width, int height);

    void zoomIn();
    void zoomOut();
    void fitToWindow();
    void resetView();

    void setWindowLevel(int windowWidth, int windowCenter);

protected:
    void wheelEvent(QWheelEvent *event) override;

private:
    QGraphicsScene *scene;
    QGraphicsPixmapItem *imageItem;

    double zoomFactor;

    std::vector<uint16_t> pixels16;
    int imageWidth;
    int imageHeight;

    int windowWidth;
    int windowCenter;

    QImage displayImage;

    void updateDisplayImage();
    void updatePixmap();
};

#endif // IMAGEVIEWER_H
