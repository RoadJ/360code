#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QEvent>
#include <QObject>
#include <QGraphicsView>

#include<opencv2/opencv.hpp>
#include<opencv2/videoio.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include "myitem.h"


class QImage;
class QGraphicsScene;
class QGraphicsView;
class QMouseEvent;
class Review;
class MainScene;
class Scene360;
class MyItem;



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_actionFileOpen_triggered();

    void on_actionCoordinate_calibration_triggered();
    void  scene_mouse_press_slot(qreal x, qreal y);
    void  scene_mouse_move_slot(qreal x, qreal y);
    void  Perspective_transformation_slot();
    void scene360_mouse_move_slot(qreal x, qreal y);

    void on_action360view_triggered();

    void on_btn2_clicked();

    void on_v360_clicked();

    void on_radioButton_1_clicked();

    void on_radioButton_2_clicked();

    void on_radioButton_3_clicked();

    void on_radioButton_4_clicked();

protected:
//    bool eventFilter(QObject *obj, QEvent  *event);
    void paintEvent(QPaintEvent *event);

private:
    Review * newView;
    unsigned short actual_h ;
    unsigned short actual_w ;
    unsigned short actual_x;
    unsigned short actual_y;
    QGraphicsView * gview;

    Ui::MainWindow *ui;
    unsigned char mouse_pre_cnt;
    QImage image,image1, image2, image3, image4;
    cv::Mat mimg, mimgx;
    cv::Mat mimgx1;
    cv::Mat mimgx2;
    cv::Mat mimgx3;
    cv::Mat mimgx4;
    bool open_flag;
    bool open_type;
    bool cali_flag;
     cv::Mat FullScreamFig;
     cv::Mat MscalFig;
    //--------------------
    MainScene * scene;

    QGraphicsItem * item ;
    QGraphicsItem * item360;
    unsigned short cordic_x, cordic_y;
    QPoint org_coordix0,org_coordix1,org_coordix2,org_coordix3;

    //bool eventFilter(QObject *obj, QEvent  *event);
//            bool graphicsView->eventFilter(QObject *obj, QMouseEvent  *event);

     double zoomDouble = 0.4;
     cv::Point *UTL1Array;
    cv::Mat rgb;
    cv::Mat mapx;
    cv::Mat mapy;
    cv::Mat intrinsic_matrix;
    cv::Mat distortion_coeffs;
    //--------------------------------
    Scene360 *scene360;
    cv::Mat mimg_fish2dist;
    //item----------------------------
//    MyItem item;
//    MyItem item1;
//    MyItem item2;
//    MyItem item3;
//    MyItem item4;

    unsigned short calib_x0,calib_y0;
    unsigned short calib_x1,calib_y1;
    unsigned short calib_x2,calib_y2;
    unsigned short calib_x3,calib_y3;
    unsigned char fig_index;
    unsigned char fig_index_temp;
protected:


//    void mousePressEvent(QMouseEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);
//    void QWidget::mouseReleaseEvent(QMouseEvent *event)


};

class Review : public QObject
{
    Q_OBJECT
public:
    Review(QObject *parent = Q_NULLPTR);
    ~Review();
protected:
//   bool eventFilter(QObject *obj, QEvent  *event);


//        void mousePressEvent(QMouseEvent *event);
//        void mouseMoveEvent(QMouseEvent *event);

};

#endif // MAINWINDOW_H
