#ifndef XYSPLINE_H
#define XYSPLINE_H

#include "xygraph_global.hpp"
#include <QVector>
#include <QMap>
#include <QPointF>
#include <QPen>
#include <QBrush>

#ifndef NOGSLLIB
#   include <gsl/gsl_spline.h>
#endif

class XYGRAPHSHARED_EXPORT XYSPline
{
public:
    enum Interpolation {
        Spline,
        Linear
    };

    XYSPline(const QMap<qreal, qreal> &pointMap, enum Interpolation type = Spline,
             const QPen &dotPen = QPen(), const QBrush &dotBrush = QBrush(), qreal dotRadius = 2.0, const QPen &linePen = QPen());
    XYSPline(enum Interpolation type = Spline, const QPen &dotPen = QPen(),
             const QBrush &dotBrush = QBrush(), qreal dotRadius = 2.0, const QPen &linePen = QPen());
    ~XYSPline();

    void setVisible(bool on);
    bool isVisible() const;

    void setDotPen(const QPen &pen);
    void setDotBrush(const QBrush &brush);
    void setDotRadius(qreal radius);
    void setLinePen(const QPen &pen);

    void addPoint(qreal x, qreal y);
    void addPoint(const QPointF &p);
    void loadFromPointMap(const QMap<qreal, qreal> &pointMap);
    void addFromPointList(const QList<QPointF> &pointList);

    void removePoint(qreal x);

    /* toPointMap : accesseur read-only */
    const QMap<qreal, qreal> &pointMap() const;

    void respline();
    qreal interpolate(qreal x);

    qreal xMinimum();
    qreal xMaximum();

private:
    qreal interpolate2(qreal x) const;
    qreal interpolate4(qreal x) const;
#ifndef NOGSLLIB
    qreal interpolateS(qreal x);
    gsl_interp_accel *m_accel;
    gsl_spline *m_spline;
#endif


    QMap<qreal, qreal> m_points;
    QVector<qreal> m_xs;
    QVector<qreal> m_ys;

    enum Interpolation m_type;


    QPen m_dotPen;
    QBrush m_dotBrush;
    qreal m_dotRadius;
    QPen m_linePen;

    bool m_visible;

    friend class XYScene;
};

#endif // XYSPLINE_H
