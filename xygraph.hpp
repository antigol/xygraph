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

#include <xygraph_global.hpp>

#include <QGraphicsView>

class QResizeEvent;
class QTimer;

class XYGraph : public QGraphicsView
{
    Q_OBJECT
public:
    explicit XYGraph(QWidget *parent = 0);
    explicit XYGraph(QGraphicsScene *scene, QWidget *parent = 0);

protected:
    virtual void resizeEvent(QResizeEvent *);

public slots:
    void startRegraphTimer();
    void resizeScene();

private:
    inline void initialisation();
    QTimer *m_timer;
};
