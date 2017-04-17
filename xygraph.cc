#include "xygraph.hh"
#include <QPaintEvent>
#include <QPainter>
#include <cmath>
#include <QDebug>

namespace XY {

Function::Function(const QPen& pen) : pen(pen) {}

Function::~Function() {}
bool Function::domain(qreal x) const { Q_UNUSED(x); return true; }

PointList::PointList() {}

PointList::PointList(const QList<QPointF> &points)
    : QList<QPointF>(points),
      linePen(Qt::white),
      dotRadius(2.0),
      dotPen(Qt::lightGray),
      dotBrush(Qt::lightGray)
{
}


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
    connect(&m_timerRegraph, SIGNAL(timeout()), this, SLOT(update()));
}

Graph::~Graph()
{

}

void Graph::addPointList(PointList *pointList)
{
    m_pointLists << pointList;
}

void Graph::addFunction(Function *function)
{
    m_functions << function;
}

void Graph::autoZoom()
{
    QPointF firstPoint;
    for (int i = 0; i < m_pointLists.size(); ++i) {
        if (!m_pointLists[i]->isEmpty()) {
            firstPoint = m_pointLists[i]->first();
            break;
        }
    }

    qreal xmin = firstPoint.x();
    qreal xmax = xmin;
    qreal ymin = firstPoint.y();
    qreal ymax = ymin;

    for (int i = 0; i < m_pointLists.size(); ++i) {
        for (int j = 1; j < m_pointLists[i]->size(); ++j) {
            if (m_pointLists[i]->at(j).x() < xmin)
                xmin = m_pointLists[i]->at(j).x();
            if (m_pointLists[i]->at(j).x() > xmax)
                xmax = m_pointLists[i]->at(j).x();
            if (m_pointLists[i]->at(j).y() < ymin)
                ymin = m_pointLists[i]->at(j).y();
            if (m_pointLists[i]->at(j).y() > ymax)
                ymax = m_pointLists[i]->at(j).y();
        }
    }

    if (xmin != xmax && ymin != ymax)
        setZoom(xmin, xmax, ymin, ymax);
}

void Graph::setZoom(qreal xmin, qreal xmax, qreal ymin, qreal ymax)
{
    m_zoomMin.setX(xmin);
    m_zoomMin.setY(ymin);

    m_zoomMax.setX(xmax);
    m_zoomMax.setY(ymax);
}

qreal Graph::xr2i(qreal xr) const {
    return (xr - m_zoomMin.x()) * width() / (m_zoomMax.x() - m_zoomMin.x());
}

qreal Graph::xi2r(qreal xi) const {
    return m_zoomMin.x() + xi * (m_zoomMax.x() - m_zoomMin.x()) / width();
}

qreal Graph::yr2i(qreal yr) const {
    return height() - (yr - m_zoomMin.y()) * height() / (m_zoomMax.y() - m_zoomMin.y());
}

qreal Graph::yi2r(qreal yi) const {
    return m_zoomMax.y() - yi * (m_zoomMax.y() - m_zoomMin.y()) / height();
}

qreal Graph::wr2i(qreal wr) const {
    return wr * width() / (m_zoomMax.x() - m_zoomMin.x());
}

qreal Graph::wi2r(qreal wi) const {
    return wi * (m_zoomMax.x() - m_zoomMin.x()) / width();
}

qreal Graph::hr2i(qreal hr) const {
    return hr * height() / (m_zoomMax.y() - m_zoomMin.y());
}

qreal Graph::hi2r(qreal hi) const {
    return hi * (m_zoomMax.y() - m_zoomMin.y()) / height();
}

QPointF Graph::real2image(const QPointF &real) const {
    return QPointF(xr2i(real.x()), yr2i(real.y()));
}

QPointF Graph::image2real(const QPointF &image) const {
    return QPointF(xi2r(image.x()), yi2r(image.y()));
}

#define PIXELMIN 40.0
#define LENGTH 5.0
#define SUITESIZE 4

