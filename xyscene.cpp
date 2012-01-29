/*

    Copyright (c) 2010 by Mario Geiger <mario.geiger@epfl.ch>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "xyscene.hpp"

#include <QGraphicsTextItem>
#include <QKeyEvent>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsSceneMouseEvent>
#include <QFontMetrics>
#include <QTime>

#include <cmath>

bool XYFunction::domain(qreal x) const
{
    Q_UNUSED(x);
    return true;
}

XYScene::XYScene(QObject *parent) :
    QGraphicsScene(parent),
    m_zoomRect(0),
    m_axesPen(Qt::white),
    m_subaxesPen(Qt::gray),
    m_textColor(Qt::white),
    m_zoomPen(Qt::yellow),
    m_state(XYScene::RegraphOnResize)
{
    setBackgroundBrush(Qt::darkGray);

    m_timerRegraph = new QTimer(this);
    connect(m_timerRegraph, SIGNAL(timeout()), this, SLOT(regraph()));

    m_timerZoom = new QTimer(this);
    m_timerZoom->setSingleShot(true);
    m_timerZoom->setInterval(100);
    connect(m_timerZoom, SIGNAL(timeout()), this, SIGNAL(zoomChanged()));

    connect(this, SIGNAL(sceneRectChanged(QRectF)), this, SLOT(maiRegraph()));
}

XYScene::~XYScene()
{
}

void XYScene::maiRegraph()
{
    if ((m_state & XYScene::RegraphOnResize) == XYScene::RegraphOnResize)
        regraph();
}

void XYScene::regraph()
{
    if (m_zoomRect)
        return;

    if (sceneRect().isNull())
        return;

    if (m_realSceneRect.isInvalid())
        return;

    clear();

    //	QTime ch;
    //	ch.start();
    drawaxes();
    //    	qDebug("drawaxes      %d", ch.restart());
    drawfunctions();
    //	qDebug("drawfunctions %d", ch.restart());
    drawpoints();
    //	qDebug("drawpoints    %d", ch.restart());
}

#define PIXELMIN 40.0
#define LENGTH 5.0
#define SUITESIZE 4
qreal suite[SUITESIZE] = {2.0, 2.5, 5.0, 10.0};

void XYScene::drawaxes()
{
    //	QTime ch;
    //	ch.start();

    QPointF imageZero(real2image(QPointF(0.0, 0.0)));
    if (imageZero.x() < sceneRect().left())
        imageZero.rx() = sceneRect().left();

    if (imageZero.x() > sceneRect().right())
        imageZero.rx() = sceneRect().right();

    if (imageZero.y() < sceneRect().top())
        imageZero.ry() = sceneRect().top();

    if (imageZero.y() > sceneRect().bottom())
        imageZero.ry() = sceneRect().bottom();

    // les deux lignes du 0
    addLine(sceneRect().left(), imageZero.y(), sceneRect().right(), imageZero.y(), m_axesPen);
    addLine(imageZero.x(), sceneRect().top(), imageZero.x(), sceneRect().bottom(), m_axesPen);


    QPainterPath path, subpath;
    qreal div;
    qreal decade;

    //	qDebug("init %d", ch.restart());

    // graduation de l'axe X
    {
        // Recherche de la division
        div = wi2r(PIXELMIN);
        decade = 1.0;
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
        if (imageZero.y() < (sceneRect().top() + sceneRect().bottom()) / 2.0)
            yTxtPos += 5.0;
        else
            yTxtPos -= 27.0;

        const int xmax = ceil(m_realSceneRect.xMax() / div);
        for (int x = ceil(m_realSceneRect.xMin() / div); x < xmax; ++x) {
            if (x == 0)
                continue;
            qreal xreal = x * div;
            qreal ximage = xr2i(xreal);

            path.moveTo(ximage, imageZero.y() - LENGTH);
            path.lineTo(ximage, imageZero.y() + LENGTH);

            subpath.moveTo(ximage, sceneRect().bottom());
            subpath.lineTo(ximage, sceneRect().top());

            if (qAbs(ximage - imageZero.x()) > 30.0) {
                QGraphicsTextItem *txt = addText(QString::number(xreal));
                txt->setZValue(2);
                txt->setDefaultTextColor(m_textColor);
                static QFontMetrics fm(txt->font());

                txt->setPos(ximage - fm.boundingRect(txt->toPlainText()).width() * 0.5 - 2.0,
                            yTxtPos);
            }
        }
    }

    //	qDebug("x %d", ch.restart());

    // graduation de l'axe Y
    {
        // Recherche de la division
        div = hi2r(PIXELMIN);
        decade = 1.0;
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
        bool textOnRight = imageZero.x() < (sceneRect().left() + sceneRect().right()) / 2.0;

        const int ymax = ceil(m_realSceneRect.yMax() / div);
        for (int y = ceil(m_realSceneRect.yMin() / div); y < ymax; ++y) {
            if (y == 0)
                continue;
            qreal yreal = y * div;
            qreal yimage = yr2i(yreal);

            path.moveTo(imageZero.x() - LENGTH, yimage);
            path.lineTo(imageZero.x() + LENGTH, yimage);

            subpath.moveTo(sceneRect().left(), yimage);
            subpath.lineTo(sceneRect().right(), yimage);

            if (qAbs(yimage - imageZero.y()) > 30.0) {
                QGraphicsTextItem *txt = addText(QString::number(yreal));
                txt->setZValue(2);
                txt->setDefaultTextColor(m_textColor);
                static QFontMetrics fm(txt->font());

                QPointF txtpos(imageZero.x(), yimage - fm.height() * 0.72);

                if (textOnRight)
                    txtpos.rx() += 7.0;
                else
                    txtpos.rx() -= fm.boundingRect(txt->toPlainText()).width() + 12.0;

                txt->setPos(txtpos);
            }
        }
    }

    //	qDebug("y %d", ch.restart());

    addPath(path, m_axesPen)->setZValue(1);
    addPath(subpath, m_subaxesPen);

    //	qDebug("end %d", ch.restart());
}

void XYScene::drawfunctions()
{
    bool startPath;
    qreal xreal;
    QPointF point;

    for (int i = 0; i < m_functions.size(); ++i) {
        if (!m_functions[i]->isVisible())
            continue;

        QPainterPath path;
        startPath = false;
        for (qreal ximage = sceneRect().left(); ximage < sceneRect().right(); ximage += 1.5) {
            xreal = xi2r(ximage);
            point.setX(ximage);
            if (m_functions[i]->domain(xreal)) {
                point.setY(yr2i(m_functions[i]->y(xreal)));
                if (startPath) {
                    path.lineTo(point);
                } else {
                    path.moveTo(point);
                    startPath = true;
                }
            } else {
                startPath = false;
            }
        }
        addPath(path, m_functions[i]->m_pen, QBrush(Qt::NoBrush));
    }
}

void XYScene::drawpoints()
{
    for (int i = 0; i < m_scatterplots.size(); ++i) {
        if (!m_scatterplots[i]->isVisible())
            continue;

        if (m_scatterplots[i]->m_linepen != QPen(Qt::NoPen)) {
            QPainterPath path;
            for (int p = 0; p < m_scatterplots[i]->size(); ++p) {
                const QPointF point = real2image(m_scatterplots[i]->at(p));
                if (p == 0)
                    path.moveTo(point);
                else
                    path.lineTo(point);
            }
            addPath(path, m_scatterplots[i]->m_linepen, QBrush(Qt::NoBrush));
        }

        /* draw ellipses */
        const qreal r = m_scatterplots[i]->m_r;
        const qreal d = 2.0 * r;
        if (r != 0.0) {
            for (int p = 0; p < m_scatterplots[i]->size(); ++p) {
                addEllipse(xr2i(m_scatterplots[i]->at(p).x()) - r, yr2i(m_scatterplots[i]->at(p).y()) - r, d, d, m_scatterplots[i]->m_pen, m_scatterplots[i]->m_brush);
            }
        }
    }
}

