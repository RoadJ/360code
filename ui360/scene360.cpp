#include "scene360.h"

#include <QGraphicsSceneMouseEvent>

#include<QGraphicsScene>
#include<QPainter>
#include<QPointF>
#include<QToolTip>
#include<QMouseEvent>
#include <QDebug>

 Scene360::Scene360(QObject *parent):
    QGraphicsScene(parent)
{
//    this->setMouseTracking(true);
    //setMouseTracking(true);

}
void  Scene360::drawBackground (QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect)
    //QPixmap pixmap(":/ico/icons/Jellyfish.jpg");
    //QRectF   theRect(pixmap.rect());

//    painter->fillRect(oneimg_theRect, this->backgroundBrush());


}


void Scene360::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
//void MainScene::mouseMoveEvent(QMouseEvent *event)
{
    QPointF pos = QPointF(event->scenePos().x(),event->scenePos().y());
//     QPointF  pos = QPointF(event->x(),event->y());
//    qDebug()<<"tttt" <<"x = " <<event->scenePos().x() <<"," << "y= " <<event->scenePos().y();

    //QToolTip::showText(QPoint(,pos.y()),"zsh");

    emit scene360_mouseMove( pos.x(), pos.y());
    //QGraphicsScene::mouseMoveEvent(event);


}


