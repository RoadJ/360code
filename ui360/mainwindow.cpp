#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"mainscene.h"
#include "scene360.h"
#include "myitem.h"
#include<opencv2/opencv.hpp>
#include<opencv2/videoio.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

#include<QGraphicsScene>
#include<QGraphicsView>
#include<QPalette>

#include<QMouseEvent>
#include <QEvent>
#include <QObject>
#include <QToolTip>
#include <QGraphicsPixmapItem>

using namespace std;
using namespace cv;
#define  MAXHW  1280
//#define  SCREEM_H  800
//#define  SCREEM_W  1280
#define  SCREEM_H  720
#define  SCREEM_W  420

#define  LEFT_UPx   120
#define  LEFT_UPy   165

#define  RIGHT_UPx   300
#define  RIGHT_UPY   165

#define  LEFT_DOWNx   120
#define  LEFT_DOWNy   555

#define  RIGHT_DOWNx   300
#define  RIGHT_DOWNY   555
float K1 = (float)LEFT_UPy/LEFT_UPx;
float K2 = (float)RIGHT_UPY/(RIGHT_UPx - SCREEM_W);

  static unsigned char file_index = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    actual_h = ui->graphicsView->height();
    actual_w = ui->graphicsView->width();
//    gview = new QGraphicsView(this);
//    scene = new QGraphicsScene(this);
//    newView = new Review(this);
     open_flag = false;
     cali_flag = false;
     fig_index = 1;
     fig_index_temp = 1;
       MscalFig = Mat::zeros(MAXHW,MAXHW,CV_8UC3);
       FullScreamFig = Mat::zeros(SCREEM_H,SCREEM_W,CV_8UC3);
//    ui->textBrowser_1->setFont();
        QPalette pal;
        pal = ui->textBrowser_1->palette();
        pal.setColor(QPalette::Base, QColor(255,255,0));//改变背景色

        ui->textBrowser_1->setCurrentFont(QFont("Times New Roman",10));
        ui->textBrowser_2->setCurrentFont(QFont("Times New Roman",10));
        ui->textBrowser_3->setCurrentFont(QFont("Times New Roman",10));
        ui->textBrowser_4->setCurrentFont(QFont("Times New Roman",10));

        ui->textBrowser_1->setText(tr("(0,0)"));
        ui->textBrowser_2->setText(tr("(0,0)"));
        ui->textBrowser_3->setText(tr("(0,0)"));
        ui->textBrowser_4->setText(tr("(0,0)"));

        ui->textBrowser_1->setPalette(pal);
        ui->textBrowser_2->setPalette(pal);
        ui->textBrowser_3->setPalette(pal);
        ui->textBrowser_4->setPalette(pal);

        //------construct input-------------------------------------
        ui->lineEdit->setValidator(new QIntValidator(0, 10000, ui->lineEdit));
        ui->lineEdit_0->setValidator(new QIntValidator(0, 10000, ui->lineEdit_0));
        ui->lineEdit_1->setValidator(new QIntValidator(0, 10000, ui->lineEdit_1));
        ui->lineEdit_2->setValidator(new QIntValidator(0, 10000, ui->lineEdit_2));
        ui->lineEdit_3->setValidator(new QIntValidator(0, 10000, ui->lineEdit_3));
        ui->lineEdit_4->setValidator(new QIntValidator(0, 10000, ui->lineEdit_4));
        ui->lineEdit_5->setValidator(new QIntValidator(0, 10000, ui->lineEdit_5));
        ui->lineEdit_6->setValidator(new QIntValidator(0, 10000, ui->lineEdit_6));

          ui->lineEdit->setText("400");
          ui->lineEdit_0->setText("300");
          ui->lineEdit_1->setText("500");
          ui->lineEdit_2->setText("300");

          ui->lineEdit_3->setText("500");
          ui->lineEdit_4->setText("400");

          ui->lineEdit_5->setText("400");
          ui->lineEdit_6->setText("400");

          //    corners_trans[0] = Point2f(200,150);
          //    corners_trans[1] = Point2f(620,150);
          //    corners_trans[2] = Point2f(320,300);
          //    corners_trans[3] = Point2f(500,300);

        mouse_pre_cnt = 0;
        open_type = false;
//        ui->graphicsView->installEventFilter( this);

//         ui->graphicsView->installEventFilter(this);


