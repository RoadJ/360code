#ifndef DESCPICTURE_H
#define DESCPICTURE_H

#include <QWidget>
#include "opencv2/opencv.hpp"

namespace Ui {
class descPicture;
}

class descPicture : public QWidget
{
    Q_OBJECT

public:
    explicit descPicture(QWidget *parent = 0);
    ~descPicture();

public:
    void showPicture();
    void setSourcePicture(QString fileName);
    void setResultXml(QString fileName);
    void paintEvent(QPaintEvent *event);
    void setZoom(float Zoom);
    void setPictureWidth(int width);
    void setPictureHeight(int height);
    void cale();
    QImage cvMat2QImage(const cv::Mat& mat);

private slots:
    void on_pushButton_save_clicked();

private:
    QString sourcePictureName;
    QString resultXmlName;
    QImage sourceImage;
    QImage descImage;
    QImage * cutDescImage;

    cv::Mat intrinsic_matrix;    /*****    摄像机内参数矩阵    ****/
    cv::Mat distortion_coeffs;     /* 摄像机的4个畸变系数：k1,k2,k3,k4*/

    cv::Mat mapx;
    cv::Mat mapy;

    float zoom = 1.0;

    int pictureWidth;
    int pictureHeight;

private:
    Ui::descPicture *ui;
};

#endif // DESCPICTURE_H
