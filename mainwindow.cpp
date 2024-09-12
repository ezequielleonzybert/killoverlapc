#include "mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QXmlStreamWriter>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    readSvg();
    killoverlap(lines);
    writeSvg();
}

MainWindow::~MainWindow() {}

void MainWindow::readSvg()
{
    QFile file(R"(../../test.svg)");
    file.open(QIODeviceBase::ReadOnly);
    QXmlStreamReader xmlReader(&file);
    QXmlStreamAttributes a;
    static QRegularExpression regex(R"(rotate\(([^)]*)\))");

    while(!xmlReader.atEnd()){
        xmlReader.readNext();
        if (xmlReader.tokenType() == QXmlStreamReader::StartElement){
            if(xmlReader.name().toString() == "svg"){
                width = xmlReader.attributes().value("width").toString();
                height = xmlReader.attributes().value("height").toString();
            }
            if(xmlReader.name().toString() == "circle"){
                a = xmlReader.attributes();
                Circle c;
                c.x = a.value("cx").toFloat();
                c.y = a.value("cy").toFloat();
                c.r = a.value("r").toFloat();
                if(a.hasAttribute("transform"))
                    c.rot = regex.match(a.value("transform")).captured(1).toFloat();
                circles.append(c);
            }
            else if(xmlReader.name().toString() == "ellipse"){
                a = xmlReader.attributes();
                Ellipse e;
                e.x = a.value("cx").toFloat();
                e.y = a.value("cy").toFloat();
                e.rx = a.value("rx").toFloat();
                e.ry = a.value("ry").toFloat();
                if(a.hasAttribute("transform"))
                    e.rot = regex.match(a.value("transform")).captured(1).toFloat();
                ellipses.append(e);
            }
            else if(xmlReader.name().toString() == "rect"){
                a = xmlReader.attributes();
                Rectangle r;
                r.x = a.value("x").toFloat();
                r.y = a.value("y").toFloat();
                r.w = a.value("width").toFloat();
                r.h = a.value("height").toFloat();
                if(a.hasAttribute("transform"))
                    r.rot = regex.match(a.value("transform")).captured(1).toFloat();

                float matrix[3][3];
                float angle = r.rot * M_PI / 180;

                matrix[0][0] = cos(angle);
                matrix[0][1] = -sin(angle);
                matrix[0][2] = 0;
                matrix[1][0] = sin(angle);
                matrix[1][1] = cos(angle);
                matrix[1][2] = 0;
                matrix[2][0] = 0;
                matrix[2][1] = 0;
                matrix[2][2] = 1;

                Line l;
                l.a = {r.x, r.y};
                l.b = {r.x + r.w, r.y};
                lines.append(transformLine(l, matrix));
                l.a = {r.x + r.w, r.y};
                l.b = {r.x + r.w, r.y + r.h};
                lines.append(transformLine(l, matrix));
                l.a = {r.x + r.w, r.y + r.h};
                l.b = {r.x, r.y + r.h};
                lines.append(transformLine(l, matrix));
                l.a = {r.x, r.y + r.h};
                l.b = {r.x, r.y};
                lines.append(transformLine(l, matrix));
            }
            else if(xmlReader.name().toString() == "path"){
                QString pathCode = xmlReader.attributes().value("d").toString();
                static QRegularExpression regexSplit(R"(\s+|,+)");
                QStringList pathCodeSplit = pathCode.split(regexSplit);
                QChar command;
                QList<Point> points;
                Point firstPoint;
                firstPoint.x = pathCodeSplit[1].toFloat();
                firstPoint.y = pathCodeSplit[2].toFloat();
                float x = 0;
                float y = 0;
                int i = 0;
                while (i < pathCodeSplit.size()) {
                    if (pathCodeSplit[i].size() == 1 && QString("mMaAlLzZ").contains(pathCodeSplit[i][0])) {
                        command = pathCodeSplit[i][0];
                        i++;
                    }
                    if(command == 'm' || command == 'l'){
                        Point p;
                        p.x = x + pathCodeSplit[i].toFloat();
                        i++;
                        p.y = y + pathCodeSplit[i].toFloat();
                        points.append(p);
                        x = p.x;
                        y = p.y;
                    }
                    if(command == 'M' || command == 'L'){
                        Point p;
                        p.x = pathCodeSplit[i].toFloat();
                        i++;
                        p.y = pathCodeSplit[i].toFloat();
                        points.append(p);
                        x = p.x;
                        y = p.y;
                    }
                    if(command == 'a'){
                        if(points.size() > 0){
                            for(int j = 0; j < points.size() - 1; j++){
                                Line l;
                                l.a = points[j];
                                l.b = points[j+1];
                                lines.append(l);
                            }
                            points.clear();
                        }

                        Arc a;
                        a.a = {x,y};
                        a.rx = pathCodeSplit[i].toFloat();
                        i++;
                        a.ry = pathCodeSplit[i].toFloat();
                        i++;
                        a.rot = pathCodeSplit[i].toFloat();
                        i++;
                        a.large = pathCodeSplit[i].toInt();
                        i++;
                        a.sweep = pathCodeSplit[i].toInt();
                        i++;
                        a.b.x = x + pathCodeSplit[i].toFloat();
                        i++;
                        a.b.y = y + pathCodeSplit[i].toFloat();

                        arcs.append(a);

                        x = a.b.x;
                        y = a.b.y;

                        Point p;
                        p.x = x;
                        p.y = y;
                        points.append(p);
                    }
                    if(command == 'A'){
                        if(points.size() > 0){
                            for(int j = 0; j < points.size()-1; j++){
                                Line l;
                                l.a = points[j];
                                l.b = points[j+1];
                                lines.append(l);
                            }
                            points.clear();
                            //THIS IS WRONG, CAN't CLEAR POINTS BEFORE Z
                            //because z command create another line that needs the point 0 of a path
                        }

                        Arc a;
                        a.a = {x,y};
                        a.rx = pathCodeSplit[i].toFloat();
                        i++;
                        a.ry = pathCodeSplit[i].toFloat();
                        i++;
                        a.rot = pathCodeSplit[i].toFloat();
                        i++;
                        a.large = pathCodeSplit[i].toInt();
                        i++;
                        a.sweep = pathCodeSplit[i].toInt();
                        i++;
                        a.b.x = pathCodeSplit[i].toFloat();
                        i++;
                        a.b.y = pathCodeSplit[i].toFloat();

                        arcs.append(a);

                        x = a.b.x;
                        y = a.b.y;

                        Point p;
                        p.x = x;
                        p.y = y;
                        points.append(p);
                    }
                    if(command == 'z' || command == 'Z'){
                        if(firstPoint.x != x && firstPoint.y != y)
                        {
                            points.append(firstPoint);
                            for(int j = 0; j < points.size() - 1; j++){
                                Line l;
                                l.a = points[j];
                                l.b = points[j+1];
                                lines.append(l);
                            }
                            points.clear();
                        }
                    }
                    if(pathCodeSplit.size()-1 == i ){
                        for(int j = 0; j < points.size() - 1; j++){
                            Line l;
                            l.a = points[j];
                            l.b = points[j+1];
                            lines.append(l);
                        }
                        x = 0;
                        y = 0;
                    }
                    i++;
                }
            }
        }
    }
}

