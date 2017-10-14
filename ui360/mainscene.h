#ifndef MAINSCENE_H
#define MAINSCENE_H

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include<QMouseEvent>

class MainScene : public QGraphicsScene
{
    Q_OBJECT
public:
    MainScene(QObject *paraent=0);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
//    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent  *event);

//    void mouseMoveEvent(QMouseEvent *event);

  //  void enterEvent(QEvent *event);
  //  void leaveEvent ( QEvent * event );
    virtual void  drawBackground (QPainter *painter, const QRectF &rect);
signals:
   void scene_mouse_press(qreal x,qreal y);
   void scene_mouseMove(qreal x,qreal y);
   void scene_mouse_rightpress();

//   void enter_scene_s();
 //  void leave_scene_s();
public:
   QPixmap pixmap;
   QPixmap mypixmap ;
   QPixmap crosspixmap  ;
};

#endif // MAINSCENE_H
