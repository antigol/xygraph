#ifndef XYGRAPH_HH
#define XYGRAPH_HH

#include <QOpenGLWidget>
#include <QPen>
#include <QBrush>

namespace XY {

//================= FUNCTION =================//

class Function
{
public:
    Function(const QPen& pen = QPen(Qt::white));
    virtual ~Function();

    virtual qreal y(qreal x) = 0;
    virtual bool domain(qreal x) const;

    QPen pen;
};

//================= POINTLIST =================//

class PointList : public QList<QPointF>
{
public:
    PointList();
    PointList(const QList<QPointF>& points);

    QPen linePen;
    qreal dotRadius;
    QPen dotPen;
    QBrush dotBrush;
};

//================= GRAPH =================//

class Graph : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit Graph(QWidget *parent = 0);
    virtual ~Graph();

    inline qreal xmin() const { return m_zoomMin.x(); }
    inline qreal xmax() const { return m_zoomMax.x(); }
    inline qreal ymin() const { return m_zoomMin.y(); }
    inline qreal ymax() const { return m_zoomMax.y(); }
    inline qreal xwidth()  const { return m_zoomMax.x() - m_zoomMin.x(); }
    inline qreal yheight() const { return m_zoomMax.y() - m_zoomMin.y(); }
    inline void setxmin(qreal o) { m_zoomMin.rx() = o; }
    inline void setxmax(qreal o) { m_zoomMax.rx() = o; }
    inline void setymin(qreal o) { m_zoomMin.ry() = o; }
    inline void setymax(qreal o) { m_zoomMax.ry() = o; }
    void setZoom(qreal xmin, qreal xmax, qreal ymin, qreal ymax);

    void autoZoom();
    void relativeZoom(qreal factor);

    qreal xr2i(qreal xr) const;
    qreal xi2r(qreal xi) const;
    qreal yr2i(qreal yr) const;
    qreal yi2r(qreal yi) const;
    qreal wr2i(qreal wr) const;
    qreal wi2r(qreal wi) const;
    qreal hr2i(qreal hr) const;
    qreal hi2r(qreal hi) const;
    QPointF real2image(const QPointF &real) const;
    QPointF image2real(const QPointF &image) const;

    QBrush backgroundBrush;
    QPen axesPen;
    QPen subaxesPen;
    QPen textPen;
    QPen zoomPen;

    QList<PointList*> pointLists;
    QList<Function*> functions;

private:
    qreal nearestAxisDivision(qreal pixels);
    void paintAxes(QPainter& painter);
    void paintText(QPainter& painter);
    void paintPointLists(QPainter& painter);
    void paintFunctions(QPainter& painter);

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    QPointF m_zoomMin;
    QPointF m_zoomMax;

    QPointF m_lastMousePos;
    QPoint m_zoomRectOrigin;
    QPoint m_zoomRectDest;
};

}

#endif // XYGRAPH_HH
