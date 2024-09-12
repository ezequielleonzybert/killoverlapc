#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    struct Circle{
        float
            x,
            y,
            r;
    };
    struct Ellipse{
        float
            x,
            y,
            rx,
            ry,
            rot;
    };
    struct Rectangle{
        float
            x,
            y,
            w,
            h,
            rot;
    };
    struct Point{
        float x, y;
    };
    struct Line{
        Point a, b;
    };
    struct Arc{
        float
            rx,
            ry,
            rot;
        int
            large,
            sweep;
        Point
            a,
            b;
    };
    struct Path{
        QList<Line> lines;
        QList<Arc> arcs;
    };

    QList<Circle> circles;
    QList<Ellipse> ellipses;
    QList<Rectangle> rectangles;
    QList<Line> lines;
    QList<Arc> arcs;

    QString width;
    QString height;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void readSvg();
    void writeSvg();
};
#endif // MAINWINDOW_H