const RealZoom &XYScene::zoom() const
{
    return m_realSceneRect;
}

void XYScene::setZoom(const RealZoom &zoom)
{
    m_realSceneRect = zoom;
    if (m_state & XYScene::SendZoomChanged)
        m_timerZoom->start();
}

void XYScene::setZoom(qreal xmin, qreal xmax, qreal ymin, qreal ymax)
{
    Q_ASSERT(ymin <= ymax);
    setZoom(RealZoom(xmin, xmax, ymin, ymax));
}

void XYScene::setXMin(qreal xmin)
{
    m_realSceneRect.setXMin(xmin);
    if (m_state & XYScene::SendZoomChanged)
        m_timerZoom->start();
}

void XYScene::setXMax(qreal xmax)
{
    m_realSceneRect.setXMax(xmax);
    if (m_state & XYScene::SendZoomChanged)
        m_timerZoom->start();
}

void XYScene::setYMin(qreal ymin)
{
    m_realSceneRect.setYMin(ymin);
    if (m_state & XYScene::SendZoomChanged)
        m_timerZoom->start();
}

void XYScene::setYMax(qreal ymax)
{
    m_realSceneRect.setYMax(ymax);
    if (m_state & XYScene::SendZoomChanged)
        m_timerZoom->start();
}

void XYScene::relativeZoom(qreal k)
{
    const qreal xadd = m_realSceneRect.width() * (k - 1.0) / 2.0;
    const qreal yadd = m_realSceneRect.height() * (k - 1.0) / 2.0;
    setZoom(m_realSceneRect.xMin() - xadd, m_realSceneRect.xMax() + xadd,
            m_realSceneRect.yMin() - yadd, m_realSceneRect.yMax() + yadd);
}

