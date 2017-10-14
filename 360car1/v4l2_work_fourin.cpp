/*
 * main.cpp
 *
 *  Created on: 2016年9月5日
 *      Author: hh
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <getopt.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>
#include <linux/videodev2.h>
#include <pthread.h>

#include <errno.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>

#include "v4l2_work_fourin.h"
#include <QDebug>
#include <QTime>

#define CLEAR(x) memset (&(x), 0, sizeof (x))

#define topCameraWhich 0
#define bottomCameraWhich 1
#define leftCameraWhich 2
#define rightCameraWhich 3

//////////////////////////////////////////////////////
CameraWorkFourin::CameraWorkFourin(QObject *parent)
{
    memset(fourCamera.topCamera.dev_name, '\0', sizeof(fourCamera.topCamera.dev_name));
    fourCamera.topCamera.buf[0] = NULL;
    fourCamera.topCamera.buf[1] = NULL;
    fourCamera.topCamera.buf[2] = NULL;
    fourCamera.topCamera.buf[3] = NULL;
    fourCamera.topCamera.data = NULL;
    fourCamera.topCamera.fd = -1;
    fourCamera.topCamera.n_buffers = 0;
    setCameraName(&(fourCamera.topCamera),"/dev/video0");
    init(&(fourCamera.topCamera),topCameraWhich);

#if 1
    memset(fourCamera.bottomCamera.dev_name, '\0', sizeof(fourCamera.bottomCamera.dev_name));
    fourCamera.bottomCamera.buf[0] = NULL;
    fourCamera.bottomCamera.buf[1] = NULL;
    fourCamera.bottomCamera.buf[2] = NULL;
    fourCamera.bottomCamera.buf[3] = NULL;
    fourCamera.bottomCamera.data = NULL;
    fourCamera.bottomCamera.fd = -1;
    fourCamera.bottomCamera.n_buffers = 0;
    setCameraName(&(fourCamera.bottomCamera),"/dev/video1");
    init(&(fourCamera.bottomCamera),bottomCameraWhich);


    memset(fourCamera.leftCamera.dev_name, '\0', sizeof(fourCamera.leftCamera.dev_name));
    fourCamera.leftCamera.buf[0] = NULL;
    fourCamera.leftCamera.buf[1] = NULL;
    fourCamera.leftCamera.buf[2] = NULL;
    fourCamera.leftCamera.buf[3] = NULL;
    fourCamera.leftCamera.data = NULL;
    fourCamera.leftCamera.fd = -1;
    fourCamera.leftCamera.n_buffers = 0;
    setCameraName(&(fourCamera.leftCamera),"/dev/video2");
    init(&(fourCamera.leftCamera),leftCameraWhich);


    memset(fourCamera.rightCamera.dev_name, '\0', sizeof(fourCamera.rightCamera.dev_name));
    fourCamera.rightCamera.buf[0] = NULL;
    fourCamera.rightCamera.buf[1] = NULL;
    fourCamera.rightCamera.buf[2] = NULL;
    fourCamera.rightCamera.buf[3] = NULL;
    fourCamera.rightCamera.data = NULL;
    fourCamera.rightCamera.fd = -1;
    fourCamera.rightCamera.n_buffers = 0;
    setCameraName(&(fourCamera.rightCamera),"/dev/video3");
    init(&(fourCamera.rightCamera),rightCameraWhich);
#endif


    int in_buffer_size = countCamera * width * height * 2;
//    int in_buffer_size =  width * height * 2;

        for (int j=0;j<bufferCount;j++)
        {
#if 1
            g_g2d_src_buffer[j] = g2d_alloc(in_buffer_size, 0);

            if (g_g2d_src_buffer[j] == 0)
            {
                qDebug("can not malloc memory for g2d source\n") ;
                exit(-1);
            }
#endif
        }

    int out_buffer_size = countCamera * width * height *4;
//    int out_buffer_size = width * height *4;
    for (int i=0;i<bufferCount;i++)
    {
#if 1
        g_g2d_dst_buffer[i] =g2d_alloc(out_buffer_size, 0);

        if (g_g2d_dst_buffer[i] == 0)
        {
            qDebug("can not malloc memory for g2d dst\n");
            g2d_free(g_g2d_src_buffer[i]);
            exit(-1);
        }

        memset(g_g2d_dst_buffer[i]->buf_vaddr,0x0,out_buffer_size);
#endif

    }

}


int CameraWorkFourin::v4l_capture_setup(CameraStruct *camera)
{

        struct v4l2_capability cap;
        struct v4l2_cropcap cropcap;
        struct v4l2_control ctl;
        struct v4l2_crop crop;
        struct v4l2_format fmt;
        struct v4l2_requestbuffers req;
    //struct v4l2_dbg_chip_ident chip;
        struct v4l2_streamparm parm;
        v4l2_std_id id;
        unsigned int min;

        if (ioctl (camera->fd, VIDIOC_QUERYCAP, &cap) < 0) {
                if (EINVAL == errno) {
                        qDebug ("%s is no V4L2 device\n",
                                        camera->dev_name);
                        return TFAIL;
                } else {
                        qDebug( "%s isn not V4L device,unknow error\n",
                        camera->dev_name);
                        return TFAIL;
                }
        }

        if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
                qDebug ( "%s is no video capture device\n",
                        camera->dev_name);
                return TFAIL;
        }

        if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
                qDebug ( "%s does not support streaming i/o\n",
                        camera->dev_name);
                return TFAIL;
        }

        if (ioctl(camera->fd, VIDIOC_S_INPUT, &g_input) < 0) {
                qDebug("VIDIOC_S_INPUT failed\n");
                close(camera->fd);
                return TFAIL;
        }

        memset(&cropcap, 0, sizeof(cropcap));
        cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (ioctl (camera->fd, VIDIOC_CROPCAP, &cropcap) < 0) {
                crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                crop.c = cropcap.defrect; /* reset to default */

                if (ioctl (camera->fd, VIDIOC_S_CROP, &crop) < 0) {
                        switch (errno) {
                                case EINVAL:
                                        /* Cropping not supported. */
                                        qDebug ( "%s  doesn't support crop\n",
                                                camera->dev_name);
                                        break;
                                default:
                                        /* Errors ignored. */
                                        break;
                        }
                }
        } else {
                /* Errors ignored. */
        }

        parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        parm.parm.capture.timeperframe.numerator = 1;
        parm.parm.capture.timeperframe.denominator = 0;
        parm.parm.capture.capturemode = 0;
        if (ioctl(camera->fd, VIDIOC_S_PARM, &parm) < 0) {
                qDebug("VIDIOC_S_PARM failed\n");
                close(camera->fd);
                return TFAIL;
        }

        memset(&fmt, 0, sizeof(fmt));

        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width = 0;
        fmt.fmt.pix.height = 0;
        fmt.fmt.pix.pixelformat = g_in_fmt;
        fmt.fmt.pix.field = V4L2_FIELD_ANY;
        if (ioctl (camera->fd, VIDIOC_S_FMT, &fmt) < 0) {
                qDebug ( "%s iformat not supported \n",
                        camera->dev_name);
                return TFAIL;
        }

        /* Note VIDIOC_S_FMT may change width and height. */

        /* Buggy driver paranoia. */
        min = fmt.fmt.pix.width * 2;
        if (fmt.fmt.pix.bytesperline < min)
                fmt.fmt.pix.bytesperline = min;

        min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
        if (fmt.fmt.pix.sizeimage < min)
                fmt.fmt.pix.sizeimage = min;

        if (ioctl(camera->fd, VIDIOC_G_FMT, &fmt) < 0) {
                qDebug("VIDIOC_G_FMT failed\n");
                close(camera->fd);
                return TFAIL;
        }

        file_length = fmt.fmt.pix.sizeimage;

        g_in_width = fmt.fmt.pix.width;
        g_in_height = fmt.fmt.pix.height;
        qDebug("g_in_width = %d, g_in_height = %d.\r\n", g_in_width, g_in_height);

        if (g_g2d_render) {
                switch (g_in_fmt) {
                        case V4L2_PIX_FMT_RGB565:
                                g_frame_size = g_in_width * g_in_height * 2;
                                //g_g2d_fmt = G2D_RGB565;
                                break;

                        case V4L2_PIX_FMT_UYVY:
                                g_frame_size = g_in_width * g_in_height * 2;
                                //g_g2d_fmt = G2D_UYVY;
                                break;

                        case V4L2_PIX_FMT_YUYV:
                                g_frame_size = g_in_width * g_in_height * 2;
                                //g_g2d_fmt = G2D_YUYV;
                                break;

                        case V4L2_PIX_FMT_YUV420:
                                g_frame_size = g_in_width * g_in_height * 3 / 2;
                                //g_g2d_fmt = G2D_I420;
                                break;

                        case V4L2_PIX_FMT_NV12:
                                g_frame_size = g_in_width * g_in_height * 3 / 2;
                                //g_g2d_fmt = G2D_NV12;
                                break;

                        default:
                                qDebug("Unsupported format.\n");
                                return TFAIL;
                }
        }

        memset(&req, 0, sizeof (req));
        req.count = g_capture_num_buffers;
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = (v4l2_memory)g_mem_type;
        if (ioctl (camera->fd, VIDIOC_REQBUFS, &req) < 0) {
                if (EINVAL == errno) {
                        qDebug ( "%s does not support "
                                         "memory mapping\n", camera->dev_name);
                        return TFAIL;
                } else {
                        qDebug ( "%s does not support "
                                         "memory mapping, unknow error\n", camera->dev_name);
                        return TFAIL;
                }
        }

        if (req.count < 2) {
                qDebug ( "Insufficient buffer memory on %s\n",
                         camera->dev_name);
                return TFAIL;
        }

        return TPASS;
}

