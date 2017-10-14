#include "mainscene.h"

#include <QGraphicsSceneMouseEvent>

#include<QGraphicsScene>
#include<QPainter>
#include<QPointF>
#include<QToolTip>
#include<QMouseEvent>
#include <QDebug>

MainScene::MainScene(QObject *parent):
    QGraphicsScene(parent)
{
//    this->setMouseTracking(true);
    //setMouseTracking(true);

}
void  MainScene::drawBackground (QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect)
    //QPixmap pixmap(":/ico/icons/Jellyfish.jpg");
    //QRectF   theRect(pixmap.rect());

//    painter->fillRect(oneimg_theRect, this->backgroundBrush());


}

#if 1
void MainScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF pos = QPointF(event->scenePos().x(),event->scenePos().y());
//    QPointF mypos = QPointF(event->pos().x(),event->pos().y());

//    qDebug()<<"pos" <<"x = " <<event->scenePos().x() <<"," << "y= " <<event->scenePos().y();
//    qDebug()<<"mypos" <<"x = " <<event->pos().x() <<"," << "y= " <<event->pos().y();


    if(event->button() == Qt::LeftButton)
    {
       emit scene_mouse_press( pos.x(), pos.y());
    }
//    else
//    {
//        emit scene_mouse_rightpress();
//    }
    QGraphicsScene::mousePressEvent(event);
}
#else
void MainScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
    QPointF pos = QPointF(event->scenePos().x(),event->scenePos().y());
    emit scene_mouse_press( pos.x(), pos.y());;//  pos.x(), pos.y());

}


#endif

#if 1
void MainScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
//void MainScene::mouseMoveEvent(QMouseEvent *event)
{
    QPointF pos = QPointF(event->scenePos().x(),event->scenePos().y());
//     QPointF  pos = QPointF(event->x(),event->y());
//    qDebug()<<"tttt" <<"x = " <<event->scenePos().x() <<"," << "y= " <<event->scenePos().y();

    //QToolTip::showText(QPoint(,pos.y()),"zsh");

    emit scene_mouseMove( pos.x(), pos.y());
    //QGraphicsScene::mouseMoveEvent(event);


}
#endif