void MainWindow::writeSvg()
{
    QFile file("C:/Users/Ezequiel/Desktop/output.svg");
    file.open(QIODeviceBase::WriteOnly);
    QXmlStreamWriter xmlWriter(&file);

    xmlWriter.setAutoFormatting(true);

    xmlWriter.writeStartElement("svg");
    xmlWriter.writeAttribute("xmlns", "http://www.w3.org/2000/svg");
    xmlWriter.writeAttribute("width", width);
    xmlWriter.writeAttribute("height", height);
    xmlWriter.writeAttribute("viewBox", "0 0 " + width.remove("mm") + " " + height.remove("mm"));

    xmlWriter.writeStartElement("style");
    xmlWriter.writeCharacters("* { fill: none; stroke: black; stroke-width: 0.25; }");
    xmlWriter.writeEndElement();

    for(Line &l : lines){
        xmlWriter.writeStartElement("line");
        xmlWriter.writeAttribute("x1", QString::number(l.a.x));
        xmlWriter.writeAttribute("y1", QString::number(l.a.y));
        xmlWriter.writeAttribute("x2", QString::number(l.b.x));
        xmlWriter.writeAttribute("y2", QString::number(l.b.y));
        xmlWriter.writeEndElement();
    }

    for(Arc &a : arcs){
        xmlWriter.writeStartElement("path");
        QString pathCode =
            "M " +
            QString::number(a.a.x) + " " +
            QString::number(a.a.y) + " " +
            "A " +
            QString::number(a.rx) + " " +
            QString::number(a.ry) + " " +
            QString::number(a.rot) + " " +
            QString::number(a.large) + " " +
            QString::number(a.sweep) + " " +
            QString::number(a.b.x) + " " +
            QString::number(a.b.y) + " ";
        xmlWriter.writeAttribute("d", pathCode);
        xmlWriter.writeEndElement();
    }

    for(Ellipse &e : ellipses){
        xmlWriter.writeStartElement(("ellipse"));
        xmlWriter.writeAttribute("cx", QString::number(e.x));
        xmlWriter.writeAttribute("cy", QString::number(e.y));
        xmlWriter.writeAttribute("rx", QString::number(e.rx));
        xmlWriter.writeAttribute("ry", QString::number(e.ry));
        if(e.rot){
            xmlWriter.writeAttribute("transform", "rotate(" + QString::number(e.rot) + ")");
        }
        xmlWriter.writeEndElement();
    }

    for(Circle &c : circles){
        xmlWriter.writeStartElement(("circle"));
        xmlWriter.writeAttribute("cx", QString::number(c.x));
        xmlWriter.writeAttribute("cy", QString::number(c.y));
        xmlWriter.writeAttribute("r", QString::number(c.r));
        if(c.rot){
            xmlWriter.writeAttribute("transform", "rotate(" + QString::number(c.rot) + ")");
        }
        xmlWriter.writeEndElement();

    }
    xmlWriter.writeEndElement();
}

