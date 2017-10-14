#include "sourcepicture.h"
#include "ui_sourcepicture.h"
#include <QMessageBox>
#include <QFileInfo>
#include <QPainter>
#include <QFileDialog>

sourcePicture::sourcePicture(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::sourcePicture)
{
    ui->setupUi(this);
}

void sourcePicture::setSourcePicture(QString fileName)
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

    sourcePictureImage = QImage(sourcePictureName);
}

void sourcePicture::setPictureHeight(int height)
{
    this->pictureHeight = height;
}

void sourcePicture::setPictureWidth(int width)
{
    this->pictureWidth = width;
}

void sourcePicture::swapWidthHeight()
{
    int tmp_swap_value = this->pictureWidth;
    setPictureWidth(this->pictureHeight);
    setPictureHeight(tmp_swap_value);
}

void sourcePicture::setOrientation(enum orientation src_orientation)
{
   this->src_orientation = src_orientation;

   if (src_orientation == left_orientation)
   {
//        swapWidthHeight();
   }

   if (src_orientation == right_orientation)
   {
//       swapWidthHeight();
   }
}

void sourcePicture::cale()
{

    //朝向向前
    if (src_orientation == top_orientation)
    {

        cutSourceImage = new QImage(this->pictureWidth,this->pictureHeight,QImage::Format_ARGB32);
        int tmpCounti = 0;
        int heightLimit = sourcePictureImage.height() - pictureHeight;
        for(int i=0;i<sourcePictureImage.height();i++)
        {
            int basePoint =  sourcePictureImage.width()/2;


            if (i > heightLimit)
            {
                uchar * bitSourcePicture = sourcePictureImage.scanLine(i);
                uchar * bitCutSourcePicture =  cutSourceImage->scanLine(tmpCounti);
                int tmpCountj = 0;
                for(int j=0;j<sourcePictureImage.width();j++)
                {
                    if ( (j>(basePoint-(pictureWidth/2)))&& (j<(basePoint+(pictureWidth/2))))
                    {
                        memcpy(&(bitCutSourcePicture[tmpCountj*4]),&(bitSourcePicture[j*4]),4);
                        tmpCountj++;
                    }
                }

                tmpCounti++;
            }
        }
    }

    //朝向向后
    if (src_orientation == bottom_orientation)
    {
        QImage* imgRatate = new QImage;
        QMatrix matrix;
        matrix.rotate(180);
        *imgRatate = sourcePictureImage.transformed(matrix);

        cutSourceImage = new QImage(this->pictureWidth,this->pictureHeight,QImage::Format_ARGB32);
        int tmpCounti = 0;
//        int heightLimit = imgRatate->height() - pictureHeight;
        for(int i=0;i<imgRatate->height();i++)
        {
            int basePoint =  imgRatate->width()/2;

            if (i < pictureHeight)
            {
                uchar * bitSourcePicture = imgRatate->scanLine(i);
                uchar * bitCutSourcePicture =  cutSourceImage->scanLine(tmpCounti);
                int tmpCountj = 0;
                for(int j=0;j<imgRatate->width();j++)
                {
                    if ( (j>(basePoint-(pictureWidth/2)))&& (j<(basePoint+(pictureWidth/2))))
                    {
                        memcpy(&(bitCutSourcePicture[tmpCountj*4]),&(bitSourcePicture[j*4]),4);
                        tmpCountj++;
                    }
                }

                tmpCounti++;
            }
        }
    }

    //朝向向左
    if (src_orientation == left_orientation)
    {
        QImage* imgRatate = new QImage;
        QMatrix matrix;
        matrix.rotate(270);
        *imgRatate = sourcePictureImage.transformed(matrix);

        cutSourceImage = new QImage(this->pictureWidth,this->pictureHeight,QImage::Format_ARGB32);
        int tmpCounti = 0;
        int heightLimit = imgRatate->width() - pictureWidth;
        int basePoint =  imgRatate->height()/2;
        for(int i=0;i<imgRatate->height();i++)
        {
//            int basePoint =  imgRatate->width()/2;

            if ( (i > (basePoint - (pictureHeight/2)))&& (i < (basePoint + (pictureHeight/2))) )
            {
                uchar * bitSourcePicture = imgRatate->scanLine(i);
                uchar * bitCutSourcePicture =  cutSourceImage->scanLine(tmpCounti);
                int tmpCountj = 0;
                for(int j=0;j<imgRatate->width();j++)
                {
                    if ( j> heightLimit)
                    {
                        memcpy(&(bitCutSourcePicture[tmpCountj*4]),&(bitSourcePicture[j*4]),4);
                        tmpCountj++;
                    }
                }

                tmpCounti++;
            }
        }
    }

    //朝向向右
    if (src_orientation == right_orientation)
    {
        QImage* imgRatate = new QImage;
        QMatrix matrix;
        matrix.rotate(90);
        *imgRatate = sourcePictureImage.transformed(matrix);

        cutSourceImage = new QImage(this->pictureWidth,this->pictureHeight,QImage::Format_ARGB32);
        int tmpCounti = 0;
        int heightLimit = imgRatate->width() - pictureWidth;
        int basePoint =  imgRatate->height()/2;
        for(int i=0;i<imgRatate->height();i++)
        {
//            int basePoint =  imgRatate->width()/2;

            if ( (i > (basePoint - (pictureHeight/2)))&& (i < (basePoint + (pictureHeight/2))) )
            {
                uchar * bitSourcePicture = imgRatate->scanLine(i);
                uchar * bitCutSourcePicture =  cutSourceImage->scanLine(tmpCounti);
                int tmpCountj = 0;
                for(int j=0;j<imgRatate->width();j++)
                {
                    if ( j< pictureWidth)
                    {
                        memcpy(&(bitCutSourcePicture[tmpCountj*4]),&(bitSourcePicture[j*4]),4);
                        tmpCountj++;
                    }
                }

                tmpCounti++;
            }
        }
    }
}

void sourcePicture::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter;
    painter.begin(this);
    painter.drawImage(0,0,*cutSourceImage);
    painter.end();
}

sourcePicture::~sourcePicture()
{
    delete ui;
}

void sourcePicture::on_pushButton_save_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                "c:\\source.jpg",
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

    cutSourceImage->save(fileName);

}
