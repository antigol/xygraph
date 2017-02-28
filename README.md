# XYGraph

    XYGraph graph;
    XYScene scene;
    
    QList<QPointF> points;
    points << QPointF(0.0, 0.0) << QPointF(0.5, 1.0) << QPointF(1.0, 1.0) << QPointF(1.045, 1.05);
    
    XYScatterplot my_scatterplot(points);
    scene.addScatterplot(&my_scatterplot);

    scene.autoZoom();
    scene.relativeZoom(1.2);

    graph.setScene(&scene);
    graph.setWindowTitle("Test 1");
    graph.show();
    graph.resize(600, 600);
