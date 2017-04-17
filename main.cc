#include <QApplication>
//#include "xyview.hh"
//#include "xyscene.hh"
#include "xygraph.hh"
#include <cmath>
#include <QDebug>

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
    for (int i = 0; i < 10000; ++i) {
        points << QPointF(qreal(i) / 10000, qreal(qrand()) / RAND_MAX - 0.5);
    }

    /*
	XYScene scene;
	scene.setBackgroundBrush(QBrush(Qt::white));
	scene.setSubaxesPen(Qt::NoPen);
	scene.setAxesPen(QPen());
	scene.setZoomPen(QPen(Qt::darkGreen));
	scene.setTextColor(QColor("red"));
	scene.setFlag(XYScene::ShowPointPosition);

	// add scatter points

	XYPointList my_pointlist(points, QPen(), QBrush(), 5.0, QPen(Qt::red));
	scene.addPointList(&my_pointlist);

	// add function
	Sinusoidal sinus;
	scene.addFunction(&sinus);

	scene.autoZoom();
	scene.relativeZoom(1.2);

	// create the view
	XYView graph(&scene);
	graph.setWindowTitle("Test 1");
	graph.show();
	graph.resize(600, 600);
    */

    XY::Graph graph;

    XY::PointList pointlist(points);
    Sinusoidal sin;

    graph.addPointList(&pointlist);
    graph.addFunction(&sin);
    graph.autoZoom();

    graph.show();

	return a.exec();
}