//         ui->graphicsView->resize(971,700);
//        this->installEventFilter(this);
//        ui->graphicsView->setMouseTracking(true);
         scene = new MainScene(ui->graphicsView);
         scene360 = new Scene360(ui->graphicsView360);
         //item = new QGraphicsPixmapItem();
         item = NULL;
         //         item =  scene->addPixmap(ui->graphicsView);

         connect(scene,SIGNAL(scene_mouse_press(qreal,qreal)), this,SLOT(scene_mouse_press_slot(qreal,qreal)));
         connect(scene,SIGNAL(scene_mouseMove(qreal,qreal)), this,SLOT(scene_mouse_move_slot(qreal,qreal)));
         connect(scene360,SIGNAL(scene360_mouseMove(qreal,qreal)), this,SLOT(scene360_mouse_move_slot(qreal,qreal)));

           ui->graphicsView->setScene(scene);
           ui->graphicsView360->setScene(scene360);
//  this->setMouseTracking(true);
             ui->graphicsView360->setMouseTracking(true);

//             ui->graphicsView->show();
            connect(ui->btn0,SIGNAL(clicked(bool)),this, SLOT(Perspective_transformation_slot()));

         ui->graphicsView360->hide();


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionFileOpen_triggered()
{

    QImage itemp;
    Mat mmg;
    QString fileName  = QFileDialog::getOpenFileName(this,tr("Open Image"),"F:/zsh/cam/cam/",tr("Image File(*.bmp *.jpg *.jpeg *.png)"));
     gview = new QGraphicsView(this);


    if(fileName.isEmpty())
    {
        return;
    }

    else
    {
        QFileInfo info(fileName);
        //qDebug() << info.baseName();//不包括后缀名
        //qDebug() << info.bundleName();//只有在Mac OS才有用，其它平台为空字符串
        qDebug() << info.suffix();//后缀名
        qDebug() << fileName;//后缀名

         ui->graphicsView->setMouseTracking(true);

        if(info.suffix() =="jpg" || info.suffix() =="png")
        {
              mimg = cv::imread(fileName.toLatin1().data());
        # if 0
//                cv::resize(mimg,MscalFig,MscalFig.size(),  CV_INTER_CUBIC);
                cv::resize(mimg,mmg,Size(0,0),1.5,1.5 , CV_INTER_CUBIC);            
//                Mat image= imreadimag）；
                Rect rect(330, 0, SCREEM_W, SCREEM_H);
//                Mat image_roi = image(rect);
                mmg(rect).copyTo(MscalFig);
//                imshow("12345",MscalFig);
            #endif;
              qDebug()<<"channel = "<<mimg.channels();

                for (int i =0; i<mimg.rows; i++)
                {
                    for(int j = 0 ; j < mimg.cols; j ++)
                    {
//                       MscalFig.row(i) = mimg[i][]j
                      memcpy(&MscalFig.at<cv::Vec3b>(i,j)[0],&mimg.at<cv::Vec3b>(i,j)[0],3);
                    }
                }
//                  imshow("12345",MscalFig);




                open_flag = true;
              Mat rgb;

                  switch(fig_index_temp)
                  {
                  case  1 : ui->radioButton_1->setChecked(true);
                            ui->radioButton_2->setChecked(false);
                            ui->radioButton_3->setChecked(false);
                            ui->radioButton_4->setChecked(false);
                            mimgx1 = MscalFig;
                                  cvtColor(MscalFig,rgb,CV_BGR2RGB);
                                  image1 =QImage((const unsigned char*)(rgb.data),
                                  rgb.cols,rgb.rows,
                                  rgb.cols*rgb.channels(),
                                  QImage::Format_RGB888).copy();
                                  itemp  = image1;
                                  fig_index = 1;
                                  fig_index_temp = 2;
                         break;
                  case  2 :  ui->radioButton_1->setChecked(false);
                             ui->radioButton_2->setChecked(true);
                             ui->radioButton_3->setChecked(false);
                             ui->radioButton_4->setChecked(false);
                              mimgx2 = MscalFig;
                                  cvtColor(MscalFig,rgb,CV_BGR2RGB);
                                  image2 =QImage((const unsigned char*)(rgb.data),
                                  rgb.cols,rgb.rows,
                                  rgb.cols*rgb.channels(),
                                  QImage::Format_RGB888).copy();
                                  itemp  = image2;
                                  fig_index = 2;
                                  fig_index_temp = 3;
                             break;
                  case  3 :  ui->radioButton_1->setChecked(false);
                             ui->radioButton_2->setChecked(false);
                             ui->radioButton_3->setChecked(true);
                             ui->radioButton_4->setChecked(false);
                              mimgx3 = MscalFig;
                                  cvtColor(MscalFig,rgb,CV_BGR2RGB);
                                  image3 =QImage((const unsigned char*)(rgb.data),
                                  rgb.cols,rgb.rows,
                                  rgb.cols*rgb.channels(),
                                  QImage::Format_RGB888).copy();
                                  itemp  = image3;
                                  fig_index = 3;
                                  fig_index_temp = 4;break;
                  case  4 :  ui->radioButton_1->setChecked(false);
                             ui->radioButton_2->setChecked(false);
                             ui->radioButton_3->setChecked(false);
                             ui->radioButton_4->setChecked(true);
                              mimgx4 = MscalFig;
                                  cvtColor(MscalFig,rgb,CV_BGR2RGB);
                                  image4 =QImage((const unsigned char*)(rgb.data),
                                  rgb.cols,rgb.rows,
                                  rgb.cols*rgb.channels(),
                                  QImage::Format_RGB888).copy();
                                  itemp  = image4;
                                  fig_index = 4;
                                  fig_index_temp = 1;break;
                  default: break;
                  }
              }


       qDebug()<<fig_index<<"dafdfsf";


      scene->clear();
      scene->addPixmap(QPixmap::fromImage(itemp))->setPos(0,0);
      ui->graphicsView->setScene(scene);
//      ui->graphicsView->adjustSize();


//                 update();

   }
}