int CameraWorkFourin::start_capturing(CameraStruct *camera)
{
        unsigned int i;
        struct v4l2_buffer buf;
        enum v4l2_buf_type type;

        for (i = 0; i < g_capture_num_buffers; i++) {

         camera->buf[i] = (struct testbuffer*) calloc(1, file_length); //内存中建立对应空间

            if (camera->buf[i] == NULL)
            {
                qDebug("camera.buf is NULL\n");
                close(camera->fd);
                return TFAIL;
            }

                memset(&buf, 0, sizeof (buf));
                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory =(v4l2_memory) g_mem_type;
                buf.index = i;
                if (g_mem_type == V4L2_MEMORY_USERPTR) {
                        buf.length = camera->buf[i]->length;
                        buf.m.userptr = (unsigned long)camera->buf[i]->offset;
                }
                if (ioctl(camera->fd, VIDIOC_QUERYBUF, &buf) < 0) {
                        qDebug("VIDIOC_QUERYBUF error\n");
                        return TFAIL;
                }

                if (g_mem_type == V4L2_MEMORY_MMAP) {
                        camera->buf[i]->length = buf.length;
                        camera->buf[i]->offset = (size_t) buf.m.offset;
                        camera->buf[i]->start = (unsigned char *) mmap(NULL, camera->buf[i]->length,
                            PROT_READ | PROT_WRITE, MAP_SHARED,
                            camera->fd, camera->buf[i]->offset);
                        memset(camera->buf[i]->start, 0xFF, camera->buf[i]->length);
                }
        }

        for (i = 0; i < g_capture_num_buffers; i++) {
                memset(&buf, 0, sizeof (buf));
                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = (v4l2_memory)g_mem_type;
                buf.index = i;
                if (g_mem_type == V4L2_MEMORY_USERPTR)
                        buf.m.offset = (unsigned int)camera->buf[i]->start;
                else
                        buf.m.offset = camera->buf[i]->offset;
                buf.length = camera->buf[i]->length;
                if (ioctl(camera->fd, VIDIOC_QBUF, &buf) < 0) {
                        qDebug("VIDIOC_QBUF error\n");
                        return TFAIL;
                }
        }

        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (ioctl (camera->fd, VIDIOC_STREAMON, &type) < 0) {
                qDebug("VIDIOC_STREAMON error\n");
                return TFAIL;
        }
        return TPASS;
}

