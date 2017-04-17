#include <QApplication>
#include "xyview.hh"
#include "xyscene.hh"
#include <cmath>
#include <QDebug>

class Sinusoidal : public XYFunction {
public :
	qreal y(qreal x) override
	{
		return std::pow(std::sin(x),3);
	}

};

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	XYScene scene;
	scene.setBackgroundBrush(QBrush(Qt::white));
	scene.setSubaxesPen(Qt::NoPen);
	scene.setAxesPen(QPen());
	scene.setZoomPen(QPen(Qt::darkGreen));
	scene.setTextColor(QColor("red"));
	scene.setFlag(XYScene::ShowPointPosition);

	// add scatter points
	QList<QPointF> points;
	points << QPointF(0.0, 0.0) << QPointF(0.5, 1.0) << QPointF(1.0, 1.0) << QPointF(1.045, 1.05);

    for (int i = 0; i < 10000; ++i) {
        points << QPointF(i, qrand());
    }

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

	return a.exec();
}
