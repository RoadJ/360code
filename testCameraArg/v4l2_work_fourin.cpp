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

#include "v4l2_work_fourin.h"
#include <QDebug>

#define CLEAR(x) memset (&(x), 0, sizeof (x))

#define topCameraWhich 0
#define bottomCameraWhich 1
#define leftCameraWhich 2
#define rightCameraWhich 3

//////////////////////////////////////////////////////
CameraWorkFourin::CameraWorkFourin(QObject *parent)
{
    memset(fourCamera.topCamera.dev_name, '\0', sizeof(fourCamera.topCamera.dev_name));
    fourCamera.topCamera.buf = NULL;
    fourCamera.topCamera.data = NULL;
    fourCamera.topCamera.fd = -1;
    fourCamera.topCamera.n_buffers = 0;
    setCameraName(&(fourCamera.topCamera),"/dev/video4");
    init(&(fourCamera.topCamera));


    memset(fourCamera.bottomCamera.dev_name, '\0', sizeof(fourCamera.bottomCamera.dev_name));
    fourCamera.bottomCamera.buf = NULL;
    fourCamera.bottomCamera.data = NULL;
    fourCamera.bottomCamera.fd = -1;
    fourCamera.bottomCamera.n_buffers = 0;
    setCameraName(&(fourCamera.bottomCamera),"/dev/video5");
    init(&(fourCamera.bottomCamera));


    memset(fourCamera.leftCamera.dev_name, '\0', sizeof(fourCamera.leftCamera.dev_name));
    fourCamera.leftCamera.buf = NULL;
    fourCamera.leftCamera.data = NULL;
    fourCamera.leftCamera.fd = -1;
    fourCamera.leftCamera.n_buffers = 0;
    setCameraName(&(fourCamera.leftCamera),"/dev/video6");
    init(&(fourCamera.leftCamera));


    memset(fourCamera.rightCamera.dev_name, '\0', sizeof(fourCamera.rightCamera.dev_name));
    fourCamera.rightCamera.buf = NULL;
    fourCamera.rightCamera.data = NULL;
    fourCamera.rightCamera.fd = -1;
    fourCamera.rightCamera.n_buffers = 0;
    setCameraName(&(fourCamera.rightCamera),"/dev/video7");
    init(&(fourCamera.rightCamera));

    int in_buffer_size = countCamera * width * height * 2;

    for (int i=0;i<bufferCount;i++)
    {
#if 0
        g_g2d_src_buffer[i] = g2d_alloc(in_buffer_size, 0);

        if (g_g2d_src_buffer[i] == 0)
        {
            qDebug() << "can not malloc memory for g2d source";
            exit(-1);
        }
#endif
    }

    int out_buffer_size = countCamera * width * height *2;

    for (int i=0;i<bufferCount;i++)
    {
#if 0
        g_g2d_dst_buffer[i] =g2d_alloc(out_buffer_size, 0);

        if (g_g2d_dst_buffer[i] == 0)
        {
            qDebug() << "can not malloc memory for g2d dst";
            g2d_free(g_g2d_src_buffer[i]);
            exit(-1);
        }

        memset(g_g2d_dst_buffer[i]->buf_vaddr,0x0,out_buffer_size);
#endif

    }

}


//////////////////////////////////////////////////////
//获取一帧数据
//////////////////////////////////////////////////////
int CameraWorkFourin::read_frame(CameraStruct *camera,int theCamera)
{

    //pthread_mutex_lock (&hashlock);

    //qDebug("theCamera = %d\n",theCamera);

    struct v4l2_buffer buf;

    CLEAR(buf);
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    //buf.length = 2*width*height;
    /*8.出队列以取得已采集数据的帧缓冲，取得原始采集数据。VIDIOC_DQBUF*/
    int ff = ioctl(camera->fd, VIDIOC_DQBUF, &buf);
    if (ff < 0)
    {
        qDebug("read buffer is failture\n"); //出列采集的帧缓冲
        return -1;
    }

    assert(offsetCount < count );

#if 1
    //写入内存文件
    memcpy(((uchar*)(startData) + ((offsetCount) *countCamera * 2 *width *height) + (theCamera * 2 *width *height)),((camera->buf)[buf.index]).start, ((camera->buf)[buf.index]).length);
    //memcpy(((uchar*)(g_g2d_src_buffer[offsetCount]->buf_vaddr) + (theCamera * 2 *width *height)),((camera->buf)[buf.index]).start, ((camera->buf)[buf.index]).length);
#else
    FILE * file_fd;
    file_fd = fopen("test.jpg","w+");
    fwrite(((camera.buf)[buf.index]).start, ((camera.buf)[buf.index]).length,1,file_fd); //将其写入文件中
    fclose(file_fd);
#endif

    /*9.将缓冲重新入队列尾,这样可以循环采集。VIDIOC_QBUF*/
    ff = ioctl(camera->fd, VIDIOC_QBUF, &buf); //再将其入列
    if (ff < 0) //把数据从缓存中读取出来
        qDebug("failture VIDIOC_QBUF\n");

    return 1;
}


