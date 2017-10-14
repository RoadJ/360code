#ifndef G2D_THREAD_H
#define G2D_THREAD_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QImage>
#include <QTimer>
#include <QPixmap>
#include "g2d_yuyv_bgra.h"
#include "g2d.h"
#include "v4l2_work_fourin.h"

//extern QImage * finishImage;

class G2DThread : public QThread
{
    Q_OBJECT

public:
    G2DThread(QObject *parent = Q_NULLPTR);
    ~G2DThread();

    void setWhichCamera(int whichCamera);

signals:void caleFinish(unsigned int);

public slots:
    void copyData(unsigned int frameOffset);
    void cleanFrameSpeed();

public:
    static const int bufferCount = 4;
    QImage * finishImage;
    QImage * stitchingImage[bufferCount];
    QPixmap fisheyePixmap;

private:
    QMutex mutex;
    CameraWorkFourin *cameraFourInThread;
    unsigned char * cameraBuffer;
    int srcWidth = 1280;
    int srcheight = 800;

    int imageWidth = 420;
    int imageHeight = 720;
    const unsigned int countCamera = 4;
    struct g2d_buf *g_g2d_src_cale_buffer;
    struct g2d_buf *g_g2d_desc_cale_buffer;

    int countFrameSpeed = 0;

    //QImage * stitchingImage[bufferCount];
    //QImage * finishImage;

    bool status =false;

    QTimer *timer;

    int whichCamera = 0;
};

#endif // G2D_THREAD_H
