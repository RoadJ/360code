#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "sourcepicture.h"
#include "descpicture.h"

#include <QFileDialog>
#include <QListWidget>
#include <QRadioButton>
#include <QDoubleValidator>

#include <QSettings>

#ifdef Q_OS_ANDROID
#include "g2d.h"
#include "g2d_yuyv_bgra.h"
#include "4_in_read.h"

static struct g2d_buf *g_g2d_src_buffer;
static struct g2d_buf *g_g2d_dst_buffer;
#endif
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
//    board_size = cv::Size(13,7);
    board_size = cv::Size(9,7);

//    board_size = cv::Size(9,6);
//    board_size = cv::Size(4,6);
    groupCount = 1;
    currentCount = 1;
    initApp();

    ui->listWidget_file->setViewMode(QListWidget::IconMode);
    ui->listWidget_file->setIconSize(QSize(200,200));
    ui->listWidget_file->setResizeMode(QListWidget::Adjust);


    ui->lineEdit_cutSrcPicture_width->setValidator(new QIntValidator(0, 1280, ui->lineEdit_cutSrcPicture_width));
    ui->lineEdit_cutSrcPicture_height->setValidator(new QIntValidator(0,1280,ui->lineEdit_cutSrcPicture_height));
    ui->lineEdit_cutDescPicture_width->setValidator(new QIntValidator(0,1280,ui->lineEdit_cutDescPicture_width));
    ui->lineEdit_cutDescPicture_height->setValidator(new QIntValidator(0,1280,ui->lineEdit_cutDescPicture_height));
    ui->lineEdit_zoom->setValidator(new QDoubleValidator(0.01,10.0,4,ui->lineEdit_zoom));

    src_orientation = top_orientation;

    //setCurrentFileName(groupCount,currentCount);
    //scanPicture();
    //createListResult();
}

