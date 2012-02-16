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
        s << QPointF(qrand()%11-5, qrand()%11-5);
    }

    // crée une funyfunction (décarée plus haut dans le main.cpp)
    FunyFunction f;

    // crée la vue
    XYGraph graph;

    // crée la scène
    XYScene scene;

    QRadialGradient radialGrad(QPointF(100, 100), 100);
    radialGrad.setColorAt(0, Qt::red);
    radialGrad.setColorAt(0.5, Qt::yellow);
    radialGrad.setColorAt(1, Qt::white);

    scene.setBackgroundBrush(QBrush(radialGrad));
    scene.setSubaxesPen(Qt::NoPen);
    scene.setAxesPen(QPen());
    scene.setTextColor(QColor("red"));

    // ajoute le nuage de points
    scene.addScatterplot(&s);
    scene.addFunction(&f);
    scene.autoZoom();
    scene.relativeZoom(1.2);

    // donne la scène à la vue
    graph.setScene(&scene);
    graph.setWindowTitle("Test 1");
    graph.show();

    return a.exec();
}