//工作线程
void CameraWorkFourin::init(CameraStruct *camera)
{
    struct v4l2_capability cap;
    struct v4l2_format fmt;
    unsigned int i;
    struct v4l2_input input;

    unsigned int min;
    int ret;
    struct v4l2_standard standard;
    v4l2_std_id std_id;

    //
    enum v4l2_buf_type type;
    //
    unsigned long file_length;

    /*1.打开设备文件。 int fd=open(”/dev/video0″,O_RDWR);*********/

    if (*(camera->dev_name) == '\0') {
        qDebug("you must use the i argment for command line\n");
        return ;
    }

    qDebug("camera.dev_name = %s",camera->dev_name);

    camera->fd = open(camera->dev_name,O_RDWR /* required */| O_NONBLOCK , 0); //打开设备
    if (camera->fd == -1)
    {
        qDebug("open device fail\n");
        return ;
    }

    qDebug("open this device is ok\n");

    /*2.取得设备的capability，看看设备具有什么功能，比如是否具有视频输入,或者音频输入输出等。VIDIOC_QUERYCAP,struct v4l2_capability*/
    int ff = ioctl(camera->fd, VIDIOC_QUERYCAP, &cap); //获取摄像头参数
    if (ff < 0) {
        qDebug("failture VIDIOC_QUERYCAP\n");
        close(camera->fd);
        return ;
    }
    //打印设备的capability
    qDebug("DriverName:%s\nCard Name:%s\nBus info:%s\nDriverVersion:%u.%u.%u\n",cap.driver,cap.card,cap.bus_info,(cap.version>>16)&0xff,(cap.version>>8)&0xff,cap.version&0xff);

    //判断是不是摄像头
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        qDebug ( "%s is no video capture device\n",camera->dev_name);
        close(camera->fd);
        return ;
    }
    qDebug("this device is a capture\n");

    //判断是不是流设备
    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        qDebug ( "%s does not support streaming i/o\n",camera->dev_name);
        close(camera->fd);
        return ;
    }
    qDebug("this capture is stream device\n");

    //如果是数字摄像头，就执行if语言。
    //如果是模拟摄像头，则跳过
    if(analog != true)
    {
        //读取当前的视频输入格式
        if (ioctl(camera->fd, VIDIOC_G_STD, &std_id) < 0)
        {
            qDebug("VIDIOC_G_STD failed\n");
            close(camera->fd);
            return ;
        }
        qDebug("getting VIDIOC_G_STD = %llx is ok \n",std_id);

        //枚举当前的视频输入格式，得名字
        memset(&standard, 0, sizeof(standard));
        standard.index = 0;

        while (0 == ioctl(camera->fd, VIDIOC_ENUMSTD, &standard)) {
            if (standard.id & std_id) {
                printf("Current video standard: %s\n", standard.name);
                break;
            }

            standard.index++;
        }

        //设置输入制式
        std_id = V4L2_STD_NTSC;
        if (ioctl(camera->fd, VIDIOC_S_STD, &std_id) < 0)
        {
        qDebug("VIDIOC_S_STD failed\n");
        close(camera->fd);
        return ;
        }
        qDebug("setting VIDIOC_S_STD = V4L2_STD_NTSC is ok\n");
    }

    //取得当前驱动的格式
    CLEAR(fmt);
    fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(camera->fd,VIDIOC_G_FMT,&fmt);
    if (ret == -1)
    {
        qDebug("ioctl VIDIOC_G_FMT fail\n");
        close(camera->fd);
        return ;
    }
    qDebug("Currentdata format information:\n\twidth:%d\n\theight:%d\n",fmt.fmt.pix.width,fmt.fmt.pix.height);

    /*3.设置视频的制式和帧格式，制式包括PAL，NTSC，帧的格式个包括宽度和高度等。*/
    struct v4l2_fmtdesc fmt1;
    memset(&fmt1, 0, sizeof(fmt1));
    fmt1.index = 0;
    fmt1.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    //获取当前驱动支持的视频格式
    while(ioctl(camera->fd,VIDIOC_ENUM_FMT,&fmt1)!=-1)
    {
        if(fmt1.pixelformat & fmt.fmt.pix.pixelformat)
        {
            qDebug("\tformat:%s = %d \n",fmt1.description,fmt1.pixelformat);
            break;
        }

        qDebug("\tformat:%s = %d \n",fmt1.description,fmt1.pixelformat);

        fmt1.index++;
    }    //帧的格式，比如宽度，高度等

    CLEAR(fmt);
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; //数据流类型，必须永远是V4L2_BUF_TYPE_VIDEO_CAPTURE
    fmt.fmt.pix.width = width; //宽，必须是16的倍数
    fmt.fmt.pix.height = height; ////高，必须是16的倍数
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; //视频数据存储类型//V4L2_PIX_FMT_YUYV;//V4L2_PIX_FMT_YVU420;//V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    qDebug("\n\n i want setting ");
    qDebug("fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE\n");
    qDebug("fmt.fmt.pix.width = 720\n");
    qDebug("fmt.fmt.pix.height = 480\n");
    qDebug("fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV\n");
    qDebug("fmt.fmt.pix.field = V4L2_FIELD_INTERLACED\n");

    //设置当前驱动的频捕获格式
    ff = ioctl(camera->fd, VIDIOC_S_FMT, &fmt);
    if (ff < 0)
    {
        qDebug("failture VIDIOC_S_FMT\n");
        close(camera->fd);
        return ;
    }
    qDebug("\n\nVIDIOC_S_FMT is ok\n");

    //重新读取当前投置的值
    CLEAR(fmt);
    fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(camera->fd,VIDIOC_G_FMT,&fmt);
    if (ret == -1)
    {
        qDebug("ioctl VIDIOC_G_FMT fail\n");
        close(camera->fd);
        return ;
    }
    qDebug("Currentdata format information:\n\twidth:%d\n\theight:%d\n",fmt.fmt.pix.width,fmt.fmt.pix.height);


    //计算图片大小
    min = fmt.fmt.pix.width * 2;
    if (fmt.fmt.pix.bytesperline < min)
    fmt.fmt.pix.bytesperline = min;
    min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
    if (fmt.fmt.pix.sizeimage < min)
    fmt.fmt.pix.sizeimage = min;

    file_length = fmt.fmt.pix.sizeimage;

    /*4.向驱动申请帧缓冲，一般不超过5个。struct v4l2_requestbuffers*/
    struct v4l2_requestbuffers req;
    CLEAR(req);
    req.count = count; //缓存数量，也就是说在缓存队列里保持多少张照片
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP; //或V4L2_MEMORY_USERPTR

    ff = ioctl(camera->fd, VIDIOC_REQBUFS, &req); //申请缓冲，count是申请的数量
    if (ff < 0)
    {
        qDebug("failture VIDIOC_REQBUFS\n");
        close(camera->fd);
        return ;
    }
    if (req.count < count)
    {
        qDebug("Insufficient buffer memory\n");
        close(camera->fd);
        return ;
    }
    qDebug("VIDIOC_REQBUFS is ok\n");


    camera->buf = (struct buffer*) calloc(req.count, file_length); //内存中建立对应空间

    if (camera->buf == NULL)
    {
        qDebug("camera.buf is NULL\n");
        close(camera->fd);
        return ;
    }

    qDebug("camera.buf is ok\n");

    /*5.将申请到的帧缓冲映射到用户空间，这样就可以直接操作采集到的帧了，而不必去复制。mmap*/
    for (camera->n_buffers = 0; ( (camera->n_buffers) <  (req.count)  ); ++(camera->n_buffers))
    {
        struct v4l2_buffer buf; //驱动中的一帧
        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = camera->n_buffers;
        //buf.length = 2 * width * height;
        //把VIDIOC_REQBUFS中分配的数据缓存转换成物理地址
        if (-1 == ioctl(camera->fd, VIDIOC_QUERYBUF, &buf)) //映射用户空间
        {
            qDebug("VIDIOC_QUERYBUF error\n");
            close(camera->fd);
            return ;
        }

        camera->buf[camera->n_buffers].length = buf.length;
        camera->buf[camera->n_buffers].start = mmap(NULL /* start anywhere */, buf.length,
        PROT_READ | PROT_WRITE /* required */,
        MAP_SHARED /* recommended */, camera->fd, buf.m.offset); //通过mmap建立映射关系,返回映射区的起始地址

        if (MAP_FAILED == camera->buf[camera->n_buffers].start)
        {
            qDebug("mmap failed\n");
            close(camera->fd);
            return ;
        }
    }
    /*6.将申请到的帧缓冲全部入队列，以便存放采集到的数据.VIDIOC_QBUF,struct v4l2_buffer*/
    for (i = 0; i < camera->n_buffers; ++i)
    {
        //i = 0;
        struct v4l2_buffer buf;
        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        //把数据从缓存中读取出来
        if (-1 == ioctl(camera->fd, VIDIOC_QBUF, &buf)) //申请到的缓冲进入列队
            qDebug("VIDIOC_QBUF failed\n");
    }

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    /*7.开始视频的采集。VIDIOC_STREAMON*/
    if (-1 == ioctl(camera->fd, VIDIOC_STREAMON, &type)) //开始捕捉图像数据
        qDebug("VIDIOC_STREAMON failed\n");

    return ;
}