void Graph::paintAxes(QPainter &painter)
{
    qreal suite[SUITESIZE] = {2.0, 2.5, 5.0, 10.0};

    QPointF imageZero(real2image(QPointF(0.0, 0.0)));
    if (imageZero.x() < 0)
        imageZero.rx() = 0;

    if (imageZero.x() > width())
        imageZero.rx() = width();

    if (imageZero.y() < 0)
        imageZero.ry() = 0;

    if (imageZero.y() > height())
        imageZero.ry() = height();

    // les deux lignes du 0
    painter.setPen(axesPen);
    painter.drawLine(0, imageZero.y(), width(), imageZero.y());
    painter.drawLine(imageZero.x(), 0, imageZero.x(), height());


    QPainterPath path, subpath;
    QFontMetrics fm(painter.font());

    // graduation de l'axe X
    {
        // Recherche de la division
        qreal div = wi2r(PIXELMIN);
        qreal decade = 1.0;
        while (div >= 10.0) {
            div /= 10.0;
            decade *= 10.0;
        }
        while (div < 1.0) {
            div *= 10.0;
            decade /= 10.0;
        }
        for (int i = 0; i < SUITESIZE; ++i) {
            if (div < suite[i]) {
                div = suite[i] * decade;
                break;
            }
        }

        // graduation X avec la division
        qreal yTxtPos = imageZero.y();
        if (imageZero.y() < height() / 2.0)
            yTxtPos += 15.0;
        else
            yTxtPos -= 10.0;

        const int xmax = ceil(m_zoomMax.x() / div);
        for (int x = ceil(m_zoomMin.x() / div); x < xmax; ++x) {
            if (x == 0)
                continue;
            qreal xreal = x * div;
            qreal ximage = xr2i(xreal);

            path.moveTo(ximage, imageZero.y() - LENGTH);
            path.lineTo(ximage, imageZero.y() + LENGTH);

            subpath.moveTo(ximage, height());
            subpath.lineTo(ximage, 0);
        }
    }

    // graduation de l'axe Y
    {
        // Recherche de la division
        qreal div = hi2r(PIXELMIN);
        qreal decade = 1.0;
        while (div >= 10.0) {
            div /= 10.0;
            decade *= 10.0;
        }
        while (div < 1.0) {
            div *= 10.0;
            decade /= 10.0;
        }
        for (int i = 0; i < SUITESIZE; ++i) {
            if (div < suite[i]) {
                div = suite[i] * decade;
                break;
            }
        }

        // graduation Y avec la division
        const int ymax = ceil(m_zoomMax.y() / div);
        for (int y = ceil(m_zoomMin.y() / div); y < ymax; ++y) {
            if (y == 0)
                continue;
            qreal yreal = y * div;
            qreal yimage = yr2i(yreal);

            path.moveTo(imageZero.x() - LENGTH, yimage);
            path.lineTo(imageZero.x() + LENGTH, yimage);

            subpath.moveTo(0, yimage);
            subpath.lineTo(width(), yimage);
        }
    }

    painter.setPen(subaxesPen);
    painter.drawPath(subpath);

    painter.setPen(axesPen);
    painter.drawPath(path);
}

