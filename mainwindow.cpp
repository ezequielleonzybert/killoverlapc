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
    writeSvg();
    qDebug() << lines.size();
}

MainWindow::~MainWindow() {}

void MainWindow::readSvg()
{
    QFile file(R"(../../test.svg)");
    file.open(QIODeviceBase::ReadOnly);
    QXmlStreamReader xml(&file);
    QXmlStreamAttributes a;
    static QRegularExpression regex(R"(rotate\(([^)]*)\))");

    while(!xml.atEnd()){
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement){
            if(xml.name().toString() == "svg"){
                width = xml.attributes().value("width").toString();
                height = xml.attributes().value("height").toString();
            }
            if(xml.name().toString() == "circle"){
                a = xml.attributes();
                Circle c;
                c.x = a.value("cx").toFloat();
                c.y = a.value("cy").toFloat();
                c.r = a.value("r").toFloat();
                circles.append(c);
            }
            else if(xml.name().toString() == "ellipse"){
                a = xml.attributes();
                Ellipse e;
                e.x = a.value("cx").toFloat();
                e.y = a.value("cy").toFloat();
                e.rx = a.value("rx").toFloat();
                e.ry = a.value("ry").toFloat();
                if(a.hasAttribute("transform"))
                    e.rot = regex.match(a.value("transform")).captured(1).toFloat();
                ellipses.append(e);
            }
            else if(xml.name().toString() == "rect"){
                a = xml.attributes();
                Rectangle r;
                r.x = a.value("x").toFloat();
                r.y = a.value("y").toFloat();
                r.w = a.value("width").toFloat();
                r.h = a.value("height").toFloat();
                if(a.hasAttribute("transform"))
                    r.rot = regex.match(a.value("transform")).captured(1).toFloat();
                rectangles.append(r);
            }
            else if(xml.name().toString() == "path"){
                QString pathCode = xml.attributes().value("d").toString();
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

    xmlWriter.writeEndElement();
}