void CameraWorkFourin::setWhichCamera(int theCamera)
{
    this->theCamera = theCamera;
}

void CameraWorkFourin::run()
{
    while(1)
    {
        //switch (theCamera)
        {
            //case topCameraWhich:
                cameraRun(&(fourCamera.topCamera),topCameraWhich);
                //break;

            //case bottomCameraWhich:
                cameraRun(&(fourCamera.bottomCamera),bottomCameraWhich);
                //break;

            //case leftCameraWhich:
                cameraRun(&(fourCamera.leftCamera),leftCameraWhich);
                //break;

            //case rightCameraWhich:
                cameraRun(&(fourCamera.rightCamera),rightCameraWhich);
                //break;

        //default:
            //break;
        }

        //memcpy(g_g2d_src_buffer[offsetCount]->buf_vaddr,startData + ((offsetCount) *countCamera * 2 *width *height) , countCamera * 2 * width * height);
        //YUYV2BGRA(g_g2d_src_buffer[offsetCount],width,height * countCamera,g_g2d_dst_buffer[offsetCount],width,height * countCamera);

        emit Frame(offsetCount);

        offsetCount++;

        if (offsetCount % count == 0)
        {
            offsetCount = 0;
        }
    }
}

void CameraWorkFourin::cameraRun(CameraStruct *camera,int theCamera)
{
    for (;;) //这一段涉及到异步IO
    {
        fd_set fds;
        struct timeval tv;
        int r;

        FD_ZERO(&fds); //将指定的?件描述符集清空
        FD_SET(camera->fd, &fds); //在文件描述符集合中增鍔????个新的文件描述符

        /* Timeout. */
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        r = select((camera->fd) + 1, &fds, NULL, NULL, &tv); //判断是否可读（即摄像头是否准备好），tv是定时

        if (-1 == r) {
            if (EINTR == errno)
                continue;
            printf("select err\n");
        }
        if (0 == r) {
            qDebug( "select timeout from %s\n", camera->dev_name);
            close(camera->fd);
            return ;
        }

        if (read_frame(camera,theCamera)) //如果可读，执行read_frame ()函数，并跳出循环
        {
            break;
        }
    }
}

void CameraWorkFourin::uninit(CameraStruct *camera)
{
    for (int i = 0; i < camera->n_buffers; ++i)
        if (-1 == munmap(camera->buf->start, camera->buf->length))
            printf("munmap error");

    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    /*10.停止视频的采集。VIDIOC_STREAMOFF*/
    if (-1 == ioctl(camera->fd, VIDIOC_STREAMOFF, &type))
        printf("VIDIOC_STREAMOFF");

    /*11.关闭视频设备。close(fd);*/
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