void MainWindow::on_actionCoordinate_calibration_triggered()
{
    mouse_pre_cnt = 0;
       ui->textBrowser_1->setText(tr("(0,0)"));
       ui->textBrowser_2->setText(tr("(0,0)"));
       ui->textBrowser_3->setText(tr("(0,0)"));
       ui->textBrowser_4->setText(tr("(0,0)"));
}

Review::Review(QObject *parent):QObject(parent)
{

}

Review::~Review()
{

}


void MainWindow::scene_mouse_press_slot(qreal x, qreal y)
{
    QString str="("+QString::number(x)+","+QString::number(y)+")";
//    if( (mouse_event->buttons() == Qt::LeftButton) )
//    {
//        qDebug() <<str;
//    }
//    QMessageBox::about(NULL, "About", "About this application");

          mouse_pre_cnt ++;
            switch(mouse_pre_cnt)
            {
                case 1:    ui->textBrowser_1->setText(str);
                           org_coordix0.rx()  = int(x); org_coordix0.ry() = int(y); break;
                case 2:    ui->textBrowser_2->setText(str);
                           org_coordix1.rx()  = int(x); org_coordix1.ry() = int(y); break;
                case 3:    ui->textBrowser_3->setText(str);
                           org_coordix2.rx()  = int(x); org_coordix2.ry() = int(y); break;
                case 4:    ui->textBrowser_4->setText(str);
                           org_coordix3.rx()  = int(x); org_coordix3.ry() = int(y); break;
                default : break;
            }
            qDebug() << org_coordix0.rx();
            qDebug() << org_coordix0.ry();
}

void MainWindow::scene_mouse_move_slot(qreal x,qreal y)
{
     QPainter *paint = new QPainter(this);
     cordic_x = int(x);
     cordic_y = int(y);
       //if((x >0) & (x <current_grayImg->width()) & (y>0) & (y< current_grayImg->height()) )
//            ui->statusbar->showMessage("x="+QString::number(int(x))+",y="+QString::number(int(y)));//+",v="+QString::number(rawmaxdata[int(x) + int(y)*current_grayImg->width()]));
        QString str ="("+QString::number(x)+","+QString::number(y)+")";

          ui->statusBar->showMessage("x="+QString::number(int(x))+",y="+QString::number(int(y)));
             //------loacl-scale-----------------------------
             unsigned int width=32, height=32;
             scene->mypixmap =  scene->mypixmap.grabWidget( ui->graphicsView,int(x)-width/2,int(y)-height/2,width,height);
             QImage crosstmp =scene->mypixmap.toImage();
             QRgb value = qRgb(0,0,255);
             for(int i=0;i<width;i++)
             {
                 for(int j=height/2-1;j<height/2;j++)
                 {
                     crosstmp.setPixel(i,j,value);
                 }
             }
             for(int i=width/2-1;i<width/2;i++)
             {
                 for(int j=0;j<height;j++)
                 {
                     crosstmp.setPixel(i,j,value);
                 }
             }
             scene->crosspixmap = QPixmap::fromImage( crosstmp.scaled(width*5,height*5,Qt::KeepAspectRatio) );
             scene->pixmap = scene->mypixmap.scaled(width * 5,height * 5,Qt::KeepAspectRatio);
            update();
           // ui->statusbar->showMessage(tr(""));

}

