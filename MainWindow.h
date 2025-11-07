#ifndef DREAM_MAINWINDOW_H
#define DREAM_MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QDebug>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QGraphicsScene *m_scene;
    QGraphicsView *m_view;
};

#endif //DREAM_MAINWINDOW_H
