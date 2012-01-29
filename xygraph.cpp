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

#include "xygraph.hpp"

#include <QResizeEvent>
#include <QTimer>

XYGraph::XYGraph(QWidget *parent) :
    QGraphicsView(parent)
{
    initialisation();
}

XYGraph::XYGraph(QGraphicsScene *scene, QWidget *parent) :
    QGraphicsView(scene, parent)
{
    initialisation();
}

inline void XYGraph::initialisation()
{
    setFocusPolicy(Qt::WheelFocus);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_timer = new QTimer(this);
    m_timer->setInterval(100);
    m_timer->setSingleShot(true);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(resizeScene()));
}

void XYGraph::startRegraphTimer()
{
    if (scene()) {
        m_timer->start();
//        scene()->clear();
    }
}

void XYGraph::resizeEvent(QResizeEvent *e)
{
    startRegraphTimer();

    QGraphicsView::resizeEvent(e);
}

void XYGraph::resizeScene()
{
    if (scene())
        scene()->setSceneRect(geometry());
}
