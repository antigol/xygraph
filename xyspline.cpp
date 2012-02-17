#include "xyspline.hpp"

XYSPline::XYSPline(const QPen &dotPen, const QBrush &dotBrush, qreal dotRadius, const QPen &linePen) :
    m_spline(0), m_dotPen(dotPen), m_dotBrush(dotBrush), m_dotRadius(dotRadius), m_linePen(linePen), m_visible(true)
{
    m_accel = gsl_interp_accel_alloc();
}

XYSPline::~XYSPline()
{
    if (m_spline != 0)
        gsl_spline_free(m_spline);

    gsl_interp_accel_free(m_accel);
}

void XYSPline::setVisible(bool on)
{
    m_visible = on;
}

bool XYSPline::isVisible() const
{
    return m_visible;
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

void XYSPline::removePoint(qreal x)
{
    m_points.remove(x);
    respline();
}

void XYSPline::respline()
{
    if (m_spline != 0) {
        gsl_spline_free(m_spline);
        m_spline = 0;
    }
}

qreal XYSPline::spline(qreal x)
{
    if (m_points.size() < 2) {
        return 0.0;
    } else if (m_points.size() == 2) {
        qreal x1 = m_points.constBegin().key();
        qreal y1 = m_points.constBegin().value();
        qreal x2 = (++m_points.constBegin()).key();
        qreal y2 = (++m_points.constBegin()).value();

        return (x - x1) / (x2 - x1) * (y2 - y1) + y1;
    } else {

        if (m_spline == 0) {
            m_spline = gsl_spline_alloc(gsl_interp_cspline, m_points.size());

            m_xs = m_points.keys().toVector();
            m_ys = m_points.values().toVector();

            gsl_spline_init(m_spline, m_xs.constData(), m_ys.constData(), m_points.size());
        }

        return gsl_spline_eval(m_spline, x, m_accel);
    }
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