void XYScene::autoZoom()
{
    QPointF firstPoint;
    for (int i = 0; i < m_scatterplots.size(); ++i) {
        if (!m_scatterplots[i]->isEmpty()) {
            firstPoint = m_scatterplots[i]->first();
            break;
        }
    }

    qreal xmin = firstPoint.x();
    qreal xmax = xmin;
    qreal ymin = firstPoint.y();
    qreal ymax = ymin;

    for (int i = 0; i < m_scatterplots.size(); ++i) {
        for (int j = 1; j < m_scatterplots[i]->size(); ++j) {
            if (m_scatterplots[i]->at(j).x() < xmin)
                xmin = m_scatterplots[i]->at(j).x();
            if (m_scatterplots[i]->at(j).x() > xmax)
                xmax = m_scatterplots[i]->at(j).x();
            if (m_scatterplots[i]->at(j).y() < ymin)
                ymin = m_scatterplots[i]->at(j).y();
            if (m_scatterplots[i]->at(j).y() > ymax)
                ymax = m_scatterplots[i]->at(j).y();
        }
    }

    if (xmin != xmax && ymin != ymax)
        setZoom(xmin, xmax, ymin, ymax);
}

void XYScene::focusOn(qreal x, qreal y)
{
    const qreal w = m_realSceneRect.width() / 2.0;
    const qreal h = m_realSceneRect.height() / 2.0;

    setZoom(x - w, x + w, y - h, y + h);
}

void XYScene::keyPressEvent(QKeyEvent *keyEvent)
{
    QGraphicsScene::keyPressEvent(keyEvent);
    keyEvent->accept();

    const qreal addw = m_realSceneRect.width() / 10.0;
    const qreal addh = m_realSceneRect.height() / 10.0;

    RealZoom z = zoom();
    switch (keyEvent->key()) {
    case Qt::Key_Left :
        z.setXMax(m_realSceneRect.xMax() - addw);
        z.setXMin(m_realSceneRect.xMin() - addw);
        setZoom(z);
        break;
    case Qt::Key_Right :
        z.setXMax(m_realSceneRect.xMax() + addw);
        z.setXMin(m_realSceneRect.xMin() + addw);
        setZoom(z);
        break;
    case Qt::Key_Up :
        z.setYMax(m_realSceneRect.yMax() + addh);
        z.setYMin(m_realSceneRect.yMin() + addh);
        setZoom(z);
        break;
    case Qt::Key_Down :
        z.setYMax(m_realSceneRect.yMax() - addh);
        z.setYMin(m_realSceneRect.yMin() - addh);
        setZoom(z);
        break;
    case Qt::Key_Minus :
        relativeZoom(1.1);
        break;
    case Qt::Key_Plus :
        relativeZoom(0.9);
        break;
    default:
        keyEvent->ignore();
    }

    if (keyEvent->isAccepted())
        regraph();
}