void MainWindow::initApp()
{
    srcWidth = 720;
    srcheight = 480;

    descWidth = srcWidth;
    descHeight = srcheight;

    //this->showFullScreen();

    connect(ui->actionOpen,SIGNAL(triggered(bool)),this,SLOT(openFiles()));

    buffer = (unsigned char *) malloc(4*1024*1024);//建立jbarray数组

    if (buffer == NULL)
    {
        qDebug() << "can not malloc memory \n";
        exit(-1);
    }

#ifdef Q_OS_ANDROID
    int in_buffer_size = srcWidth * srcheight * 2;
    g_g2d_src_buffer = g2d_alloc(in_buffer_size, 0);

    if (g_g2d_src_buffer == 0)
    {
        qDebug() << "can not malloc memory for g2d source";
        free(buffer);
        exit(-1);
    }

    int out_buffer_size = srcWidth * srcheight *4;
    g_g2d_dst_buffer =g2d_alloc(out_buffer_size, 0);

    if (g_g2d_dst_buffer == 0)
    {
        qDebug() << "can not malloc memory for g2d dst";
        free(buffer);
        g2d_free(g_g2d_src_buffer);
        exit(-1);
    }

    memset(g_g2d_dst_buffer->buf_vaddr,0x0,srcWidth * srcheight *4);
#endif

    //timerCapture = new QTimer(this);
    //connect(timerCapture,SIGNAL(timeout()),this,SLOT(viewWindows()));
    //timerCapture->start(delayTime);
#if 0
    takePictureButton = new QPushButton(this);
    takePictureButton->setGeometry(720,50,150,80);
    takePictureButton->setText("take picture");
    takePictureButton->setEnabled(true);
    takePictureButton->show();

    connect(takePictureButton,SIGNAL(clicked(bool)),this,SLOT(takePicture()));

    caleButton = new QPushButton(this);
    caleButton->setGeometry(720,150,150,80);
    caleButton->setText("cale");
    caleButton->setEnabled(false);
    caleButton->show();

    connect(caleButton,SIGNAL(clicked(bool)),this,SLOT(calePicture()));

    deleteButton = new QPushButton(this);
    deleteButton->setGeometry(720,500,150,80);
    deleteButton->setText("delete");
    deleteButton->show();

    connect(deleteButton,SIGNAL(clicked(bool)),this,SLOT(deletePictureAndXml()));

    showGroupLab = new QLabel(this);
    showGroupLab->setGeometry(720,300,200,30);
    showGroupLab->show();

    showPictureLab = new QLabel(this);
    showPictureLab->setGeometry(720,250,200,30);
    showPictureLab->show();

    exitButton = new QPushButton(this);
    exitButton->setGeometry(100,480,180,80);
    exitButton->setText("exit");
    exitButton->show();

    connect(exitButton,SIGNAL(clicked(bool)),this,SLOT(CloseWindows()));

    distortion_coeffs_tableWidget = new QTableWidget(this);
    distortion_coeffs_tableWidget->setGeometry(900,100,400,200);
    distortion_coeffs_tableWidget->setColumnCount(4);
    distortion_coeffs_tableWidget->setRowCount(groupMax);
    distortion_coeffs_tableWidget->horizontalScrollBar()->setStyleSheet("QScrollBar{background:lightblue; height:50px; width:50px;}");
    distortion_coeffs_tableWidget->verticalScrollBar()->setStyleSheet("QScrollBar{background:lightblue; height:50px;width:50px;}");
    distortion_coeffs_tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:lightblue;color: black;padding-left: 10px;border: 1px solid #6c6c6c;}");

    distortion_coeffs_tableWidget->show();

    QStringList distortion_coeffs_headers;
    distortion_coeffs_headers << "K1" << "K2" << "K3" << "K4";
    distortion_coeffs_tableWidget->setHorizontalHeaderLabels(distortion_coeffs_headers);

    intrinsic_matrix_tableWidget = new QTableWidget(this);
    intrinsic_matrix_tableWidget->setGeometry(900,400,400,200);
    intrinsic_matrix_tableWidget->setColumnCount(9);
    intrinsic_matrix_tableWidget->setRowCount(groupMax);
    intrinsic_matrix_tableWidget->setColumnHidden(1,true);
    intrinsic_matrix_tableWidget->setColumnHidden(3,true);
    intrinsic_matrix_tableWidget->setColumnHidden(6,true);
    intrinsic_matrix_tableWidget->setColumnHidden(7,true);
    intrinsic_matrix_tableWidget->setColumnHidden(8,true);
    intrinsic_matrix_tableWidget->horizontalScrollBar()->setStyleSheet("QScrollBar{background:lightblue; height:50px; width:50px;}");
    intrinsic_matrix_tableWidget->verticalScrollBar()->setStyleSheet("QScrollBar{background:lightblue; height:50px;width:50px;}");
    intrinsic_matrix_tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:lightblue;color: black;padding-left: 10px;border: 1px solid #6c6c6c;}");
    intrinsic_matrix_tableWidget->show();

    QStringList intrinsic_matrix_headers;
    intrinsic_matrix_headers <<"fx"<<"(0,1)" << "cx" << "(1,0)" << "fy" << "cy" <<"(2,0)"<<"(2,1)"<<"(2,2)";
    intrinsic_matrix_tableWidget->setHorizontalHeaderLabels(intrinsic_matrix_headers);

#endif

#ifdef Q_OS_ANDROID
    cameraFourInThread = new CameraWorkFourin();
    cameraFourInThread->setCameraData(&cameraBuffer);
    cameraFourInThread->setWhichCamera(theCamera);
    connect(cameraFourInThread,SIGNAL(Frame(unsigned int)),this,SLOT(viewWindows(unsigned int)));
    cameraFourInThread->start();
#endif
}

void MainWindow::openFiles()
{
    QSettings settings("MySoft", "Star Runner");

    dir = settings.value("OpenFilePath").toString();

    if (dir =="")
    {
        dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"F:\\",QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    }
    else
    {
        dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),dir,QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    }

    if (dir == "")
    {
        QMessageBox msgBox;
        msgBox.setText("你没有选择文件夹！！！");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
    }

    settings.setValue("OpenFilePath",dir);


        filePath = dir.toStdString();


        QDir m_dir(dir);
        m_dir.setFilter(QDir::Files|QDir::Hidden|QDir::NoSymLinks);
        m_dir.setSorting(QDir::Size|QDir::Reversed);
        QFileInfoList list = m_dir.entryInfoList();

        ui->listWidget_file->clear();

        for (int i=0;i<list.size();++i)
        {

            QFileInfo fileInfo = list.at(i);

            if (fileInfo.suffix()=="jpg")
            {
                std::string filenameJPG  = filePath + "/" + fileInfo.fileName().toStdString();

                ui->listWidget_file->addItem(new QListWidgetItem(QIcon(dir + "/" + fileInfo.fileName()),fileInfo.fileName()));
            }
        }