void MainWindow::scene360_mouse_move_slot(qreal x,qreal y)
{
       //if((x >0) & (x <current_grayImg->width()) & (y>0) & (y< current_grayImg->height()) )
//            ui->statusbar->showMessage("x="+QString::number(int(x))+",y="+QString::number(int(y)));//+",v="+QString::number(rawmaxdata[int(x) + int(y)*current_grayImg->width()]));
        QString str ="("+QString::number(x)+","+QString::number(y)+")";
        ui->statusBar->showMessage("x="+QString::number(int(x))+",y="+QString::number(int(y)));
       //------loacl-scale-----------------------------

}

void MainWindow::paintEvent(QPaintEvent *)
{
    static int counttest = 0;


        if (open_flag  && !cali_flag)
        {

           scene->removeItem(item);
           item =  scene->addPixmap(scene->pixmap);
           scene->addPixmap(scene->crosspixmap);

//           item->setPos(600,500);


//           scene->clear();
//            scene->addPixmap(QPixmap::fromImage(image1));
//           ui->graphicsView->setScene(scene);

//        }



//        else if(open_flag  && cali_flag)
//        {
//              scene->removeItem(item);
//              scene->clear();

//        switch(fig_index)
//        {
//            case 1 :scene->addPixmap(QPixmap::fromImage(image1));
//                       break;
//            case 2 :scene->addPixmap(QPixmap::fromImage(image2));
//                       break;
//            case 3 :scene->addPixmap(QPixmap::fromImage(image3));
//                       break;
//            case 4 :scene->addPixmap(QPixmap::fromImage(image4));
//                       break;
//            default :break;

//        }
//      scene->addPixmap(scene->crosspixmap);
  }

//     ui->graphicsView->setScene(scene);
//     ui->graphicsView->adjustSize();
//      ui->graphicsView->show();

}


//       if(open_type  && cali_flag)
//       {
//        scene360->clear();
//        item360 = scene360->addPixmap(QPixmap::fromImage(image));
////        item360->setPos(0,100);
//         ui->graphicsView360->adjustSize();
//       }
       //scene->setBackgroundBrush(QBrush(QImage(image)));
//        ui->graphicsView360->setScene(scene360);
//        ui->graphicsView360->adjustSize();
//        ui->graphicsView360->show();

   //usleep(10000);

//    scene->addPixmap(scene->pixmap);
//      scene->addPixmap(scene->crosspixmap)->setPos(0,0);;


   //ui->graphicsView->setScene(scene);
//   ui->graphicsView->adjustSize();

//   ui->graphicsView->show();
   //---------------------------
//  if(open_flag == true)
//  {
//      QVector<QPoint> point;
//      point<<QPoint(0,0)<<QPoint(200,0)<<QPoint(50,50)<<QPoint(100,50);
//      QGraphicsPolygonItem *item = new QGraphicsPolygonItem QPolygonF(point);
//      item->setBrush(QColor(qrand()%256,qrand()%256,qrand()%256));
//      item->setFlag(QGraphicsItem::ItemIsMovable);
//      scnen360->addItem(item);
//      item->setPos((qrand()%int(scene->scene->sceneRect().width())));
//   QPainter painter(this);
//   painter.setRenderHint(QPainter::Antialiasing,true);

//   QPolygonF m_polygonfLeft;
//   m_polygonfLeft<<QPointF(160.0,190.0)<<QPointF(160.0,210.0)<<QPointF(225.0,188.0)<<QPointF(225.0,165.0);
//   m_polygonfRight<<QPointF(225.0,165.0)<<QPointF(225.0,190.0)<<QPointF(305.0,205.0)<<QPointF(305.0,185.0);

//   painter.setPen(QPen(Qt::white,1,Qt::SolidLine,Qt::RoundCap));
//   painter.setBrush(QBrush(QColor(208,105,38,120),Qt::SolidPattern));
//   painter.drawPolygon(m_polygonfLeft,Qt::WindingFill);

  //}




