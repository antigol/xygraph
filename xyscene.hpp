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

#ifndef XYSCENE_H
#define XYSCENE_H

#include "xygraph_global.hpp"
#include "realzoom.hpp"

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QTimer>

class XYFunction;
class XYScatterplot;
struct XYLook
{
    QPen axesPen;
    QPen subaxesPen;
    QColor textColor;
    QPen zoomPen;
    QBrush backgroundBrush;
};

class XYGRAPHSHARED_EXPORT XYScene : public QGraphicsScene {
    Q_OBJECT
public:
    enum Status {
        //                                 Par defaut
        SendMouseMove         = 0x1000, //             Envoie les coordonées souris par le signal 'coordonneesSouris(QPointF)'
        RegraphOnResize       = 0x2000, //     x       Redessine quand il recoit le signal 'sceneRectChanged(QRectF)'
        SendZoomChanged       = 0x4000, //             Envoie un signal
        AutoZoomOnDoubleClick = 0x8000  //     x       Rezoom sur les nuages de points quand on double clique sur le graph
    };

    explicit XYScene(QObject *parent = 0);
    virtual ~XYScene();

    QList<const XYFunction *> &getFunctionsList();
    void addFunction(const XYFunction *); // similaire getFunctionsList().append(...);
    QList<const XYScatterplot *> &getScatterplotList();
    void addScatterplot(const XYScatterplot *); // similaire getScatterplotList().append(...);

    const RealZoom &zoom() const;
    void setZoom(const RealZoom &zoom);
    void setZoom(qreal xmin, qreal xmax, qreal ymin, qreal ymax);
    void setXMin(qreal xmin);
    void setXMax(qreal xmax);
    void setYMin(qreal ymin);
    void setYMax(qreal ymax);

    void relativeZoom(qreal k); // si k < 1.0 ca zoom sinon ca dezoom
    void autoZoom(); // zoom automatique autour des XYScatterplot
    void focusOn(qreal x, qreal y);

    int state() const;
    void setState(int);

    void setAxesPen(const QPen &pen);
    void setSubaxesPen(const QPen &pen);
    void setTextColor(const QColor &color);
    void setZoomPen(const QPen &pen);
    void setLook(const struct XYLook &look);
    struct XYLook look() const;

    inline qreal xr2i(qreal xr) const;
    inline qreal xi2r(qreal xi) const;
    inline qreal yr2i(qreal yr) const;
    inline qreal yi2r(qreal yi) const;
    inline qreal wr2i(qreal wr) const;
    inline qreal wi2r(qreal wi) const;
    inline qreal hr2i(qreal hr) const;
    inline qreal hi2r(qreal hi) const;
    inline QPointF real2image(const QPointF &p) const;
    inline QPointF image2real(const QPointF &p) const;
    inline QRectF real2image(const RealZoom &r) const;
    inline RealZoom image2real(const QRectF &r) const;

signals:
    void mousePosition(QPointF rpos);
    void mouseClic(QPointF rpos, Qt::MouseButtons = 0);
    void zoomChanged();

public slots:
    void regraph();

private slots:
    void maiRegraph();

private:
    void drawaxes();
    void drawfunctions();
    void drawpoints();

    void keyPressEvent(QKeyEvent *keyEvent);
    void wheelEvent(QGraphicsSceneWheelEvent *wheelEvent);

    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    QTimer *m_timerRegraph;
    QTimer *m_timerZoom;

    QPointF m_zoomRectOrigin;
    QGraphicsRectItem *m_zoomRect;

    QPen m_axesPen;
    QPen m_subaxesPen;
    QColor m_textColor;
    QPen m_zoomPen;
    //	struct XYLook m_look;

    RealZoom m_realSceneRect;

    QList<const XYFunction *> m_functions;
    QList<const XYScatterplot *> m_scatterplots;

    int m_state;
    Qt::MouseButtons m_mouseDontMove;
};

class XYGRAPHSHARED_EXPORT XYFunction
{
public:
    XYFunction(const QPen &pen = QPen());
    XYFunction(const XYFunction &other);
    void setVisible(bool visible);
    bool isVisible() const;
    void setPen(const QPen &pen);
    virtual qreal y(qreal x) const = 0;
    virtual bool domain(qreal x) const;

private:
    bool m_visible;
    QPen m_pen;

    friend class XYScene;
};

class XYGRAPHSHARED_EXPORT XYScatterplot : public QList<QPointF>
{
public:
    XYScatterplot(const QPen &pen = QPen(), const QBrush &brush = QBrush(), qreal r = 2.0, const QPen &linePen = QPen(Qt::NoPen));
    XYScatterplot(const QList<QPointF> &points, const QPen &pen = QPen(), const QBrush &brush = QBrush(), qreal r = 2.0, const QPen &linePen = QPen(Qt::NoPen));
    QList<QPointF> &getPoints();
    const QList<QPointF> &getPoints() const;
    void setVisible(bool visible);
    bool isVisible() const;
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setRadius(qreal r);
    void setLinePen(const QPen &pen);

private:
    bool m_visible;
    QPen m_pen;
    QBrush m_brush;
    qreal m_r;
    QPen m_linepen;

    friend class XYScene;
};


/* INLINE IMPLANTATION */

qreal XYScene::xr2i(qreal xr) const {
    return (xr - m_realSceneRect.xMin()) / m_realSceneRect.width() * (sceneRect().width() - 1) + sceneRect().left();
}

qreal XYScene::xi2r(qreal xi) const {
    return (xi - sceneRect().left()) / (sceneRect().width() - 1) * m_realSceneRect.width() + m_realSceneRect.xMin();
}

qreal XYScene::yr2i(qreal yr) const {
    return - (yr - m_realSceneRect.yMax()) / m_realSceneRect.height() * (sceneRect().height() - 1) + sceneRect().top();
}

qreal XYScene::yi2r(qreal yi) const {
    return - (yi - sceneRect().top()) / (sceneRect().height() - 1) * m_realSceneRect.height() + m_realSceneRect.yMax();
}

qreal XYScene::wr2i(qreal wr) const {
    return wr / m_realSceneRect.width() * (sceneRect().width() - 1);
}

qreal XYScene::wi2r(qreal wi) const {
    return wi / (sceneRect().width() - 1) * m_realSceneRect.width();
}

qreal XYScene::hr2i(qreal hr) const {
    return hr / m_realSceneRect.height() * (sceneRect().height() - 1);
}

qreal XYScene::hi2r(qreal hi) const {
    return hi / (sceneRect().height() - 1) * m_realSceneRect.height();
}

QPointF XYScene::real2image(const QPointF &real) const {
    return QPointF(xr2i(real.x()), yr2i(real.y()));
}

QPointF XYScene::image2real(const QPointF &image) const {
    return QPointF(xi2r(image.x()), yi2r(image.y()));
}

QRectF XYScene::real2image(const RealZoom &real) const {
    return QRectF(QPointF(xr2i(real.xMin()), yr2i(real.yMax())), QPointF(xr2i(real.xMax()), yr2i(real.yMin())));
}

RealZoom XYScene::image2real(const QRectF &image) const {
    return RealZoom(xi2r(image.left()), xi2r(image.right()), yi2r(image.bottom()), yi2r(image.top()));
}

#endif // XYSCENE_H
