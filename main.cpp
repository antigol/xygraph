#include <QApplication>
#include "xygraph.hpp"
#include "xyscene.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    XYGraph graph;
    XYScene scene;
    graph.setScene(&scene);
    graph.show();

    return a.exec();
}