MainWindow::Point MainWindow::transformPoint(Point &p, float matrix[3][3]){
    Point result;
    result.x = p.x * matrix[0][0] + p.y * matrix[0][1] + matrix[0][2];
    result.y = p.x * matrix[1][0] + p.y * matrix[1][1] + matrix[1][2];
    return result;
}

MainWindow::Line MainWindow::transformLine(Line &l, float matrix[3][3]){
    Line result;
    result.a = transformPoint(l.a, matrix);
    result.b = transformPoint(l.b, matrix);
    return result;
}

bool MainWindow::areEqual(float &a, float &b){
    return abs(a-b) < EPSILON;
}

void MainWindow::killoverlap(QList<Line> &lines)
{
    int counter = 0;
    for(int i = 0; i < lines.size(); i++){
        float m1 = (lines[i].b.y - lines[i].a.y) / ((lines[i].b.x - lines[i].a.x));
        float b1 = lines[i].a.y - m1 * lines[i].a.x;
        for(int j = i + 1; j < lines.size(); j++){
            float m2 = (lines[j].b.y - lines[j].a.y) / ((lines[j].b.x - lines[j].a.x));
            float b2 = lines[j].a.y - m2 * lines[j].a.x;
            if(areEqual(m1, m2) && areEqual(b1, b2)){
                counter ++;
            }
        }
    }
    qDebug() << counter;
}