void MainWindow::Perspective_transformation_slot( )
{

    if(open_flag == false)
        return;






    QString str0 = ui->lineEdit->text();
    QString str1 = ui->lineEdit_0->text();
    QString str2 = ui->lineEdit_1->text();
    QString str3 = ui->lineEdit_2->text();
    QString str4 = ui->lineEdit_3->text();
    QString str5 = ui->lineEdit_4->text();
    QString str6 = ui->lineEdit_5->text();
    QString str7 = ui->lineEdit_6->text();
    calib_x0 = str0.toInt() ;
    calib_y0 = str1.toInt() ;
    calib_x1 = str2.toInt() ;
    calib_y1 = str3.toInt() ;
    calib_x2 = str4.toInt() ;
    calib_y2 = str5.toInt() ;
    calib_x3 = str6.toInt() ;
    calib_y3 = str7.toInt() ;

    qDebug()<<calib_x0;qDebug()<<calib_y0;qDebug()<<calib_x1;qDebug()<<calib_y1;
    qDebug()<<calib_x2;qDebug()<<calib_y2;qDebug()<<calib_x3;qDebug()<<calib_y3;



    int img_height = MAXHW;
    int img_width = MAXHW;
//    int img_height =  SCREEM_H;
//    int img_width = SCREEM_W;


//    cout<<"img.channels() = " <<mimg.channels();
    vector<Point2f> corners(4);//扭曲坐标

    corners[0] = Point2f(org_coordix0.x(),org_coordix0.y());
    corners[1] = Point2f(org_coordix1.x(),org_coordix1.y());
    corners[2] = Point2f(org_coordix2.x(),org_coordix2.y());
    corners[3] = Point2f(org_coordix3.x(),org_coordix3.y());

    vector<Point2f> corners_trans(4); //平视坐标

    corners_trans[0] = Point2f(calib_x0,calib_y0);
    corners_trans[1] = Point2f(calib_x1,calib_y1);
    corners_trans[2] = Point2f(calib_x2,calib_y2);
    corners_trans[3] = Point2f(calib_x3,calib_y3);



    Mat transform = getPerspectiveTransform(corners_trans,corners);//--------------
    cout<<transform<<endl;
    vector<Point2f> ponits, points_trans;
    for(int i=0;i<img_height;i++){
        for(int j=0;j<img_width;j++){
            ponits.push_back(Point2f(j,i));
        }
    }

    perspectiveTransform( ponits, points_trans, transform);
    //------------合成一个表-------------------------------------------
 #define  two_table

 #ifdef two_table
    unsigned int cnt = 0;
    for(int i=0;i<img_height;i++)
    {
        for(int j=0;j<img_width;j++)
        {
            int y = points_trans[cnt].y;
            int x = points_trans[cnt].x;
           {
             if(y > img_height-1)
              y = img_height-1;
             else if(y<0)
              y = 0;

             if(x > img_width -1)
              x =img_width -1;
             else if(x<0)
              x = 0;
          }

/*          int ROW = y * img_width + x;
          int ROW = y * img_width + x;

          if (ROW >= mimg.rows-1)
              ROW = mimg.rows -1;
          if (ROW >= mimg.rows-1)
              COL =  mimg.cols -1*/;

          points_trans[cnt].x= UTL1Array[y * img_width + x].x;
          points_trans[cnt].y= UTL1Array[y * img_width + x].y;
          cnt++;
        }
    }
 #endif
    //----------------------------------------------
    Mat mimg_temp;
    switch(fig_index)
    {
        case 1 :   mimg_temp = mimgx1;
                   break;
        case 2 :   mimg_temp = mimgx2;
                   break;
        case 3 :   mimg_temp = mimgx3;
                   break;
        case 4 :   mimg_temp = mimgx4;
                   break;
        default :break;

    }
    //-------------------------------------------


    Mat img_trans = Mat::zeros(img_height,img_width,CV_8UC3);
    int count = 0;
    for(int i=0;i<img_height;i++){
        uchar* t = img_trans.ptr<uchar>(i);
        for(int j=0;j<img_width;j++){
            int y = points_trans[count].y;
            int x = points_trans[count].x;

           {
             if(y>img_height-1)
              y = 255;
             else if(y<0)
              y = 255;
           }
          {
          if(x>img_width-1)
              x =255;
          else if(x<0)
              x = 255;
          }
        #ifdef  two_table
          uchar* p = mimg_temp.ptr<uchar>(y);
        #else
            uchar* p = MscalFig.ptr<uchar>(y);
        #endif

            t[j*3]  = p[ x*3 ];
            t[j*3+1]  = p[x*3+1];
            t[j*3+2]  = p[ x*3+2];

            count++;
        }
    }

//    imshow("distort",img_trans);

  //--UP----------------------------
  //--#####_______________________________________

//     y = K1*x ; y = K2(x-SCREEM_W)

//    1,  UP_area :
//        y = 0  ->LEFT_UPy
//        x = y/K1  -> (y/K2+SCREEM_W)
//    2, Down_area :
//      y = LEFT_DOWNy  ->SCREEM_H
//      x = (y/K2+SCREEM_W)  -> y/K1
//    3, LEFT_area :
//         x = 0  -> LEFT_DOWNx
//         y = x K1 -> k(x-SCREEM_W)
//    4, RIGHT_area :
//        x = RIGHT_UPx  -> SCREEM_W
//        y = K2(x-SCREEM_W) -> xK1

    //-------1,  UP_area :-----------------------
     qDebug()<<"K1="; qDebug()<<K1;
    if(fig_index == 1)
    {
        count = 0;
        for(int i =0; i< LEFT_UPy; i++){
            uchar* t = FullScreamFig.ptr<uchar>(i);
            for(int j = (int)(i/K1); j< (int)(i/K2+SCREEM_W);j++){
                int y = points_trans[i*img_width + j].y;
                int x = points_trans[i*img_width + j].x;

               {
                 if(y>img_height-1)
                  y = 255;
                 else if(y<0)
                  y = 255;

                 if(x>img_width-1)
                  x =255;
                 else if(x<0)
                  x = 255;
              }
                uchar* p = mimgx1.ptr<uchar>(y);

                t[j*3]  = p[ x*3 ];
                t[j*3+1]  = p[x*3+1];
                t[j*3+2]  = p[ x*3+2];

                count++;
            }
        }
    }



    //-------2, Down_area :------LEFT_DOWNy  ->SCREEM_H------x = (y/K2+SCREEM_W)  -> y/K1--------
  if(fig_index == 2)
      {
        count = 0;
        for(int i = LEFT_DOWNy; i< SCREEM_H; i++){
            uchar* t = FullScreamFig.ptr<uchar>(i);
            for(int j = (int)((i-SCREEM_H)/K2); j< (int)((i-SCREEM_H)/K1 + SCREEM_W);j++){
                int y = points_trans[i*img_width + j].y;
                int x = points_trans[i*img_width + j].x;

               {
                 if(y>img_height-1)
                  y = 255;
                 else if(y<0)
                  y = 255;

                 if(x>img_width-1)
                  x =255;
                 else if(x<0)
                  x = 255;
              }
              uchar* p = mimgx2.ptr<uchar>(y);

                t[j*3]  = p[ x*3 ];
                t[j*3+1]  = p[x*3+1];
                t[j*3+2]  = p[ x*3+2];

                count++;
            }
        }
    }
    //-------3, //    3, LEFT_area :
    //         x = 0  -> LEFT_DOWNx
    //         y = x K1 -> k2(x-SCREEM_W)---

  #if 1

  if(fig_index == 3)
    {
        count = 0;
        for(int i = 0; i< LEFT_DOWNx; i++){

            for(int j = (int)(i * K1); j< (int)(K2*i+ SCREEM_H);j++){
                 uchar* t = FullScreamFig.ptr<uchar>(j);
                int y = points_trans[j*img_width + i].y;
                int x = points_trans[j*img_width + i].x;

               {
                 if(y>img_height-1)
                  y = 255;
                 else if(y<0)
                  y = 255;

                 if(x>img_width-1)
                  x =255;
                 else if(x<0)
                  x = 255;
              }
               uchar* p = mimgx3.ptr<uchar>(y);

                t[i*3]  = p[ x*3 ];
                t[i*3+1]  = p[x*3+1];
                t[i*3+2]  = p[ x*3+2];

                count++;
            }
        }
    }

    //    4, RIGHT_area :
    //        x = RIGHT_UPx  -> SCREEM_W
    //        y = K2(x-SCREEM_W) -> xK1
  if(fig_index == 4)
  {
     count = 0;
    for(int i = RIGHT_UPx; i< SCREEM_W; i++){

        for(int j = (int)(K2*(i-SCREEM_W)); j< (int)((i -SCREEM_W) *K1 + SCREEM_H);j++){
            uchar* t = FullScreamFig.ptr<uchar>(j);
            int y = points_trans[j*img_width + i].y;
            int x = points_trans[j*img_width + i].x;
           {
             if(y>img_height-1)
              y = 255;
             else if(y<0)
              y = 255;

            if(x>img_width-1)
              x =255;
            else if(x<0)
              x = 255;
          }
          uchar* p = mimgx4.ptr<uchar>(y);

            t[i*3]  = p[ x*3 ];
            t[i*3+1]  = p[x*3+1];
            t[i*3+2]  = p[ x*3+2];

            count++;
        }
    }
  }
#endif

   imshow("distortaaa",FullScreamFig);


}




