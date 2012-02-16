#include <QApplication>
#include "xygraph.hpp"
#include "xyscene.hpp"

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

    // crée la vue
    XYGraph graph;

    // crée la scène
    XYScene scene;
    scene.setBackgroundBrush(QBrush(Qt::blue, Qt::FDiagPattern));
    scene.setSubaxesPen(Qt::NoPen);
    scene.setAxesPen(QPen());
    scene.setTextColor(QColor("red"));

    // ajoute le nuage de points
    scene.addScatterplot(&s);
    scene.autoZoom();
    scene.relativeZoom(1.2);

    // donne la scène à la vue
    graph.setScene(&scene);
    graph.setWindowTitle("Test 1");
    graph.show();

    return a.exec();
}
