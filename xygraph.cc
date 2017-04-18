#include "xygraph.hh"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <cmath>

namespace XY {

//================= FUNCTION =================//

Function::Function(const QPen& pen) : pen(pen) {}

Function::~Function() {}
bool Function::domain(qreal x) const { Q_UNUSED(x); return true; }

PointList::PointList() {}

//================= POINTLIST =================//

PointList::PointList(const QList<QPointF> &points)
    : QList<QPointF>(points),
      linePen(Qt::white),
      dotRadius(2.0),
      dotPen(Qt::lightGray),
      dotBrush(Qt::lightGray)
{
}

//================= GRAPH =================//

Graph::Graph(QWidget *parent)
    : QOpenGLWidget(parent),
      backgroundBrush(Qt::black),
      axesPen(Qt::lightGray),
      subaxesPen(QBrush(Qt::darkGray), 1.0, Qt::DotLine),
      textPen(Qt::white),
      zoomPen(Qt::red),
      m_zoomMin(0, 0),
      m_zoomMax(1, 1),
      m_zoomRectOrigin(-1, 0),
      m_zoomRectDest(-1, 0)
{
    setToolTip("left click to drag\nright click to zoom\ndouble right click to auto zoom");
}

Graph::~Graph()
{

}

void Graph::setZoom(qreal xmin, qreal xmax, qreal ymin, qreal ymax)
{
    if (xmin < xmax && ymin < ymax) {
        m_zoomMin = QPointF(xmin, ymin);
        m_zoomMax = QPointF(xmax, ymax);
    }
}

void Graph::autoZoom()
{
    QPointF firstPoint;
    for (int i = 0; i < pointLists.size(); ++i) {
        if (!pointLists[i]->isEmpty()) {
            firstPoint = pointLists[i]->first();
            break;
        }
    }

    qreal xmin = firstPoint.x();
    qreal xmax = xmin;
    qreal ymin = firstPoint.y();
    qreal ymax = ymin;

    for (int i = 0; i < pointLists.size(); ++i) {
        for (int j = 1; j < pointLists[i]->size(); ++j) {
            if (pointLists[i]->at(j).x() < xmin)
                xmin = pointLists[i]->at(j).x();
            if (pointLists[i]->at(j).x() > xmax)
                xmax = pointLists[i]->at(j).x();
            if (pointLists[i]->at(j).y() < ymin)
                ymin = pointLists[i]->at(j).y();
            if (pointLists[i]->at(j).y() > ymax)
                ymax = pointLists[i]->at(j).y();
        }
    }

    if (xmin != xmax && ymin != ymax)
        setZoom(xmin, xmax, ymin, ymax);
}

void Graph::relativeZoom(qreal factor)
{
    QPointF center = 0.5 * (m_zoomMax + m_zoomMin);

    QPointF zMin = center + factor * (m_zoomMax - center);
    QPointF zMax = center + factor * (m_zoomMin - center);
    setZoom(zMin.x(), zMax.x(), zMin.y(), zMax.y());
}

qreal Graph::xr2i(qreal xr) const {
    return (xr - m_zoomMin.x()) * width() / xwidth();
}

qreal Graph::xi2r(qreal xi) const {
    return m_zoomMin.x() + xi * xwidth() / width();
}

qreal Graph::yr2i(qreal yr) const {
    return height() - (yr - m_zoomMin.y()) * height() / yheight();
}

qreal Graph::yi2r(qreal yi) const {
    return m_zoomMax.y() - yi * yheight() / height();
}

qreal Graph::wr2i(qreal wr) const {
    return wr * width() / xwidth();
}

qreal Graph::wi2r(qreal wi) const {
    return wi * xwidth() / width();
}

qreal Graph::hr2i(qreal hr) const {
    return hr * height() / yheight();
}

qreal Graph::hi2r(qreal hi) const {
    return hi * yheight() / height();
}

QPointF Graph::real2image(const QPointF &real) const {
    return QPointF(xr2i(real.x()), yr2i(real.y()));
}

QPointF Graph::image2real(const QPointF &image) const {
    return QPointF(xi2r(image.x()), yi2r(image.y()));
}

#define MIN_PIX_PER_X_DIV 50.0
#define MIN_PIX_PER_Y_DIV 40.0
#define TICKS_LENGTH 5.0
#define DIV_INTERVALS_SIZE 4
const qreal DIV_INTERVALS[DIV_INTERVALS_SIZE] = {2.0, 2.5, 5.0, 10.0};

qreal Graph::nearestAxisDivision(qreal division)
{
    if (division <= 0.0) return 1.0;

    // return the nearest value in DIV_INTERVALS * 10^k
    qreal decade = 1.0;
    while (division >= 10.0) {
        division /= 10.0;
        decade *= 10.0;
    }
    while (division < 1.0) {
        division *= 10.0;
        decade /= 10.0;
    }
    for (int i = 0; i < DIV_INTERVALS_SIZE; ++i) {
        if (division < DIV_INTERVALS[i]) {
            return DIV_INTERVALS[i] * decade;
        }
    }
    return decade;
}

void Graph::paintAxes(QPainter &painter)
{
    QPointF imageZero(real2image(QPointF(0.0, 0.0)));
    if (imageZero.x() < 0)
        imageZero.rx() = 0;

    if (imageZero.x() > width())
        imageZero.rx() = width();

    if (imageZero.y() < 0)
        imageZero.ry() = 0;

    if (imageZero.y() > height())
        imageZero.ry() = height();

    QPainterPath ticks, grid;

    qreal div = nearestAxisDivision(wi2r(MIN_PIX_PER_X_DIV));
    const int64_t xmin = std::ceil(m_zoomMin.x() / div);
    const int64_t xmax = std::ceil(m_zoomMax.x() / div);
    for (int64_t x = xmin; x < xmax; ++x) {
        if (x == 0)
            continue;

        qreal xreal = x * div;
        qreal ximage = xr2i(xreal);

        ticks.moveTo(ximage, imageZero.y() - TICKS_LENGTH);
        ticks.lineTo(ximage, imageZero.y() + TICKS_LENGTH);

        grid.moveTo(ximage, height());
        grid.lineTo(ximage, 0);
    }

    div = nearestAxisDivision(hi2r(MIN_PIX_PER_Y_DIV));
    const int64_t ymin = std::ceil(m_zoomMin.y() / div);
    const int64_t ymax = std::ceil(m_zoomMax.y() / div);
    for (int64_t y = ymin; y < ymax; ++y) {
        if (y == 0)
            continue;

        qreal yreal = y * div;
        qreal yimage = yr2i(yreal);

        ticks.moveTo(imageZero.x() - TICKS_LENGTH, yimage);
        ticks.lineTo(imageZero.x() + TICKS_LENGTH, yimage);

        grid.moveTo(0, yimage);
        grid.lineTo(width(), yimage);
    }

    painter.setBrush(Qt::NoBrush);

    // les deux lignes du 0
    painter.setPen(axesPen);
    painter.drawLine(0, imageZero.y(), width(), imageZero.y());
    painter.drawLine(imageZero.x(), 0, imageZero.x(), height());

    painter.setPen(subaxesPen);
    painter.drawPath(grid);

    painter.setPen(axesPen);
    painter.drawPath(ticks);
}

void Graph::paintText(QPainter &painter)
{
    QPointF imageZero(real2image(QPointF(0.0, 0.0)));
    if (imageZero.x() < 0)
        imageZero.rx() = 0;

    if (imageZero.x() > width())
        imageZero.rx() = width();

    if (imageZero.y() < 0)
        imageZero.ry() = 0;

    if (imageZero.y() > height())
        imageZero.ry() = height();

    QFontMetrics fm(painter.font());

    painter.setPen(textPen);

    qreal div = nearestAxisDivision(wi2r(MIN_PIX_PER_X_DIV));

    qreal yTxtPos = imageZero.y();
    if (imageZero.y() < height() / 2.0)
        yTxtPos += 25.0;
    else
        yTxtPos -= 10.0;

    const int64_t xmin = std::ceil(m_zoomMin.x() / div);
    const int64_t xmax = std::ceil(m_zoomMax.x() / div);
    for (int64_t x = xmin; x < xmax; ++x) {
        qreal xreal = x * div;
        qreal ximage = xr2i(xreal);

        if (qAbs(ximage - imageZero.x()) > 30.0) {
            QString text = QString::number(xreal);
            QPoint position(ximage - fm.boundingRect(text).width() * 0.5 - 2.0,
                            yTxtPos);

            painter.drawText(position, text);
        }
    }

    div = nearestAxisDivision(hi2r(MIN_PIX_PER_Y_DIV));
    bool textOnRight = imageZero.x() < width() / 2.0;

    const int64_t ymin = std::ceil(m_zoomMin.y() / div);
    const int64_t ymax = std::ceil(m_zoomMax.y() / div);
    for (int64_t y = ymin; y < ymax; ++y) {
        qreal yreal = y * div;
        qreal yimage = yr2i(yreal);

        if (qAbs(yimage - imageZero.y()) > 30.0) {
            QString text = QString::number(yreal);
            QPoint position(imageZero.x(), yimage + 0.4 * fm.ascent());

            if (textOnRight)
                position.rx() += 10.0;
            else
                position.rx() -= fm.width(text) + 12.0;

            painter.drawText(position, text);
        }
    }
}

void Graph::paintPointLists(QPainter& painter)
{
    for (int i = 0; i < pointLists.size(); ++i) {
        QPainterPath path;

        if (!pointLists[i]->isEmpty()) {
            const QPointF point = real2image(pointLists[i]->first());
            path.moveTo(point);
        }

        for (int j = 1; j < pointLists[i]->size(); ++j) {
            const QPointF point = real2image(pointLists[i]->at(j));
            path.lineTo(point);
        }
        painter.setPen(pointLists[i]->linePen);
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(path);
    }
    for (int i = 0; i < pointLists.size(); ++i) {
        qreal r = pointLists[i]->dotRadius;
        if (r <= 0.0) continue;

        painter.setPen(pointLists[i]->dotPen);
        painter.setBrush(pointLists[i]->dotBrush);
        for (int j = 0; j < pointLists[i]->size(); ++j) {
            const QPointF point = real2image(pointLists[i]->at(j));
            painter.drawEllipse(point, r, r);
        }
    }
}

void Graph::paintFunctions(QPainter& painter)
{
    for (int i = 0; i < functions.size(); ++i) {
        QPainterPath path;
        bool startPath = false;
        for (qreal ximage = 0; ximage < width(); ximage += 1.5) {
            qreal xreal = xi2r(ximage);
            if (functions[i]->domain(xreal)) {
                qreal yreal = functions[i]->y(xreal);
                if (!std::isnan(yreal) && !std::isinf(yreal)) {
                    if (startPath) {
                        path.lineTo(ximage, yr2i(yreal));
                    } else {
                        path.moveTo(ximage, yr2i(yreal));
                        startPath = true;
                    }
                } else {
                    startPath = false;
                }
            } else {
                startPath = false;
            }
        }
        painter.setPen(functions[i]->pen);
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(path);
    }
}

void Graph::initializeGL()
{

}

void Graph::paintGL()
{
    QPainter painter(this);

    painter.fillRect(rect(), backgroundBrush);

    paintAxes(painter);
    paintPointLists(painter);
    paintFunctions(painter);
    paintText(painter);

    if (m_zoomRectOrigin.x() > 0 && m_zoomRectDest.x() > 0) {
        painter.setPen(zoomPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(QRect(m_zoomRectOrigin, m_zoomRectDest));
    }
}

void Graph::resizeGL(int w, int h)
{
    Q_UNUSED(w);
    Q_UNUSED(h);
}

void Graph::paintEvent(QPaintEvent *event)
{
    QOpenGLWidget::paintEvent(event);
}

void Graph::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        m_lastMousePos = event->pos();
    }