void MainWindow::on_action360view_triggered()
{

    if(open_type == true)
    {
        ui->graphicsView360->hide();
        ui->graphicsView->show();

        open_type = false;
    }
    else
    {
        ui->graphicsView360->show();
        open_type = true;
         ui->graphicsView->hide();
//        scene->clear();
    }

    update();
}
//---create table of perspective distort-------------------
//void MainWindow::Perspective_transformation_slot( )



void MainWindow::on_btn2_clicked()
{



           cali_flag = true;

          std::string filename = "F:/OUYANG/PerpectiveTr/Ui360_new/result.xml";
           cv::FileStorage file_in(filename, cv::FileStorage::READ);

           cv::Mat tmp1;
           file_in["intrinsic_matrix"] >>tmp1;
           cv::Mat  intrinsic_matrix(tmp1);
           this->intrinsic_matrix= intrinsic_matrix;

           cv::Mat tmp2;
           file_in["distortion_coeffs"] >> tmp2;
           cv::Mat distortion_coeffs(tmp2);
           this->distortion_coeffs = distortion_coeffs;

           file_in.release();

//           cv::Size image_size = rgb.size();
            cv::Size image_size = MscalFig.size();
           this->mapx = cv::Mat(image_size ,CV_32FC1);
           this->mapy = cv::Mat(image_size ,CV_32FC1);
           cv::Mat R = cv::Mat::eye(3,3,CV_32F);

           cv::Mat new_intrinsic_mat;    //Mat new_intrinsic_mat(3, 3, CV_64FC1, Scalar(0))亦可，注意数据类型;


                   //fx,fy变大（小），视场变小（大），裁剪较多（少），但细节清晰（模糊）；很关键，new_intrinsic_mat决定输出的畸变校正图像的范围
           intrinsic_matrix.copyTo(new_intrinsic_mat);

                   //调整输出校正图的视场
           new_intrinsic_mat.at<double>(0, 0) *= zoomDouble;      //注意数据类型，非常重要
           new_intrinsic_mat.at<double>(1, 1) *= zoomDouble;

                   //调整输出校正图的中心
           new_intrinsic_mat.at<double>(0, 2) += 0.5;
           new_intrinsic_mat.at<double>(1, 2) += 0.5;

           cv::fisheye::initUndistortRectifyMap(intrinsic_matrix,distortion_coeffs,R,new_intrinsic_mat,image_size ,CV_32FC1,mapx,mapy);
           {
               UTL1Array = new cv::Point[mapx.rows*mapx.cols];

               for (int i=0,k=0;i<mapx.rows;i++)
               {
                   for(int j=0;j<mapx.cols;j++)
                   {
                       //
                       UTL1Array[k].x = (int)(mapx.at<float>(i,j));
                       k++;
//                       qDebug()<<k;
                   }
               }

               for (int i=0,k=0;i<mapy.rows;i++)
               {
                   for(int j=0;j<mapy.cols;j++)
                   {
                       //
                       UTL1Array[k].y = (int)(mapy.at<float>(i,j));
                       k++;
                   }
               }
           }
      //-------------------------------------------------------

           int img_height ;
           int img_width ;
           cv::Mat mimg_temp;

           switch(fig_index)
           {
               case 1 :   mimg_temp = mimgx1;
                          break;
               case 2 :   mimg_temp = mimgx2;
                          break;
               case 3 :   mimg_temp = mimgx3;
                          break;
               case 4 :   mimg_temp = mimgx4;
                          break;
               default :break;

           }

            img_height =  MAXHW;
            img_width =  MAXHW;

//          cv::Mat result_img = Mat::zeros(SCREEM_H, SCREEM_W, CV_8UC3);
               mimg_fish2dist = Mat::zeros(img_height, img_width, CV_8UC3);
          int count = 0;
          for(int i=0;i<img_height;i++){
              uchar* t = mimg_fish2dist.ptr<uchar>(i);
              for(int j=0;j<img_width;j++){
                  int y = UTL1Array[count].y;
                  int x = UTL1Array[count].x;
                 {
                   if(y>img_height-1)
                    y = img_height-1;
                   else if(y<0)
                    y = 0;

                   if(x>img_width -1)
                    x =img_width -1;
                   else if(x<0)
                    x = 0;
                }

                  uchar* p = mimg_temp.ptr<uchar>(y);


                  t[j*3]  = p[ x*3 ];
                  t[j*3+1]  = p[x*3+1];
                  t[j*3+2]  = p[ x*3+2];

                  count++;
              }
          }
          cvtColor(mimg_fish2dist,rgb,CV_BGR2RGB);
          image = QImage((const unsigned char*)(rgb.data),
                      rgb.cols,rgb.rows,
                      rgb.cols*rgb.channels(),
                      QImage::Format_RGB888);



          scene->clear();
          scene->addPixmap(QPixmap::fromImage(image));
          ui->graphicsView->setScene(scene);
//          ui->graphicsView->adjustSize();
          on_actionCoordinate_calibration_triggered();

// update();
//   imshow("fish_distort",result_img);

//           MainWindow::on_actionCoordinate_calibration_triggered();
           //---------------------------------------------------------------
#if 1
 switch(fig_index)
 {
 case  1 :
 //---Up---------顺时针坐标--------------------
         ui->lineEdit->setText("0");
         ui->lineEdit_0->setText("0");
         ui->lineEdit_1->setText("420");
         ui->lineEdit_2->setText("0");

         ui->lineEdit_3->setText("300");
         ui->lineEdit_4->setText("165");

         ui->lineEdit_5->setText("120");
         ui->lineEdit_6->setText("165");
           break;
     case  2 :
         //---Down-----------------------------


         ui->lineEdit->setText("120");
         ui->lineEdit_0->setText("555");
         ui->lineEdit_1->setText("300");
         ui->lineEdit_2->setText("555");

         ui->lineEdit_3->setText("420");
         ui->lineEdit_4->setText("720");

         ui->lineEdit_5->setText("0");
         ui->lineEdit_6->setText("720");
           break;
     case  3 :
         //---Left-----------------------------
         ui->lineEdit->setText("120");
         ui->lineEdit_0->setText("165");
         ui->lineEdit_1->setText("120");
         ui->lineEdit_2->setText("555");

         ui->lineEdit_3->setText("0");
         ui->lineEdit_4->setText("720");

         ui->lineEdit_5->setText("0");
         ui->lineEdit_6->setText("0");
           break;
     case  4 :
         //---Right-----------------------------
         ui->lineEdit->setText("300");
         ui->lineEdit_0->setText("555");
         ui->lineEdit_1->setText("300");
         ui->lineEdit_2->setText("165");

         ui->lineEdit_3->setText("420");
         ui->lineEdit_4->setText("0");

         ui->lineEdit_5->setText("420");
         ui->lineEdit_6->setText("720");
     break;
    default :break;
 }
#endif


}

