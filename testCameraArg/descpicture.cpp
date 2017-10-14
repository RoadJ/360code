#include "descpicture.h"
#include "ui_descpicture.h"
#include <QFileInfo>
#include <QMessageBox>
#include <QPainter>
#include "opencv2/opencv.hpp"
#include <QDebug>
#include <QFileDialog>

descPicture::descPicture(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::descPicture)
{
    ui->setupUi(this);
}

descPicture::~descPicture()
{
    delete ui;
}


void descPicture::setResultXml(QString fileName)
{
    resultXmlName = fileName;

    if (resultXmlName=="")
    {
        QMessageBox msgBox;
        msgBox.setText("空文件名");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return ;
    }

    QFileInfo fileInfo = QFileInfo(resultXmlName);

    if (fileInfo.exists() == false)
    {
        QMessageBox msgBox;
        msgBox.setText("无效目录或目录未进行计算");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return ;
    }
}

void descPicture::setSourcePicture(QString fileName)
{
    sourcePictureName = fileName;

    if (sourcePictureName=="")
    {
        QMessageBox msgBox;
        msgBox.setText("空文件名");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return ;
    }

    QFileInfo fileInfo = QFileInfo(sourcePictureName);

    if (fileInfo.exists() == false)
    {
        QMessageBox msgBox;
        msgBox.setText("无效文件名");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return ;
    }
}

void descPicture::setZoom(float Zoom)
{
    this->zoom = Zoom;
}


void descPicture::setPictureHeight(int height)
{
    this->pictureHeight = height;
}

void descPicture::setPictureWidth(int width)
{
    this->pictureWidth = width;
}

void descPicture::cale()
{
    std::string filename = resultXmlName.toStdString();
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

    cv::Mat rgb = cv::imread(sourcePictureName.toStdString());

    cv::Size image_size = rgb.size();
    this->mapx = cv::Mat(image_size ,CV_32FC1);
    this->mapy = cv::Mat(image_size ,CV_32FC1);
    cv::Mat R = cv::Mat::eye(3,3,CV_32F);

    cv::Mat new_intrinsic_mat;    //Mat new_intrinsic_mat(3, 3, CV_64FC1, Scalar(0))亦可，注意数据类型;


    //fx,fy变大（小），视场变小（大），裁剪较多（少），但细节清晰（模糊）；很关键，new_intrinsic_mat决定输出的畸变校正图像的范围
    intrinsic_matrix.copyTo(new_intrinsic_mat);

    float zoomDouble = zoom;

    //调整输出校正图的视场
    new_intrinsic_mat.at<double>(0, 0) *= zoomDouble;      //注意数据类型，非常重要
    new_intrinsic_mat.at<double>(1, 1) *= zoomDouble;

    //调整输出校正图的中心
    new_intrinsic_mat.at<double>(0, 2) += 0.5;
    new_intrinsic_mat.at<double>(1, 2) += 0.5;

    cv::fisheye::initUndistortRectifyMap(intrinsic_matrix,distortion_coeffs,R,new_intrinsic_mat,image_size ,CV_32FC1,mapx,mapy);

    cv::Mat descPicture;
    descPicture = rgb.clone();

    cv::remap(rgb,descPicture,mapx, mapy, cv::INTER_NEAREST);
    descImage = cvMat2QImage(descPicture);


    cutDescImage = new QImage(this->pictureWidth,this->pictureHeight,QImage::Format_RGB888);
    int tmpCounti = 0;
    int heightLimit = descImage.height() - pictureHeight;
    for(int i=0;i<descImage.height();i++)
    {
        int basePoint =  descImage.width()/2;


        if (i > heightLimit)
        {
            uchar * bitSourcePicture = descImage.scanLine(i);
            uchar * bitCutSourcePicture =  cutDescImage->scanLine(tmpCounti);
            int tmpCountj = 0;
            for(int j=0;j<descImage.width();j++)
            {
                if ( (j>(basePoint-(pictureWidth/2)))&& (j<(basePoint+(pictureWidth/2))))
                {
                    memcpy(&(bitCutSourcePicture[tmpCountj*3]),&(bitSourcePicture[j*3]),3);
                    tmpCountj++;
                }
            }

            tmpCounti++;
        }
    }

}

QImage descPicture::cvMat2QImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        qDebug() << "CV_8UC1";
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
        qDebug() << "CV_8UC3";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

void descPicture::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter;
    painter.begin(this);
    painter.drawImage(0,0,*cutDescImage);
    painter.end();
}

void descPicture::on_pushButton_save_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                "c:\\desc.jpg",
                                tr("Images (*.jpg)"));

    if (fileName == "")
    {
        QMessageBox msgBox;
        msgBox.setText("没有保存图片");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return ;
    }

    cutDescImage->save(fileName);
}