int CameraWorkFourin::prepare_g2d_buffers(CameraStruct *camera,int theCamera)
{
    int i;

    for (i = 0; i < g_capture_num_buffers; i++) {
        if(!g_g2d_src_buffer[i]) {
            printf("Fail to allocate physical memory for image buffer!\n");
            return TFAIL;
        }

        camera->buf[i]->start = (unsigned char *)g_g2d_src_buffer[i]->buf_vaddr;
        camera->buf[i]->offset = g_g2d_src_buffer[i]->buf_paddr;
        camera->buf[i]->length = g_frame_size;
    }

    return 0;
}


//////////////////////////////////////////////////////
//获取一帧数据
//////////////////////////////////////////////////////
int CameraWorkFourin::read_frame(CameraStruct *camera,int theCamera)
{

}


//工作线程
void CameraWorkFourin::init(CameraStruct *camera,int theCamera)
{
    if ((camera->fd = open(camera->dev_name, O_RDWR, 0)) < 0) {
    qDebug("Unable to open %s\n", camera->dev_name);
            return;
    }

    if (v4l_capture_setup(camera) < 0) {
            qDebug("Setup v4l capture failed.\n");
            return ;
    }

#if 0
    if (prepare_g2d_buffers(camera) < 0) {
        qDebug("prepare_g2d_buffers failed\n");
        return;
    }
#endif

#if 1
    if (start_capturing(camera) < 0) {
            qDebug("start_capturing failed\n");
            return;
    }
#endif


}

void CameraWorkFourin::setWhichCamera(int theCamera)
{
    this->theCamera = theCamera;
}

