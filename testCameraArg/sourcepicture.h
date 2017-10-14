#ifndef SOURCEPICTURE_H
#define SOURCEPICTURE_H

#include <QWidget>

#include "public.h"

namespace Ui {
class sourcePicture;
}

class sourcePicture : public QWidget
{
    Q_OBJECT

public:
    explicit sourcePicture(QWidget *parent = 0);
    ~sourcePicture();
    void paintEvent(QPaintEvent *event);
    void setSourcePicture(QString fileName);
    void setPictureWidth(int width);
    void setPictureHeight(int height);
    void setOrientation(enum orientation);
    void swapWidthHeight();
    void cale();

private slots:
    void on_pushButton_save_clicked();

private:
    QString sourcePictureName;
    QImage sourceImage;
    QImage * cutSourceImage;
    enum orientation src_orientation;

private:
    Ui::sourcePicture *ui;
    QImage sourcePictureImage;
    int pictureWidth;
    int pictureHeight;
};

#endif // SOURCEPICTURE_H