void MainWindow::on_v360_clicked()
{

    cv::resize(mimgx1,mimg,mimg.size(),  CV_INTER_CUBIC);
  imshow("sdfa",mimg);
}

void MainWindow::on_radioButton_1_clicked()
{

    fig_index = 1;
    fig_index_temp = 1;
    scene->clear();
    scene->addPixmap(QPixmap::fromImage(image1))->setPos(0,0);;
    ui->graphicsView->setScene(scene);
//    ui->graphicsView->adjustSize();
      on_actionCoordinate_calibration_triggered();
}

void MainWindow::on_radioButton_2_clicked()
{
    fig_index = 2;
    fig_index_temp = 2;
    scene->clear();
    scene->addPixmap(QPixmap::fromImage(image2))->setPos(0,0);;
    ui->graphicsView->setScene(scene);
//    ui->graphicsView->adjustSize();
     on_actionCoordinate_calibration_triggered();
}

void MainWindow::on_radioButton_3_clicked()
{
    fig_index = 3;
    fig_index_temp = 3;
    scene->clear();
    scene->addPixmap(QPixmap::fromImage(image3))->setPos(0,0);;
    ui->graphicsView->setScene(scene);
//    ui->graphicsView->adjustSize();
     on_actionCoordinate_calibration_triggered();
}

void MainWindow::on_radioButton_4_clicked()
{
    fig_index = 4;
    fig_index_temp = 4;
    scene->clear();
    scene->addPixmap(QPixmap::fromImage(image4))->setPos(0,0);;
    ui->graphicsView->setScene(scene);
//    ui->graphicsView->adjustSize();
      on_actionCoordinate_calibration_triggered();
}
