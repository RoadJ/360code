#ifndef V4L2_WORK_FOURIN_H
#define V4L2_WORK_FOURIN_H

#include <QThread>
#include <QString>
#include  <QObject>
//#include "g2d_yuyv_bgra.h"
//#include "g2d.h"

class CameraWorkFourin : public QThread
{
    Q_OBJECT

    struct buffer {
        void * start;
        size_t length;
    };

    typedef struct __CameraStruct
    {
        char dev_name[20];
        int fd;
        struct buffer * buf;
        unsigned int n_buffers;
        unsigned char * data;
    }CameraStruct,* PCameraStruct;

    typedef struct __FourCameraStruct
    {
        CameraStruct topCamera;
        CameraStruct bottomCamera;
        CameraStruct leftCamera;
        CameraStruct rightCamera;
    }FourCameraStruct, *PFourCameraStruct;

public:
    CameraWorkFourin(QObject *parent = Q_NULLPTR);
    ~CameraWorkFourin();

    void setCameraName(CameraStruct *camera,QString name);
    void setWhichCamera(int theCamera);
    unsigned char * setCameraData(unsigned char **data);

private:
    void init(CameraStruct *camera);
    void uninit(CameraStruct *camera);
    void cameraRun(CameraStruct *camera,int theCamera);
    int read_frame(CameraStruct *camera,int theCamera);

private:
    void run();

signals: void Frame(unsigned int);

public:
    static const int bufferCount = 2;
    //struct g2d_buf *g_g2d_src_buffer[bufferCount];
    //struct g2d_buf *g_g2d_dst_buffer[bufferCount];
    unsigned char * startData = NULL;

private:

    QString cameraName;
    FourCameraStruct fourCamera;
    const bool analog = true;
    const unsigned int count = 2;
    const unsigned int width = 720; //宽，必须是16的倍数
    const unsigned int height = 480; //高，必须是16的倍数
    unsigned int offsetCount = 0;
    const unsigned int countCamera = 4;
    int theCamera;
};

#endif // V4L2_WORK_FOURIN_H
