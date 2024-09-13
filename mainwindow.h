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

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    void readSvg(QString filePath);
    void writeSvg(QString outPath);
    Point transformPoint(Point &p, float matrix[3][3]);
    Line transformLine(Line &l, float matrix[3][3]);
    void killoverlap(QList<Line> &lines);
    bool areEqual(float &a, float &b);
    bool overlappingLines(Line &l1, Line &l2);
    std::pair<Point, Point> overlappingEnds(const Point &p1, const Point &p2, const Point &p3, const Point &p4);
};
#endif // MAINWINDOW_H
