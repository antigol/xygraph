#include <QApplication>
#include "xygraph.hpp"
#include "xyscene.hpp"
#include <cmath>
#include <QDebug>
#include <marioutil/pointmap.h>

class FunyFunction : public XYFunction {
public:
    FunyFunction(XYSPline *spline, PointMap *sun) :
        _spline(spline), _sun(sun)
    {
        setPen(QPen(Qt::blue));
    }

    qreal y(qreal x) const
    {
        return _spline->spline(x) * _sun->interpolate(x);
    }
    bool domain(qreal x) const
    {
        return (x >= _spline->xMinimum()) && (x <= _spline->xMaximum());
    }
private:
    XYSPline *_spline;
    PointMap *_sun;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // crée un nuage de point aléatoirement
    PointMap dsun;
    dsun.loadFile("AM1.5_global_nm.txt");
    double max = dsun.yMaximum();
    dsun /= max;
    XYScatterplot sun(dsun.toPointList(), QPen(), QBrush(), 1.0, QPen(Qt::black));



    // crée une funyfunction (décarée plus haut dans le main.cpp)
    XYSPline spl(QPen(Qt::red), QBrush(Qt::blue), 2.0, QPen(Qt::red));
    FunyFunction f(&spl, &dsun);

    // crée la vue
    XYGraph graph;

    // crée la scène
    XYScene scene;

//    a.connect(&scene, SIGNAL(splineChanged()), &a, SLOT(aboutQt()));


    scene.setBackgroundBrush(QBrush(Qt::white));
    scene.setSubaxesPen(Qt::NoPen);
    scene.setAxesPen(QPen());
    scene.setZoomPen(QPen(Qt::darkGreen));
    scene.setTextColor(QColor("red"));

    scene.setState(scene.state() | XYScene::ShowPointPosition);

    // ajoute le nuage de points
    scene.addScatterplot(&sun);
    scene.addFunction(&f);
    scene.addSpline(&spl);
    scene.setCurrentSpline(&spl);

    scene.autoZoom();
    scene.relativeZoom(1.2);

    // donne la scène à la vue
    graph.setScene(&scene);
    graph.setWindowTitle("Test 1");
    graph.show();
    graph.resize(600, 600);

    return a.exec();
}