void Graph::paintText(QPainter &painter)
{
    qreal suite[SUITESIZE] = {2.0, 2.5, 5.0, 10.0};

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

    // graduation de l'axe X
    {
        // Recherche de la division
        qreal div = wi2r(PIXELMIN);
        qreal decade = 1.0;
        while (div >= 10.0) {
            div /= 10.0;
            decade *= 10.0;
        }
        while (div < 1.0) {
            div *= 10.0;
            decade /= 10.0;
        }
        for (int i = 0; i < SUITESIZE; ++i) {
            if (div < suite[i]) {
                div = suite[i] * decade;
                break;
            }
        }

        // graduation X avec la division
        qreal yTxtPos = imageZero.y();
        if (imageZero.y() < height() / 2.0)
            yTxtPos += 15.0;
        else
            yTxtPos -= 10.0;

        const int xmax = ceil(m_zoomMax.x() / div);
        for (int x = ceil(m_zoomMin.x() / div); x < xmax; ++x) {
            if (x == 0)
                continue;
            qreal xreal = x * div;
            qreal ximage = xr2i(xreal);

            if (qAbs(ximage - imageZero.x()) > 30.0) {
                QString text = QString::number(xreal);
                QPoint position(ximage - fm.boundingRect(text).width() * 0.5 - 2.0,
                                yTxtPos);

                painter.drawText(position, text);
            }
        }
    }

    // graduation de l'axe Y
    {
        // Recherche de la division
        qreal div = hi2r(PIXELMIN);
        qreal decade = 1.0;
        while (div >= 10.0) {
            div /= 10.0;
            decade *= 10.0;
        }
        while (div < 1.0) {
            div *= 10.0;
            decade /= 10.0;
        }
        for (int i = 0; i < SUITESIZE; ++i) {
            if (div < suite[i]) {
                div = suite[i] * decade;
                break;
            }
        }

        // graduation Y avec la division
        bool textOnRight = imageZero.x() < width() / 2.0;

        const int ymax = ceil(m_zoomMax.y() / div);
        for (int y = ceil(m_zoomMin.y() / div); y < ymax; ++y) {
            if (y == 0)
                continue;
            qreal yreal = y * div;
            qreal yimage = yr2i(yreal);

            if (qAbs(yimage - imageZero.y()) > 30.0) {
                QString text = QString::number(yreal);
                QPoint position(imageZero.x(), yimage - 1);

                if (textOnRight)
                    position.rx() += 7.0;
                else
                    position.rx() -= fm.boundingRect(text).width() + 12.0;

                painter.drawText(position, text);
            }
        }
    }
}

void Graph::paintPointLists(QPainter& painter)
{
    for (int i = 0; i < m_pointLists.size(); ++i) {
        QPainterPath path;

        if (!m_pointLists[i]->isEmpty()) {
            const QPointF point = real2image(m_pointLists[i]->first());
            path.moveTo(point);
        }

        for (int j = 1; j < m_pointLists[i]->size(); ++j) {
            const QPointF point = real2image(m_pointLists[i]->at(j));
            path.lineTo(point);
        }
        painter.setPen(m_pointLists[i]->linePen);
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(path);
    }
    for (int i = 0; i < m_pointLists.size(); ++i) {
        qreal r = m_pointLists[i]->dotRadius;
        if (r <= 0.0) continue;

        painter.setPen(m_pointLists[i]->dotPen);
        painter.setBrush(m_pointLists[i]->dotBrush);
        for (int j = 0; j < m_pointLists[i]->size(); ++j) {
            const QPointF point = real2image(m_pointLists[i]->at(j));
            painter.drawEllipse(point, r, r);
        }
    }
}

void Graph::paintFunctions(QPainter& painter)
{
    for (int i = 0; i < m_functions.size(); ++i) {
        QPainterPath path;
        bool startPath = false;
        for (qreal ximage = 0; ximage < width(); ximage += 1.5) {
            qreal xreal = xi2r(ximage);
            if (m_functions[i]->domain(xreal)) {
                qreal yreal = m_functions[i]->y(xreal);
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
        painter.setPen(m_functions[i]->pen);
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
        m_timerRegraph.start();
    }

    if (event->buttons() & Qt::RightButton) {
        m_zoomRectOrigin = event->pos();
        m_zoomRectDest.setX(-1);
        m_timerRegraph.start();
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
    } else if (event->buttons() & Qt::RightButton) {
        if (m_zoomRectOrigin.x() > 0) {
            m_zoomRectDest = event->pos();
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
        m_zoomMax = image2real(QPointF(qMax(m_zoomRectOrigin.x(), m_zoomRectDest.x()),
                                       qMin(m_zoomRectOrigin.y(), m_zoomRectDest.y())));
        m_zoomMin = zMin;


        update();
    }

    m_zoomRectOrigin.setX(-1);
    m_zoomRectDest.setX(-1);
    m_timerRegraph.stop();
}

void Graph::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    autoZoom();

    update();
}

void Graph::wheelEvent(QWheelEvent *event)
{
    const qreal zoom = std::pow(0.95, event->delta() / 120.0);
    QPointF mousePos = image2real(event->pos());

    m_zoomMax = mousePos + zoom * (m_zoomMax - mousePos);
    m_zoomMin= mousePos + zoom * (m_zoomMin - mousePos);

    update();
}


}
