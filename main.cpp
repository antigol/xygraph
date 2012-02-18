#include <QApplication>
#include "xygraph.hpp"
#include "xyscene.hpp"
#include <cmath>
#include <QDebug>

class FunyFunction : public XYFunction {
public:
    FunyFunction(XYSPline *spline) :
        _spline(spline)
    {
        setPen(QPen(Qt::blue));
    }

    qreal y(qreal x) const
    {
        return _spline->spline(x) * 2.0;
    }
    bool domain(qreal x) const
    {
        return (x >= _spline->xMinimum()) && (x <= _spline->xMaximum());
    }
private:
    XYSPline *_spline;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // crée un nuage de point aléatoirement
    XYScatterplot s(QPen(),
                    QBrush(Qt::red),
                    4.0,
                    QPen(QBrush(Qt::yellow), 4,
                         Qt::DashLine, Qt::RoundCap, Qt::BevelJoin));

    qsrand(a.applicationPid());
    for (int i = 0; i < 10; ++i) {
        s << QPointF(qrand() % 11 - 5, qrand() % 11 - 5);
    }

    // crée une funyfunction (décarée plus haut dans le main.cpp)
    XYSPline spl;
    FunyFunction f(&spl);

    spl.addPoint(-3, -5);
    spl.addPoint(1, -2);
    spl.addPoint(4, -6);

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
//    scene.addScatterplot(&s);
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