void CameraWorkFourin::run()
{
    while(1)
    {

        QTime time;
        time.start();

            cameraRun(&(fourCamera.topCamera),topCameraWhich);
            cameraRun(&(fourCamera.bottomCamera),bottomCameraWhich);
            cameraRun(&(fourCamera.leftCamera),leftCameraWhich);
            cameraRun(&(fourCamera.rightCamera),rightCameraWhich);

//            qDebug()<<time.elapsed()/1000.0<<"s";

//                    QTime time;
//                    time.start();

//            YUYV2BGRA(g_g2d_src_buffer[offsetCount],width,height * countCamera,g_g2d_dst_buffer[offsetCount],width,height * countCamera);
            qDebug()<<time.elapsed()/1000.0<<"s";

            emit Frame(offsetCount);

            offsetCount++;

            if (offsetCount % count == 0)
            {
                offsetCount = 0;
            }

//            qDebug()<<time.elapsed()/1000.0<<"s";
    }
}

void CameraWorkFourin::cameraRun(CameraStruct *camera,int theCamera)
{
    struct v4l2_buffer capture_buf;

    for (int i = 0; i < g_frame_count; i++) {
            memset(&capture_buf, 0, sizeof(capture_buf));
            capture_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            capture_buf.memory = (v4l2_memory)g_mem_type;

            if (ioctl(camera->fd, VIDIOC_DQBUF, &capture_buf) < 0) {
                    qDebug("VIDIOC_DQBUF failed.\n");
                    return ;
            }

#if 0
            int fd_sdma;

            fd_sdma = ::open("/dev/sdma_test",O_RDWR);

            if(fd_sdma < 0){
              qDebug("Failed open /dev/sdma_test \n");
              exit(-1);
            }

            int offsetForReadWrite = 0;

            while (1)
            {
                if ( ( ( ( (camera->buf)[capture_buf.index])->length ) - offsetForReadWrite) > 0 )
                {
                    ::write(fd_sdma, (((camera->buf)[capture_buf.index])->start)+offsetForReadWrite, 0);
                    ::read(fd_sdma, (uchar*)(g_g2d_src_buffer[offsetCount]->buf_vaddr) + (theCamera * 2 *width *height)+offsetForReadWrite,0);
                    offsetForReadWrite += 2048000;
                }
                else
                {
                    break;
                }
            }

            ::close(fd_sdma);
#else
            memcpy(((uchar*)(g_g2d_src_buffer[offsetCount]->buf_vaddr) + (theCamera * 2 *width *height)),((camera->buf)[capture_buf.index])->start, ((camera->buf)[capture_buf.index])->length);

//             YUYV2BGRA(g_g2d_src_buffer[theCamera][offsetCount],width,height,g_g2d_dst_buffer[theCamera],width,height);

#endif

//              (unsigned char * )(g_g2d_src_buffer[offsetCount]->buf_vaddr) + (int)(theCamera * 2 *width *height) = ((camera->buf)[capture_buf.index])->start;

//            mmCopy((struct g2d_buf *)((uchar *)(g_g2d_src_buffer[offsetCount]->buf_vaddr) + (theCamera * 2 *width *height)),(struct g2d_buf *)((camera->buf)[capture_buf.index])->start,((camera->buf)[capture_buf.index])->length);

//            YUYV2BGRA(g_g2d_src_buffer[offsetCount],width,height,g_g2d_dst_buffer[offsetCount],width,height);

            if (ioctl(camera->fd, VIDIOC_QBUF, &capture_buf) < 0) {
                    qDebug("VIDIOC_QBUF failed\n");
                    return ;
            }
    }
    return ;
}

void CameraWorkFourin::uninit(CameraStruct *camera)
{
    enum v4l2_buf_type type;

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(camera->fd, VIDIOC_STREAMOFF, &type);

    for (int i = 0; i < g_capture_num_buffers; i++) {
        munmap(camera->buf[i]->start, camera->buf[i]->length);
    }

    close(camera->fd);
}

void CameraWorkFourin::setCameraName(CameraStruct *camera,QString name)
{
    this->cameraName = name;
    memcpy(&(camera->dev_name),cameraName.toLatin1().data(),sizeof(camera->dev_name));
}

unsigned char * CameraWorkFourin::setCameraData(unsigned char **data)
//unsigned char * CameraWorkFourin::setCameraData(unsigned char *data)
{
    *data = new unsigned char [count *countCamera * 2 * width *height];
    startData = *data;
    return *data;
}

CameraWorkFourin::~CameraWorkFourin()
{
    uninit(&(fourCamera.topCamera));
    uninit(&(fourCamera.bottomCamera));
    uninit(&(fourCamera.leftCamera));
    uninit(&(fourCamera.rightCamera));
}