//    ui->listView_file->addItem(new QListWidgetItem(QIcon("../earth.jpg"),"Earth"));
//    ui->listView_file->addItem(new QListWidgetItem(QIcon("../tornado.jpg"),"Tornado"));
//    ui->listView_file->addItem(new QListWidgetItem(QIcon("../ics.jpg"),"Wallpaper"));
}

void MainWindow::deletePictureAndXml()
{
    QMessageBox msgBox;
    msgBox.setText("do you want delete all picture and datat?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();

    switch (ret) {
       case QMessageBox::Yes:
           break;
       case QMessageBox::No:
           return;
           break;
       default:
           return;
           break;
     }

    deletePicture();
    deleteXml();
    deleteListResult();
    setCurrentFileName(1,1);
}

void MainWindow::scanPicture()
{
    for(int i=1;i<=groupMax;i++)
    {
        for(int j=1;j<=countMax;j++)
        {
            setCurrentFileName(i,j);
            QFile fileName(QString::fromStdString(getCurrentFileName()));
            if (!fileName.exists())
            {
                return;
            }
        }
    }
}

void MainWindow::deletePicture()
{
    for(int i=1;i<=groupMax;i++)
    {
        for(int j=1;j<=countMax;j++)
        {
            setCurrentFileName(i,j);
            QFile fileName(QString::fromStdString(getCurrentFileName()));
            if (fileName.exists())
            {
                fileName.remove();
            }
        }
    }
}

void MainWindow::deleteXml()
{
    for(int i=1;i<=groupMax;i++)
    {
        std::stringstream stringGroupCount;
        stringGroupCount <<i;

        QFile filename(QString::fromStdString( filePath + "/" + stringGroupCount.str() +"-" +"result.xml"));

        if (filename.exists())
        {
            filename.remove();
        }
    }
}

void MainWindow::createListResult()
{
    for(int i=1;i<=groupMax;i++)
    {
        std::stringstream stringGroupCount;
        stringGroupCount <<i;

        QFile filename(QString::fromStdString( filePath + "/" + stringGroupCount.str() +"-" +"result.xml"));

        if (filename.exists())
        {
            cv::FileStorage file_in(filePath + "/" + stringGroupCount.str() +"-" +"result.xml", cv::FileStorage::READ);

            cv::Mat intrinsic_matrix;
            file_in["intrinsic_matrix"] >>intrinsic_matrix;

            cv::Mat distortion_coeffs;
            file_in["distortion_coeffs"] >> distortion_coeffs;
            file_in.release();

            //
            for (int j=0;j<4;j++)
            {
                QString distortion_coeffs_string;
                distortion_coeffs_string = QString::number(distortion_coeffs.at<double>(j));
                distortion_coeffs_tableWidget->setItem(i-1, j, new QTableWidgetItem(distortion_coeffs_string));
            }

            //
            for (int j=0;j<3;j++)
            {
                for (int k=0;k<3;k++)
                {
                    QString intrinsic_matrix_string;
                    intrinsic_matrix_string = QString::number(intrinsic_matrix.at<double>(j,k));
                    intrinsic_matrix_tableWidget->setItem(i-1,j*3+k,new QTableWidgetItem(intrinsic_matrix_string));
                }
            }
        }
    }
}

void MainWindow::deleteListResult()
{
    for(int i=1;i<=groupMax;i++)
    {
            //
            for (int j=0;j<4;j++)
            {
                distortion_coeffs_tableWidget->setItem(i-1, j, new QTableWidgetItem(""));
            }

            //
            for (int j=0;j<3;j++)
            {
                for (int k=0;k<3;k++)
                {
                    intrinsic_matrix_tableWidget->setItem(i-1,j*3+k,new QTableWidgetItem(""));
                }
            }
        }
}

void MainWindow::takePicture()
{
    stopView();

    if (checkPicture() == true)
    {
        //pictrue is ok
        QToolTip::showText(QPoint(800,400),"ok !!! you can take next picture");
        savePicture();
        reGoupCountAndCount(true);
    }
    else
    {
        //pictrue is fail
        QToolTip::showText(QPoint(800,400),"fail");
        reGoupCountAndCount(false);
    }

    setCurrentFileName(groupCount,currentCount);
    startView();
}

bool MainWindow::checkPicture()
{
    //if all ok is return true

    /* 提取角点 */
    cv::Mat imageGray;
    cv::cvtColor(rgb, imageGray , CV_RGB2GRAY);
    std::vector<cv::Point2f> corners;                  /****    缓存每幅图像上检测到的角点       ****/

    bool patternfound = cv::findChessboardCorners(rgb, board_size, corners,cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE /*+ cv::CALIB_CB_FAST_CHECK*/ );

    if (!patternfound)
        return false;
    else
        return true;
}

void MainWindow::savePicture()
{
    std::string fileName;
    fileName = getCurrentFileName();
    cv::imwrite(fileName,rgb);
}

void MainWindow::stopView()
{
        //timerCapture->stop();
}

void MainWindow::startView()
{
        //timerCapture->start(delayTime);
}

void MainWindow::viewWindows(unsigned int frameOffset)
{
    LockMutex.lock();
#ifdef Q_OS_ANDROID
        //getTopCameraShareMemory(buffer,2*srcWidth*srcheight);
        memcpy(buffer,((uchar*)(cameraFourInThread->startData) + ((frameOffset) *countCamera * 2 *srcWidth *srcheight) + (1 * 2 *srcWidth *srcheight)),2*srcWidth*srcheight);

        if (buffer == NULL)
        {
            qDebug() << "buffer is empty";
            //timerCapture->start(delayTime);
            return ;
        }

        memcpy(g_g2d_src_buffer->buf_vaddr,buffer,2*srcWidth*srcheight);

        YUYV2BGRA(g_g2d_src_buffer,srcWidth,srcheight,g_g2d_dst_buffer,srcWidth,srcheight,0);
         rgb = cv::Mat(srcheight, srcWidth,CV_8UC4,g_g2d_dst_buffer->buf_vaddr );
#endif

#ifdef Q_OS_WIN

#endif


        if (rgb.empty() == true)
        {
            qDebug() << "frame is empty";
            LockMutex.unlock();
            return ;
        }
        pictureImage = cvMat2QImage(rgb);
        update();

    LockMutex.unlock();
}

QImage MainWindow::cvMat2QImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        //qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        //qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

void MainWindow::reGoupCountAndCount(bool ok)
{
    if (ok != true)
    {
        return ;
    }
    else
    {
          currentCount += 1;

          if (currentCount > countMax)
         {
              currentCount = 1;
              groupCount += 1;
              caleButton->setEnabled(true);
              takePictureButton->setEnabled(false);
          }

          if (groupCount > groupMax)
          {
            QToolTip::showText(QPoint(800,400),"all is ok");
            takePictureButton->setEnabled(false);
            caleButton->setEnabled(true);
          }
    }
}

void MainWindow::calePicture()
{

    LockMutex.lock();

    QToolTip::showText(QPoint(800,400),"start cale.");

    if (groupCount > 1)
    {
        int groupTemp = groupCount - 1;
        int countTemp = 1;

        std::vector<cv::Point2f> corners;                  /****    缓存每幅图像上检测到的角点       ****/
        std::vector<std::vector<cv::Point2f> >  corners_Seq;    /****  保存检测到的所有角点       ****/
        std::vector<cv::Mat>  image_Seq;
        int successImageNum = 0;				/****	成功提取角点的棋盘图数量	****/
        int count = 0;

        for (int i=countTemp;i<=countMax;i++)
        {
            std::stringstream stringCurrentCount;
            stringCurrentCount <<i;

            std::stringstream stringGroupCount;
            stringGroupCount<<groupTemp;

            std::string filenameJPG  = filePath + "/" +stringGroupCount.str() + "-" + stringCurrentCount.str() + ".jpg";

            cv::Mat image_temp =cv::imread(filenameJPG);

            cv::Mat imageGray;
            cv::cvtColor(image_temp, imageGray , CV_RGB2GRAY);
            bool patternfound = cv::findChessboardCorners(image_temp, board_size, corners,cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE /*+ cv::CALIB_CB_FAST_CHECK*/ );

            if (!patternfound)
            {
                continue;
            }

            /* 亚像素精确化 */
            cv::cornerSubPix(imageGray, corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

            count = count + corners.size();
            successImageNum = successImageNum + 1;
            corners_Seq.push_back(corners);

            image_Seq.push_back(image_temp);
        }


        qDebug()<<"开始定标………………";

        std::vector < std::vector<cv::Point3f> >  object_Points;        /****  保存定标板上角点的三维坐标   ****/

    #if 0
        cv::Mat image_points = cv::Mat(1, count, CV_32FC2, cv::Scalar::all(0));  /*****   保存提取的所有角点   *****/
    #endif

        std::vector<int>  point_counts;

        object_Points.clear();


        /* 初始化定标板上角点的三维坐标 */

        for (int t = 0; t<successImageNum; t++)
        {
     #if 0
            std::vector<cv::Point3f> tempPointSet;
            cv::Size square_size = cv::Size(20,20);

            for (int i = 0; i<board_size.height; i++)
            {
                for (int j = 0; j<board_size.width; j++)
                {
                    /* 假设定标板放在世界坐标系中z=0的平面上 */
                    cv::Point3f tempPoint;
                    tempPoint.x = i*square_size.width;
                    tempPoint.y = j*square_size.height;
                    tempPoint.z = 0;
                    tempPointSet.push_back(tempPoint);
                }
            }
            object_Points.push_back(tempPointSet);
      #else
            object_Points.push_back(Create3DChessboardCorners(board_size,32));
      #endif
        }

        qDebug() << "init object_Points insert ok" ;

        for (int i = 0; i< successImageNum; i++)
        {
            point_counts.push_back(board_size.width*board_size.height);
        }

        qDebug() << "int point_counts insert ok" ;

        /* 开始定标 */

        if ( image_Seq.empty())
        {
             qDebug() << "标定失败111111！！";
             caleButton->setEnabled(false);
             takePictureButton->setEnabled(true);
             LockMutex.unlock();
             return ;
        }

        if ( object_Points.size() != corners_Seq.size() )
        {
            qDebug() << "标定失败222222222！！";
        }

        qDebug() << "object_Points.size() = " << object_Points.size();
        qDebug() << "corners_Seq.size() =" << corners_Seq.size() ;

        cv::Size image_size = image_Seq[0].size();
        cv::Mat intrinsic_matrix;    /*****    摄像机内参数矩阵    ****/
        cv::Mat distortion_coeffs;     /* 摄像机的4个畸变系数：k1,k2,k3,k4*/

        std::vector<cv::Vec3d> rotation_vectors;                           /* 每幅图像的旋转向量 */
        std::vector<cv::Vec3d> translation_vectors;                        /* 每幅图像的平移向量 */

        int flags = 0;

    #if 0
        flags |= cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC;
    #endif
        flags |= cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC;
        flags |= cv::fisheye::CALIB_CHECK_COND;
        flags |= cv::fisheye::CALIB_FIX_SKEW;

        if( ( successImageNum >= 2) )
        {
            QMessageBox msgBox;
            msgBox.setText(QString::number(successImageNum));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();

            qDebug() << "定标???????";
            cv::fisheye::calibrate(object_Points, corners_Seq, image_size, intrinsic_matrix, distortion_coeffs, rotation_vectors, translation_vectors, flags,cv::TermCriteria(3, 20, 1e-6));
            qDebug() << "定标完成！";

            std::stringstream stringGroupCount;
            stringGroupCount<<groupTemp;

            std::string filenameXml  = filePath + "/" +stringGroupCount.str() + "-";
            filenameXml += "result.xml";

            cv::FileStorage file_out(filenameXml, cv::FileStorage::WRITE);
            file_out << "intrinsic_matrix" <<cv::Mat(intrinsic_matrix);
            file_out << "distortion_coeffs" <<cv::Mat(distortion_coeffs);
            file_out.release();

            QToolTip::showText(QPoint(800,400),"The data is save.");
         }
    }

    createListResult();
    caleButton->setEnabled(false);
    takePictureButton->setEnabled(true);
    LockMutex.unlock();
}

std::string MainWindow::getCurrentFileName()
{
    return currentFileName;
}

void MainWindow::setCurrentFileName(int groupCount,int currentCount)
{
    this->groupCount = groupCount;
    this->currentCount = currentCount;

    std::stringstream stringCurrentCount;
    stringCurrentCount <<currentCount;

    std::stringstream stringGroupCount;
    stringGroupCount<<groupCount;

    this->currentFileName = filePath + "/" +stringGroupCount.str() + "-" + stringCurrentCount.str() + ".jpg";

    QString StringGroupLab;
    StringGroupLab = "group:" + QString::number(groupCount) + "-" + QString::number(groupMax);
    showGroupLab->setFont(QFont(StringGroupLab,24,24));
    showGroupLab->setText(StringGroupLab);

    QString StringPictureLab;
    StringPictureLab = "count:" + QString::number(currentCount) + "-" + QString::number(countMax);
    showPictureLab->setFont(QFont(StringPictureLab,24,24));
    showPictureLab->setText(StringPictureLab);
}

std::vector<cv::Point3f> MainWindow::Create3DChessboardCorners(cv::Size boardSize, float squareSize)
{
  // This function creates the 3D points of your chessboard in its own coordinate system
float width = (boardSize.width-1)*squareSize;
float height = (boardSize.height-1)*squareSize;


  std::vector<cv::Point3f> corners;

  for( int i = 0; i < boardSize.height; i++ )
  {
    for( int j = 0; j < boardSize.width; j++ )
    {
      corners.push_back(cv::Point3f(float(j*squareSize)-width, float(i*squareSize)-height, 0));
    }
  }

  return corners;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter;
    painter.begin(this);
    painter.drawImage(0,0,pictureImage);
    painter.end();
}

void MainWindow::CloseWindows()
{
    this->close();
    exit(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_cale_clicked()
{
    if (dir == "")
    {
        QMessageBox msgBox;
        msgBox.setText("你没有选择文件夹！！！");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return ;
    }

    filePath = dir.toStdString();



    QToolTip::showText(QPoint(800,400),"start cale.");

    //if (groupCount > 1)
    {
        //int groupTemp = groupCount - 1;
        //int countTemp = 1;

        std::vector<cv::Point2f> corners;                  /****    缓存每幅图像上检测到的角点       ****/
        std::vector<std::vector<cv::Point2f> >  corners_Seq;    /****  保存检测到的所有角点       ****/
        std::vector<cv::Mat>  image_Seq;
        int successImageNum = 0;				/****	成功提取角点的棋盘图数量	****/
        int count = 0;

        QDir m_dir(dir);
        m_dir.setFilter(QDir::Files|QDir::Hidden|QDir::NoSymLinks);
        m_dir.setSorting(QDir::Size|QDir::Reversed);
        QFileInfoList list = m_dir.entryInfoList();

        for (int i=0;i<list.size();++i)
        {

            QFileInfo fileInfo = list.at(i);
//                qDebug() << qPrintable(QString("%1 %2").arg(fileInfo.size(), 10)
//                                                        .arg(fileInfo.fileName()));
//                std::cout << std::endl;


            if (fileInfo.suffix()=="jpg")
            {

                std::string filenameJPG  = filePath + "/" + fileInfo.fileName().toStdString();

                cv::Mat image_temp =cv::imread(filenameJPG);

                cv::Mat imageGray;
                cv::cvtColor(image_temp, imageGray , CV_RGB2GRAY);
                bool patternfound = cv::findChessboardCorners(image_temp, board_size, corners/*,cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK*/ );

                if (!patternfound)
                {
                    continue;
                }

                /* 亚像素精确化 */
                cv::cornerSubPix(imageGray, corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

                count = count + corners.size();

                successImageNum = successImageNum + 1;
                corners_Seq.push_back(corners);

                image_Seq.push_back(image_temp);
            }
        }


        qDebug()<<"开始定标………………";

        std::vector < std::vector<cv::Point3f> >  object_Points;        /****  保存定标板上角点的三维坐标   ****/

    #if 0
        cv::Mat image_points = cv::Mat(1, count, CV_32FC2, cv::Scalar::all(0));  /*****   保存提取的所有角点   *****/
    #endif

        std::vector<int>  point_counts;

        object_Points.clear();


        /* 初始化定标板上角点的三维坐标 */

        for (int t = 0; t<successImageNum; t++)
        {
     #if 0
            std::vector<cv::Point3f> tempPointSet;
            cv::Size square_size = cv::Size(20,20);

            for (int i = 0; i<board_size.height; i++)
            {
                for (int j = 0; j<board_size.width; j++)
                {
                    /* 假设定标板放在世界坐标系中z=0的平面上 */
                    cv::Point3f tempPoint;
                    tempPoint.x = i*square_size.width;
                    tempPoint.y = j*square_size.height;
                    tempPoint.z = 0;
                    tempPointSet.push_back(tempPoint);
                }
            }
            object_Points.push_back(tempPointSet);
      #else
            object_Points.push_back(Create3DChessboardCorners(board_size,32));
      #endif
        }

        qDebug() << "init object_Points insert ok" ;

        for (int i = 0; i< successImageNum; i++)
        {
            point_counts.push_back(board_size.width*board_size.height);
        }

        qDebug() << "int point_counts insert ok" ;

        /* 开始定标 */

        if ( image_Seq.empty())
        {
             qDebug() << "标定失败111111！！";
             //caleButton->setEnabled(false);
             //takePictureButton->setEnabled(true);
             //LockMutex.unlock();
             QMessageBox msgBox;
             msgBox.setText("标定失败111111");
             msgBox.setStandardButtons(QMessageBox::Ok);
             msgBox.setDefaultButton(QMessageBox::Ok);
             msgBox.exec();

             return ;
        }

        if ( object_Points.size() != corners_Seq.size() )
        {
            qDebug() << "标定失败222222222！！";
            QMessageBox msgBox;
            msgBox.setText("标定失败222222222！！");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();
            return;
        }

        qDebug() << "object_Points.size() = " << object_Points.size();
        qDebug() << "corners_Seq.size() =" << corners_Seq.size() ;

        cv::Size image_size = image_Seq[0].size();
        cv::Mat intrinsic_matrix;    /*****    摄像机内参数矩阵    ****/
        cv::Mat distortion_coeffs;     /* 摄像机的4个畸变系数：k1,k2,k3,k4*/

        std::vector<cv::Vec3d> rotation_vectors;                           /* 每幅图像的旋转向量 */
        std::vector<cv::Vec3d> translation_vectors;                        /* 每幅图像的平移向量 */

        int flags = 0;

    #if 0
        flags |= cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC;
    #endif
        flags |= cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC;
        flags |= cv::fisheye::CALIB_CHECK_COND;
        flags |= cv::fisheye::CALIB_FIX_SKEW;

        if( ( successImageNum >= 2) )
        {

            QMessageBox msgBox;
            msgBox.setText(QString::number(successImageNum,10));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();

            qDebug() << "定标???????";
            cv::fisheye::calibrate(object_Points, corners_Seq, image_size, intrinsic_matrix, distortion_coeffs, rotation_vectors, translation_vectors, flags,cv::TermCriteria(3, 20, 1e-6));
            qDebug() << "定标完成！";

            std::string filenameXml  = filePath + "/";
            filenameXml += "result.xml";

            cv::FileStorage file_out(filenameXml, cv::FileStorage::WRITE);
            file_out << "intrinsic_matrix" <<cv::Mat(intrinsic_matrix);
            file_out << "distortion_coeffs" <<cv::Mat(distortion_coeffs);
            file_out.release();

            QToolTip::showText(QPoint(800,400),"The data is save.");
         }
    }

    //createListResult();
//    caleButton->setEnabled(false);
//    takePictureButton->setEnabled(true);
//    LockMutex.unlock();
}

void MainWindow::on_pushButton_watch_clicked()
{
    if (dir=="")
    {
        QMessageBox msgBox;
        msgBox.setText("没有选中目录");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return ;
    }

//    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
//                                                      dir,
//                                                      tr("Images (*.jpg)"));

//    if (fileName == "")
//    {
//        QMessageBox msgBox;
//        msgBox.setText("没有选中图片");
//        msgBox.setStandardButtons(QMessageBox::Ok);
//        msgBox.setDefaultButton(QMessageBox::Ok);
//        msgBox.exec();
//        return ;
//    }

    if (ui->listWidget_file->selectedItems().isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText("没有选中图像文件");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return ;
    }

    QString fileName = ui->listWidget_file->selectedItems().constFirst()->text();

    if (fileName=="")
    {
        QMessageBox msgBox;
        msgBox.setText("没有选中图片");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return ;
    }

    fileName = dir + "/" +fileName;

    if (ui->checkBox_srcPicture->checkState() == Qt::Checked)
    {
        if (ui->radioButton_noCutSrcPicture->isChecked())
        {
            sourcePictureUI = new sourcePicture();
            sourcePictureUI->setSourcePicture(fileName);
            sourcePictureUI->setPictureWidth(1280);
            sourcePictureUI->setPictureHeight(800);
            sourcePictureUI->setOrientation(src_orientation);
            sourcePictureUI->cale();
            sourcePictureUI->show();
        }
        else
        {
            sourcePictureUI = new sourcePicture();
            sourcePictureUI->setSourcePicture(fileName);
            sourcePictureUI->setPictureWidth(ui->lineEdit_cutSrcPicture_width->text().toInt());
            sourcePictureUI->setPictureHeight(ui->lineEdit_cutSrcPicture_height->text().toInt());
            sourcePictureUI->setOrientation(src_orientation);
            sourcePictureUI->cale();
            sourcePictureUI->show();
        }
    }

    if (ui->checkBox_descPicture->checkState() == Qt::Checked)
    {
        if (ui->radioButton_noCutDescPicture->isChecked())
        {
            descPictureUI = new descPicture();
            descPictureUI->setSourcePicture(fileName);
            descPictureUI->setResultXml(dir+"/"+"result.xml");
            descPictureUI->setZoom(ui->lineEdit_zoom->text().toFloat());
            descPictureUI->setPictureWidth(1280);
            descPictureUI->setPictureHeight(800);
            descPictureUI->cale();
            descPictureUI->show();
        }
        else
        {
            descPictureUI = new descPicture();
            descPictureUI->setSourcePicture(fileName);
            descPictureUI->setResultXml(dir+"/"+"result.xml");
            descPictureUI->setZoom(ui->lineEdit_zoom->text().toFloat());
            descPictureUI->setPictureWidth(ui->lineEdit_cutDescPicture_width->text().toInt());
            descPictureUI->setPictureHeight(ui->lineEdit_cutDescPicture_height->text().toInt());
            descPictureUI->cale();
            descPictureUI->show();
        }
    }
}

void MainWindow::on_radioButton_top_clicked()
{
    src_orientation = top_orientation;

//    ui->lineEdit_cutSrcPicture_width->setValidator(new QIntValidator(0, 1280, ui->lineEdit_cutSrcPicture_width));
//    ui->lineEdit_cutSrcPicture_height->setValidator(new QIntValidator(0,800,ui->lineEdit_cutSrcPicture_height));

}

void MainWindow::on_radioButton_left_clicked()
{
    src_orientation = left_orientation;

//    ui->lineEdit_cutSrcPicture_width->setValidator(new QIntValidator(0, 800, ui->lineEdit_cutSrcPicture_width));
//    ui->lineEdit_cutSrcPicture_height->setValidator(new QIntValidator(0,1280,ui->lineEdit_cutSrcPicture_height));

}

void MainWindow::on_radioButton_bottom_clicked()
{
    src_orientation = bottom_orientation;

//    ui->lineEdit_cutSrcPicture_width->setValidator(new QIntValidator(0, 1280, ui->lineEdit_cutSrcPicture_width));
//    ui->lineEdit_cutSrcPicture_height->setValidator(new QIntValidator(0,800,ui->lineEdit_cutSrcPicture_height));

}

void MainWindow::on_radioButton_right_clicked()
{
    src_orientation = right_orientation;

//    ui->lineEdit_cutSrcPicture_width->setValidator(new QIntValidator(0, 800, ui->lineEdit_cutSrcPicture_width));
//    ui->lineEdit_cutSrcPicture_height->setValidator(new QIntValidator(0,1280,ui->lineEdit_cutSrcPicture_height));

}

void MainWindow::on_actionOpen_triggered()
{

}
