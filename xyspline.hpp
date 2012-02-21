#ifndef XYSPLINE_H
#define XYSPLINE_H

#include "xygraph_global.hpp"
#include <QVector>
#include <QMap>
#include <QPointF>
#include <QPen>
#include <QBrush>

#include <gsl/gsl_spline.h>

class XYGRAPHSHARED_EXPORT XYSPline
{
public:
    XYSPline(const QPen &dotPen = QPen(), const QBrush &dotBrush = QBrush(), qreal dotRadius = 2.0, const QPen &linePen = QPen());
    ~XYSPline();

    void setVisible(bool on);
    bool isVisible() const;

    void addPoint(qreal x, qreal y);
    void addPoint(const QPointF &p);
    void loadFromPointMap(const QMap<qreal, qreal> &pointMap);

    void removePoint(qreal x);

    void respline();

    qreal spline(qreal x);

    qreal xMinimum();
    qreal xMaximum();

private:
    QMap<qreal, qreal> m_points;
    QVector<qreal> m_xs;
    QVector<qreal> m_ys;

    gsl_interp_accel *m_accel;
    gsl_spline *m_spline;

    QPen m_dotPen;
    QBrush m_dotBrush;
    qreal m_dotRadius;
    QPen m_linePen;

    bool m_visible;

    friend class XYScene;
};

#endif // XYSPLINE_H
