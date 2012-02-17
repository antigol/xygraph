#include <QApplication>
#include "xygraph.hpp"
#include "xyscene.hpp"
#include <cmath>

class FunyFunction : public XYFunction {
public:
    FunyFunction()
    {
        setPen(QPen(QBrush(Qt::blue, Qt::Dense5Pattern), 10));
    }

    qreal y(qreal x) const
    {
        return std::pow(std::sin(x) + 1.0, std::cos(x) + 1.0);
    }
    bool domain(qreal x) const
    {
        return (x < -1.0) || (x > 1.0);
    }
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
    FunyFunction f;

    XYSPline spl;
    spl.addPoint(-3, -5);
    spl.addPoint(1, -2);
    spl.addPoint(4, -6);

    // crée la vue
    XYGraph graph;

    // crée la scène
    XYScene scene;

    scene.setBackgroundBrush(QBrush(Qt::white));
    scene.setSubaxesPen(Qt::NoPen);
    scene.setAxesPen(QPen());
    scene.setZoomPen(QPen(Qt::darkGreen));
    scene.setTextColor(QColor("red"));

    scene.setState(scene.state() | XYScene::ShowPointPosition);

    // ajoute le nuage de points
    scene.addScatterplot(&s);
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
