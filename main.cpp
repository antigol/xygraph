#include <QApplication>
#include "xygraph.hpp"
#include "xyscene.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // crée un nuage de point aléatoirement
    XYScatterplot s(QPen(), QBrush(), 2.0, QPen());
    for (int i = 0; i < 10; ++i) {
        s << QPointF(qrand()%10, qrand()%10);
    }

    // crée la vue
    XYGraph graph;

    // crée la scène
    XYScene scene;

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
