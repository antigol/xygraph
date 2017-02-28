# XYGraph

    XYScene scene;

    // add scatter points
    QList<QPointF> points;
    points << QPointF(0.0, 0.0) << QPointF(0.5, 1.0) << QPointF(1.0, 1.0) << QPointF(1.045, 1.05);

    XYScatterplot my_scatterplot(points, QPen(), QBrush(), 5.0, QPen(Qt::red));
    scene.addScatterplot(&my_scatterplot);

    scene.autoZoom();

    // create the view
    XYGraph graph(&scene);
    graph.setWindowTitle("Test 1");
    graph.show();