void XYScene::wheelEvent(QGraphicsSceneWheelEvent *wheelEvent)
{
    QGraphicsScene::wheelEvent(wheelEvent);

    const qreal dxMin = (wheelEvent->scenePos().x() - sceneRect().left()) / (sceneRect().width() - 1);
    const qreal dxMax = 1.0 - dxMin;

    const qreal dyMax = (wheelEvent->scenePos().y() - sceneRect().top()) / (sceneRect().height() - 1);
    const qreal dyMin = 1.0 - dyMax;

    const qreal zoom = - wheelEvent->delta() / 1200.0;

    const qreal xMin = m_realSceneRect.xMin() - m_realSceneRect.width() * zoom * dxMin;
    const qreal xMax = m_realSceneRect.xMax() + m_realSceneRect.width() * zoom * dxMax;
    const qreal yMin = m_realSceneRect.yMin() - m_realSceneRect.height() * zoom * dyMin;
    const qreal yMax = m_realSceneRect.yMax() + m_realSceneRect.height() * zoom * dyMax;

    setZoom(xMin, xMax, yMin, yMax);

    regraph();
}

void XYScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsScene::mouseMoveEvent(mouseEvent);

    if (mouseEvent->buttons() & Qt::LeftButton) {
        QPointF delta = mouseEvent->scenePos() - mouseEvent->lastScenePos();
        delta.rx() *= m_realSceneRect.width() / (sceneRect().width() - 1.0);
        delta.ry() *= m_realSceneRect.height() / (sceneRect().height() - 1.0);
        setZoom(m_realSceneRect.xMin() - delta.x(), m_realSceneRect.xMax() - delta.x(),
                m_realSceneRect.yMin() + delta.y(), m_realSceneRect.yMax() + delta.y());
    }

    if (mouseEvent->buttons() & Qt::RightButton) {
        if (m_zoomRect) {
            QRectF rect(qMin(mouseEvent->scenePos().x(), m_zoomRectOrigin.x()),
                        qMin(mouseEvent->scenePos().y(), m_zoomRectOrigin.y()),
                        qAbs(mouseEvent->scenePos().x() - m_zoomRectOrigin.x()),
                        qAbs(mouseEvent->scenePos().y() - m_zoomRectOrigin.y()));

            m_zoomRect->setRect(rect);
        }
    }

    if (m_state & XYScene::SendMouseMove)
        emit mousePosition(image2real(mouseEvent->scenePos()));

    m_mouseDontMove = Qt::NoButton;
}

void XYScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent)
    QGraphicsScene::mousePressEvent(mouseEvent);

    if (mouseEvent->buttons() & Qt::LeftButton) {
        m_timerRegraph->start();
    }

    if (mouseEvent->buttons() & Qt::RightButton) {
        m_zoomRect = addRect(QRectF(mouseEvent->scenePos(), mouseEvent->scenePos()), m_zoomPen);
        m_zoomRectOrigin = mouseEvent->scenePos();
    }

    m_mouseDontMove = mouseEvent->buttons();
}

void XYScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent)
    QGraphicsScene::mouseReleaseEvent(mouseEvent);

    if (m_state & XYScene::SendMouseMove && m_mouseDontMove)
        emit mouseClic(image2real(mouseEvent->scenePos()), m_mouseDontMove);

    if (m_zoomRect) {
        if (m_zoomRect->rect().width() > 0.0 && m_zoomRect->rect().height() > 0.0) {
            setZoom(image2real(m_zoomRect->rect()));
        }
        removeItem(m_zoomRect);
        m_zoomRect = 0;
        regraph();
    }

    m_timerRegraph->stop();
}
