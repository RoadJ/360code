#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QPainter>
#include <QImage>
#include <QMessageBox>
#include <string>
#include <QTimer>
#include <QDebug>
#include <QMutex>
#include <QToolTip>
#include <QLabel>
#include <QComboBox>
#include <QFile>
#include <QTableWidget>
#include <QHeaderView>
#include <QScrollBar>
#include "opencv2/opencv.hpp"
#include <QThread>

#ifdef Q_OS_ANDROID
#include "v4l2_work_fourin.h"
#endif

#include "sourcepicture.h"
#include "descpicture.h"

#include "public.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void paintEvent(QPaintEvent *event);

public:
    std::vector<cv::Point3f> Create3DChessboardCorners(cv::Size boardSize, float squareSize);

private slots:
    void takePicture();
    void stopView();
    void startView();
    void viewWindows(unsigned int frameOffset);
    void calePicture();
    void createListResult();
    void scanPicture();
    void deletePictureAndXml();
    void CloseWindows();
    void openFiles();

    void on_pushButton_cale_clicked();

    void on_pushButton_watch_clicked();

    void on_radioButton_top_clicked();

    void on_radioButton_left_clicked();

    void on_radioButton_bottom_clicked();

    void on_radioButton_right_clicked();

    void on_actionOpen_triggered();

private:
    bool checkPicture();
    void savePicture();
    std::string getCurrentFileName();
    void setCurrentFileName(int groupFile,int currentCount);
    void reGoupCountAndCount(bool ok);
    void initApp();
    QImage cvMat2QImage(const cv::Mat& mat);
    void deleteListResult();
    void deletePicture();
    void deleteXml();

private:
    QLabel *showGroupLab;
    QLabel *showPictureLab;

    QTimer * timerCapture;
    QComboBox *listResult;

    QPushButton *takePictureButton;
    QPushButton *caleButton;
    QPushButton *deleteButton;
    QPushButton *exitButton;

    QTableWidget *distortion_coeffs_tableWidget;
    QTableWidget *intrinsic_matrix_tableWidget;

    QImage pictureImage;
    std::string currentFileName;
    std::string filePath = "/data/user/img";
    QString dir;
    cv::Size board_size = cv::Size(9,6);
    int groupCount;
    const int groupMax = 20;
    const int countMax = 999;
    int currentCount;
    int srcWidth = 720;
    int srcheight = 480;
    int descWidth;
    int descHeight;
    cv::Mat rgb;
    unsigned char * buffer;
    const int delayTime = 33;
    QMutex LockMutex;

    int theCamera = 0;    

#ifdef Q_OS_ANDROID
    CameraWorkFourin *cameraFourInThread;
#endif
    unsigned char * cameraBuffer;
    const unsigned int countCamera = 4;

    sourcePicture * sourcePictureUI;
    descPicture * descPictureUI;
    enum orientation src_orientation;

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
