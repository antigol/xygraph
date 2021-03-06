#include <QApplication>
#include "xygraph.hh"
#include <cmath>

class Sinusoidal : public XY::Function {
public :
	qreal y(qreal x) override
	{
        return std::sin(x);
	}

};

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

    QList<QPointF> points;
    for (int i = 0; i < 1000; ++i) {
        points << QPointF(20.0 * i / 1000 - 10.0, 2.0 * qrand() / RAND_MAX - 1.0);
    }

    XY::Graph graph;

    XY::PointList pointlist(points);
    pointlist.dotBrush = QBrush(Qt::NoBrush);
    pointlist.dotPen = QPen(Qt::yellow, 2.0);

    Sinusoidal sin;
    sin.pen = QPen(Qt::red, 10.0, Qt::DotLine);

    graph.pointLists << &pointlist;
    graph.functions << &sin;

    graph.autoZoom();
    graph.relativeZoom(1.1);

    graph.show();

	return a.exec();
}
