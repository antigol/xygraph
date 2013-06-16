#include "xyspline.hpp"

XYSPline::XYSPline(const QMap<qreal, qreal> &pointMap, Interpolation type, const QPen &dotPen, const QBrush &dotBrush, qreal dotRadius, const QPen &linePen) :
    #ifndef NOGSLLIB
    m_accel(0), m_spline(0),
    #endif
    m_points(pointMap), m_type(type), m_dotPen(dotPen), m_dotBrush(dotBrush), m_dotRadius(dotRadius), m_linePen(linePen), m_visible(true)
{
#ifndef NOGSLLIB
    if (type == Spline)
        m_accel = gsl_interp_accel_alloc();
#endif
}

XYSPline::XYSPline(Interpolation type, const QPen &dotPen, const QBrush &dotBrush, qreal dotRadius, const QPen &linePen) :
    #ifndef NOGSLLIB
    m_accel(0), m_spline(0),
    #endif
    m_type(type), m_dotPen(dotPen), m_dotBrush(dotBrush), m_dotRadius(dotRadius), m_linePen(linePen), m_visible(true)
{
#ifndef NOGSLLIB
    if (type == Spline)
        m_accel = gsl_interp_accel_alloc();
#endif
}

XYSPline::~XYSPline()
{
#ifndef NOGSLLIB
    if (m_spline != 0)
        gsl_spline_free(m_spline);

    if (m_accel != 0)
        gsl_interp_accel_free(m_accel);
#endif
}

void XYSPline::setVisible(bool on)
{
    m_visible = on;
}

bool XYSPline::isVisible() const
{
    return m_visible;
}

void XYSPline::setDotPen(const QPen &pen)
{
    m_dotPen = pen;
}

void XYSPline::setDotBrush(const QBrush &brush)
{
    m_dotBrush = brush;
}

void XYSPline::setDotRadius(qreal radius)
{
    m_dotRadius = radius;
}

void XYSPline::setLinePen(const QPen &pen)
{
    m_linePen = pen;
}

void XYSPline::addPoint(qreal x, qreal y)
{
    m_points[x] = y;
    respline();
}

void XYSPline::addPoint(const QPointF &p)
{
    m_points[p.x()] = p.y();
    respline();
}

void XYSPline::loadFromPointMap(const QMap<qreal, qreal> &pointMap)
{
    m_points = pointMap;
    respline();
}

void XYSPline::addFromPointList(const QList<QPointF> &pointList)
{
    for (int i = pointList.size() - 1; i >= 0; --i) {
        m_points.insert(pointList[i].x(), pointList[i].y());
    }
    respline();
}

void XYSPline::removePoint(qreal x)
{
    m_points.remove(x);
    respline();
}

const QMap<qreal, qreal> &XYSPline::pointMap() const
{
    return m_points;
}

void XYSPline::respline()
{
#ifndef NOGSLLIB
    if (m_spline != 0) {
        gsl_spline_free(m_spline);
        m_spline = 0;
    }
#endif
}

qreal XYSPline::interpolate(qreal x)
{
    if (m_points.size() <= 1)
        return 0.0;

    switch (m_type) {
    case Spline:
#ifndef NOGSLLIB
        if (m_points.size() == 2) {
            return interpolate2(x);
        } else {
            return interpolateS(x);
        }
#else
        qDebug("Spline not supported: recompile xygraph without the NOGSLLIB define.");
#endif
    case Linear:
        return interpolate2(x);
        //    case Polynomial:
        //        if (m_points.size() <= 3)
        //            return interpolate2(x);
        //        else
        //            return interpolate4(x);
    }
    return 0.0;
}

qreal XYSPline::xMinimum()
{
    if (m_points.isEmpty())
        return 0.0;

    return m_points.constBegin().key();
}

qreal XYSPline::xMaximum()
{
    if (m_points.isEmpty())
        return 0.0;

    return (--m_points.constEnd()).key();
}

qreal XYSPline::interpolate2(qreal x) const
{
    // retourne l'élément plus grand que x
    QMap<qreal, qreal>::const_iterator i = m_points.upperBound(x);

    // s'il n'y a pas d'éléments plus grand que x
    if (i == m_points.constEnd())
        return (--i).value();

    // si l'élément retourné est le premier élément de la liste
    if (i == m_points.constBegin())
        return i.value();

    qreal x1 = i.key();
    qreal y1 = i.value();

    i--;
    qreal x0 = i.key();
    qreal y0 = i.value();

    // la pente
    qreal m = (y1 - y0) / (x1 - x0);

    return m * (x - x0) + y0;
}

qreal XYSPline::interpolate4(qreal x) const
{
    // retourne l'élément plus grand que x
    QMap<qreal, qreal>::const_iterator i = m_points.upperBound(x);

    // s'il n'y a pas d'éléments plus grand que x
    if (i == m_points.constEnd())
        return (--i).value();

    // si l'élément retourné est le premier élément de la liste
    if (i == m_points.constBegin())
        return i.value();


    // comme i pointe sur l'élément plus grand que x, i pointe sur le point no3

    // si i pointe sur le dernier élément, on le désincrémente
    if (i + 1 == m_points.constEnd())
        i--;

    // si i pointe sur le deuxième élément on l'incrémente
    if (i - 1 == m_points.constBegin())
        i++;

    qreal x3 = i.key();
    qreal y3 = i.value();

    i--;
    qreal x2 = i.key();
    qreal y2 = i.value();

    i--;
    qreal x1 = i.key();
    qreal y1 = i.value();

    i += 3;
    qreal x4 = i.key();
    qreal y4 = i.value();

    // merci Paul Bourke
    return
            y1 * (x-x2)*(x-x3)*(x-x4) / ((x1-x2)*(x1-x3)*(x1-x4)) +
            y2 * (x-x1)*(x-x3)*(x-x4) / ((x2-x1)*(x2-x3)*(x2-x4)) +
            y3 * (x-x1)*(x-x2)*(x-x4) / ((x3-x1)*(x3-x2)*(x3-x4)) +
            y4 * (x-x1)*(x-x2)*(x-x3) / ((x4-x1)*(x4-x2)*(x4-x3));
}

#ifndef NOGSLLIB
qreal XYSPline::interpolateS(qreal x)
{
    if (m_spline == 0) {
        m_spline = gsl_spline_alloc(gsl_interp_cspline, m_points.size());

        m_xs = m_points.keys().toVector();
        m_ys = m_points.values().toVector();

        gsl_spline_init(m_spline, m_xs.constData(), m_ys.constData(), m_points.size());
    }

    return gsl_spline_eval(m_spline, x, m_accel);
}
#endif
