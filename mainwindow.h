#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class ImageViewer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openDicom();

private:
    ImageViewer *viewer;

    void createMenu();
    void createToolBar();
    void createRightPanel();
};

#endif