#ifndef SCENE360_H
#define SCENE360_H

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include<QMouseEvent>


class Scene360 : public QGraphicsScene
{
    Q_OBJECT
public:
    Scene360(QObject *paraent=0);

protected:
//    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    virtual void  drawBackground (QPainter *painter, const QRectF &rect);
signals:
//   void scene_mouse_press(qreal x,qreal y);
   void scene360_mouseMove(qreal x,qreal y);
//   void scene_mouse_rightpress();

public:
   QPixmap pixmap;
   QPixmap mypixmap ;
   QPixmap crosspixmap  ;
};

#endif // SCENE360_H
