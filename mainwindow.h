#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    const float EPSILON = 0.001;

    struct Circle{
        float
            x,
            y,
            r,
            rot;
    };
    struct Ellipse{
        float
            x,
            y,
            rx,
            ry,
            rot = 0;
    };
    struct Rectangle{
        float
            x,
            y,
            w,
            h,
            rot = 0;
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
            rot = 0;
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
    QList<Line> lines;
    QList<Arc> arcs;

    QString width;
    QString height;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void readSvg();
    void writeSvg();
    Point transformPoint(Point &p, float matrix[3][3]);
    Line transformLine(Line &l, float matrix[3][3]);
    void killoverlap(QList<Line> &lines);
    bool areEqual(float &a, float &b);
};
#endif // MAINWINDOW_H