    if (event->buttons() & Qt::RightButton) {
        m_zoomRectOrigin = event->pos();
        m_zoomRectDest.setX(-1);
    }
}

void Graph::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        QPointF delta = event->pos() - m_lastMousePos;
        delta.rx() = -wi2r(delta.x());
        delta.ry() = hi2r(delta.y());

        m_zoomMin += delta;
        m_zoomMax += delta;

        update();
    } else if (event->buttons() & Qt::RightButton) {
        if (m_zoomRectOrigin.x() > 0) {
            m_zoomRectDest = event->pos();
            update();
        }
    }

    m_lastMousePos = event->pos();
}

void Graph::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    if (m_zoomRectOrigin.x() > 0 && m_zoomRectDest.x() > 0) {
        QPointF zMin = image2real(QPointF(qMin(m_zoomRectOrigin.x(), m_zoomRectDest.x()),
                                          qMax(m_zoomRectOrigin.y(), m_zoomRectDest.y())));
        QPointF zMax = image2real(QPointF(qMax(m_zoomRectOrigin.x(), m_zoomRectDest.x()),
                                       qMin(m_zoomRectOrigin.y(), m_zoomRectDest.y())));
        setZoom(zMin.x(), zMax.x(), zMin.y(), zMax.y());

        update();
    }

    m_zoomRectOrigin.setX(-1);
    m_zoomRectDest.setX(-1);
}

void Graph::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    autoZoom();
    relativeZoom(1.01);

    update();
}

void Graph::wheelEvent(QWheelEvent *event)
{
    const qreal zoom = std::pow(0.95, event->delta() / 120.0);
    QPointF mousePos = image2real(event->pos());

    QPointF zMin = mousePos + zoom * (m_zoomMin - mousePos);
    QPointF zMax = mousePos + zoom * (m_zoomMax - mousePos);
    setZoom(zMin.x(), zMax.x(), zMin.y(), zMax.y());

    update();
}


}
