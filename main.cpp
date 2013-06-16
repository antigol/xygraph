#include <QApplication>
#include "xygraph.hpp"
#include "xyscene.hpp"
#include <cmath>
#include <QDebug>

class Sinusoidal : public XYFunction {
public :
    qreal y(qreal x)
    {
        return std::pow(std::sin(x),3);
    }

};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Sinusoidal sinus;

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

    scene.setFlag(XYScene::ShowPointPosition);

    // ajoute le nuage de points
//    scene.addScatterplot(&sun);
    scene.addFunction(&sinus);
//    scene.addFunction(&s);
//    scene.addSpline(&spline);
//    scene.setCurrentSpline(&spline);

    scene.autoZoom();
    scene.relativeZoom(1.2);

    // donne la scène à la vue
    graph.setScene(&scene);
    graph.setWindowTitle("Test 1");
    graph.show();
    graph.resize(600, 600);

    return a.exec();
}
